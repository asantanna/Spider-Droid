using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace Phi_Core {

  static class PhiLink {

    //
    // CONST
    //

    // Phi Core listening port ... Lucas's bday ... :-)
    const Int32 PHI_LINK_PORT = 1122;

    // Phi Link communication rate
    internal const double DESIRED_LOOP_FPS = 50;
    const double DESIRED_SECS_PER_LOOP = 1.0 / DESIRED_LOOP_FPS;

    // statistics
    const double LOOP_TIME_ACCUM_RATE = (1.0 / 100);

    // motor indices
    internal enum MOTOR_IDX {
      IDX_RFT = 0,
      IDX_RFK = 1,
      IDX_LFT = 2,
      IDX_LFK = 3,
      IDX_RBT = 4,
      IDX_RBK = 5,
      IDX_LBT = 6,
      IDX_LBK = 7,
      IDX_RFH = 8,
      IDX_LFH = 9,
      IDX_RBH = 10,
      IDX_LBH = 11,
    };

    //
    //  VARS
    //

    internal enum PhiLinkState {
      LINK_OFF,
      LINK_STARTED,
      LINK_CONNECTING,
      LINK_CONNECTED,
      LINK_ERROR,
      LINK_CLOSED
    }

    internal static object dataLock = new object();

    internal static PhiLinkState linkState = PhiLinkState.LINK_OFF;

    private static Task commLoopTask = null;
    private static UInt32 loopCount = 0;
    private static double secsThisLoop = 0;
    private static double accum_secsPerLoop = DESIRED_SECS_PER_LOOP;
    private static double accum_sleepTime = 0;

    // state of PHI

    private static double accum_pitch = 0;
    private static double accum_yaw = 0;
    private static double accum_roll = 0;

    private static UInt16[] joints = new UInt16[PhiGlobals.NUM_MOTOR_ELEM];

    //
    // CODE
    //

    async static internal Task<NetworkStream> startListenerAsync() {

      // reset some vars
      accum_pitch = 0;
      accum_yaw = 0;
      accum_roll = 0;

      // create listener (accept connections from any address)
      TcpListener phiLinkListener = new TcpListener(IPAddress.Any, PHI_LINK_PORT);
      Task<TcpClient> clientTask = null;

      // change status to initializing
      setLinkState(PhiLinkState.LINK_STARTED);

      try {
        // enable listener
        phiLinkListener.Start();

        // change status to waiting
        setLinkState(PhiLinkState.LINK_CONNECTING);

        // wait async for a phi to connect
        clientTask = phiLinkListener.AcceptTcpClientAsync();

      } catch (SocketException e) {
        // something went wrong
        setLinkState(PhiLinkState.LINK_ERROR);
        Console.WriteLine("PhiLink.startListener: got exception when starting listener: SocketException: {0}", e);
        return null;
      }

      // wait async for connection
      //
      // The await keyword causes an immediate return to the caller with a "pending" Task<TcpClient>
      // The caller must do an await (or Task. to get the client member.  At which point it could:
      //   1) block if it is not async itself
      //   2) return to *its* parent, if our caller is also async

      TcpClient client =  await clientTask;

      // We get here asynchronously when AcceptTcpClientAsync()
      // completes (when a connection occurs);

      // change status to connected
      setLinkState(PhiLinkState.LINK_CONNECTED);

      // turn off "Nagle" algorithm so data is sent immediately
      Socket s = client.Client;
      s.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay, true);

      // Get a stream object for reading/writing
      NetworkStream stream = client.GetStream();

      // Note: this return statement is not *actually* a conventional
      // "return" statement because this is an async function (note that
      // the type is different than declared).  Instead, what "return" 
      // does here is to fill in the NetworkStream part of Task<NetworkStream>
      // and wake up any "awaiters" on it.

      return stream;
    }

    internal static void startCommLoop(NetworkStream phiStream) {
      if (commLoopTask == null) {
        // start comm loop
        commLoopTask = Task.Run(() => commLoop(phiStream));
      } else {
        // already started - TODO - do something here
      }
    }

    internal static void stopCommLoop() {
      if (commLoopTask != null) {
        // HACK - do something here - don't know how to stop a Task
        // maybe that cancel token thing?
      }
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
      double sleepError = 0;

      // loop forever
      
      long loopStartTicks = stopWatch.ElapsedTicks;
      
      while (true) {

        // grab Eve outputs
        // eve.writeOutputs(cmdPacket.packetData);

        // start task to send eve outputs as command packet to PHI
        Task writeTask = Task.Run(() => phiStream.Write(cmdPacket.packetData, 0, cmdPacket.Length));

        // start task to read PHI's status
        Task readTask = Task.Run(() => phiStream.Read(statePacket.packetData, 0, statePacket.Length));

        // wait for read to complete - right now this is just like blocking to begin with
        readTask.Wait();

        // write sensor states into Eve
        parseState(statePacket);
        // eve.writeInputs();

        // step Eve
        // eve.Step();

        // HACK - for now just wait for write to complete before we send the next packet
        writeTask.Wait();

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
          accum_secsPerLoop = ((1-LOOP_TIME_ACCUM_RATE) * accum_secsPerLoop) + (LOOP_TIME_ACCUM_RATE * secsThisLoop);
          accum_sleepTime =   ((1-LOOP_TIME_ACCUM_RATE) * accum_sleepTime)   + (LOOP_TIME_ACCUM_RATE * sleepTime);
        }

        // compute sleep error
        // new_error = (desired_loop + curr_error) - actual_loop
        sleepError +=  DESIRED_SECS_PER_LOOP - secsThisLoop;

        // time next
        loopStartTicks = loopEndTicks;
      }
      
      // if we get here, loop was cancelled
      // setLinkState(PhiLinkState.CANCELLED);
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
      double pitchDelta = statePacket.getPitchDelta();
      double yawDelta = statePacket.getYawDelta();
      double rollDelta = statePacket.getRollDelta();
      lock (dataLock) {
        accum_pitch = clampRange(accum_pitch + pitchDelta, 180);
        accum_yaw   = clampRange(accum_yaw   + yawDelta,   180);
        accum_roll  = clampRange(accum_roll  + rollDelta,  180);
        statePacket.getJointData(joints);
      }
    }

    internal static void resetGyroAccum() {
      lock (dataLock) {
        accum_pitch = 0;
        accum_yaw   = 0;
        accum_roll  = 0;
      }
    }


    //
    // Misc
    //

    internal static void setLinkState(PhiLinkState state) {
      linkState = state;
      PhiGlobals.mainWindow.updateLinkStatus(state);
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
        return loopCount;
    }

    internal static double getGyroAccumPitch() {
      lock (dataLock) {
        // return accum value [-180, 180]
        return accum_pitch;
      }
    }

    internal static double getGyroAccumYaw() {
      lock (dataLock) {
        // return accum value [-180, 180]
        return accum_yaw;
      }
    }

    internal static double getGyroAccumRoll() {
      lock (dataLock) {
        // return accum value [-180, 180]
        return accum_roll;
      }
    }

    internal static double getJointPos(int idx) {
      // A-to-D has 10 bit precision - convert to [0, 1.0]
      return joints[idx] / 1024.0;
    }

  }  // class
}  // namespace

