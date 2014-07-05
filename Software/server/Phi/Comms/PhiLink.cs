﻿using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace Phi {

  //
  // class PhiLink represents a TCP/IP server that Phi connects to in response
  // to the "startPhiLink" JSON command.  Once connection is established, PhiCore
  // initiates its comm loop and is the driver of the communication.
  //
  // In an infinite loop, PhiCore's commLoop() sends cmdPacket, reads statePacket
  // and then sleeps for an appropriate amount to keep up the desired comm rate.
  //

  static class PhiLink {

    //
    // CONST
    //

    // PhiLink listening port ... Lucas's bday ... :-)
    const Int32 PHI_LINK_PORT = 1122;

    // PhiLink communication rate
    internal const double DESIRED_LOOP_FPS = 20;
    const double DESIRED_SECS_PER_LOOP = 1.0 / DESIRED_LOOP_FPS;

    // adaptive sleep
    const double SLEEP_ERROR_ACCUM_RATE = (1.0 / 1e3);

    // statistics
    const double LOOP_STATS_ACCUM_RATE = (1.0 / 100);

    //
    //  VARS
    //

    // status of link
    
    internal enum PhiLinkStatus {
      LINK_OFF,
      LINK_STARTED,
      LINK_CONNECTING,
      LINK_CONNECTED,
      LINK_ERROR,
      LINK_CLOSED
    }

    internal static PhiLinkStatus linkStatus = PhiLinkStatus.LINK_OFF;

    private static Task commLoopTask = null;
    private static UInt32 loopCount = 0;
    private static double secsThisLoop = 0;
    private static double accum_secsPerLoop = DESIRED_SECS_PER_LOOP;
    private static double accum_sleepTime = 0;
    private static double sleepError = 0;

    private static UInt32 lastPacketID = 0;

    // sync object - this is used because many of the properties in this
    // class can be accessed by two different threads: directly by the
    // commLoop() thread in this class and by the UI thread through
    // the accessor methods
    
    internal static object dataLock = new object();

    // state of PHI

    private static double pitch = 0;
    private static double yaw = 0;
    private static double roll = 0;

    private static double[] joints = new double[PhiBasePacket.NUM_MOTOR_ELEM];

    //
    // CODE
    //

    static internal void phiLinkTask() {

      // reset some vars
      pitch = 0;
      yaw = 0;
      roll = 0;

      // create listener (accept connections from any address)
      TcpListener phiLinkListener = new TcpListener(IPAddress.Any, PHI_LINK_PORT);

      // change status to initializing
      setLinkStatus(PhiLinkStatus.LINK_STARTED);

      // connect

      TcpClient client = null;

      try {
        // enable listener
        phiLinkListener.Start();

        // change status to waiting
        setLinkStatus(PhiLinkStatus.LINK_CONNECTING);

        // wait for a phi to connect
        client = phiLinkListener.AcceptTcpClient();

      } catch (SocketException e) {
        // something went wrong
        setLinkStatus(PhiLinkStatus.LINK_ERROR);
        Console.WriteLine("PhiLink.startListener: got exception when starting listener: SocketException: {0}", e);
        return;
      }

      // change status to connected
      setLinkStatus(PhiLinkStatus.LINK_CONNECTED);

      // turn off "Nagle" algorithm so data is sent immediately
      Socket s = client.Client;
      s.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay, true);

      // Get a stream object for reading/writing
      NetworkStream stream = client.GetStream();

      // start comm loop  - only returns on error

      if (stream != null) {
        commLoop(stream);
      } else {
        // no stream? say error
        setLinkStatus(PhiLink.PhiLinkStatus.LINK_ERROR);
      }

      // comm loop returned
      return;
    }

    //
    // COMM LOOP
    //

    static void commLoop(NetworkStream phiStream) {

      // create PhiLink command and state packets
      PhiCmdPacket cmdPacket = new PhiCmdPacket();
      PhiStatePacket statePacket = new PhiStatePacket();

      // initialize Eve
      // eve.Init();

      // init statistics 
      double tickPeriod = 1.0 / Stopwatch.Frequency;
      Stopwatch stopWatch = Stopwatch.StartNew();

      // loop forever
      
      long loopStartTicks = stopWatch.ElapsedTicks;
      
      while (true) {

        // grab Eve outputs
        // eve.writeOutputs(cmdPacket.packetData);

        // send eve outputs as command packet to PHI
        phiStream.Write(cmdPacket.packetData, 0, cmdPacket.Length);

        // read PHI's status

        int nRead = 0;
        while (nRead < statePacket.Length) {
          nRead += phiStream.Read(statePacket.packetData, nRead, statePacket.Length - nRead);
        }

        // write sensor states into Eve
        
        parseState(statePacket);
        // eve.writeInputs();

        // step Eve
        // eve.Step();

        // count loop
        loopCount++;

        // sleep to achieve desired FPS
        long commEndTicks = stopWatch.ElapsedTicks;
        double commSecs = (commEndTicks - loopStartTicks) * tickPeriod;
        double sleepTime = DESIRED_SECS_PER_LOOP + sleepError - commSecs;

        if (sleepTime < 0) {
          sleepTime = 0;
        }

        Thread.Sleep((int)(sleepTime * 1000));

        // compute stats
        
        long loopEndTicks = stopWatch.ElapsedTicks;

        lock (dataLock) {
          // protect access
          secsThisLoop = (loopEndTicks - loopStartTicks) * tickPeriod;
          accum_secsPerLoop = ((1-LOOP_STATS_ACCUM_RATE) * accum_secsPerLoop) + (LOOP_STATS_ACCUM_RATE * secsThisLoop);
          accum_sleepTime =   ((1-LOOP_STATS_ACCUM_RATE) * accum_sleepTime)   + (LOOP_STATS_ACCUM_RATE * sleepTime);
        }

        // compute sleep error and with running avg so we don't go all over the place
        // new_error = (desired_loop + curr_error) - actual_loop
        sleepError = ((1-SLEEP_ERROR_ACCUM_RATE) * sleepError) + (SLEEP_ERROR_ACCUM_RATE*(DESIRED_SECS_PER_LOOP - secsThisLoop));

        // time next
        loopStartTicks = loopEndTicks;
      }
      
      // if we get here, loop was cancelled
      // setLinkStatus(PhiLinkStatus.CANCELLED);
    }

    private static double clampRange(double deg, double absMax) {

      if (deg > absMax) {
        deg -= absMax * 2;
      } else if (deg < -absMax) {
        deg += absMax * 2;
      }

      return deg;
    }

    private static void parseState(PhiStatePacket statePacket) {
      lock (dataLock) {
        lastPacketID = statePacket.getPacketID();
        pitch = statePacket.getPitch();
        yaw = statePacket.getYaw();
        roll = statePacket.getRoll();
        statePacket.getJointData(joints);
      }
    }

    internal static void resetGyroAccum() {
      lock (dataLock) {
        pitch = 0;
        yaw   = 0;
        roll  = 0;
      }
    }

    //
    // Misc
    //

    internal static void setLinkStatus(PhiLinkStatus status) {
      linkStatus = status;
      // notify change for UI
      PhiGlobals.mainWindow.notifyPhiLinkChanged();
    }

    internal static double getAvgIdle() {
      lock (dataLock) {
        return accum_sleepTime / DESIRED_SECS_PER_LOOP;
      }
    }

    internal static double getAvgFrameRate() {
      lock (dataLock) {
        return 1.0 / accum_secsPerLoop;
      }
    }

    internal static double getLoopCount() {
      lock (dataLock) {
        return loopCount;
      }
    }

    internal static UInt32 getLastPacketID() {
      lock (dataLock) {
        return lastPacketID;
      }
    }

    internal static double getSleepError() {
      lock (dataLock) {
        return sleepError;
      }
    }

    internal static double getGyroAccumPitch() {
      lock (dataLock) {
        // return change [0,1]?  HACK  (how about negative .. sheesh)
        return pitch;
      }
    }

    internal static double getGyroAccumYaw() {
      lock (dataLock) {
        // return change [0,1]?  HACK 
        return yaw;
      }
    }

    internal static double getGyroAccumRoll() {
      lock (dataLock) {
        // return change [0,1]?  HACK 
        return roll;
      }
    }

    internal static double getJointPos(int idx) {
      lock (dataLock) {
        // Phi returns canonical [0, 1.0] to represent [0, 360]
        return joints[idx];
      }
    }

  }  // class
}  // namespace

