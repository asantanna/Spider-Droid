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
    public const double DESIRED_LOOP_FPS = 80;
    public const double DESIRED_SECS_PER_LOOP = 1.0 / DESIRED_LOOP_FPS;

    // adaptive sleep
    const double SLEEP_ERROR_ACCUM_RATE = (1.0 / 1e3);

    // statistics
    const double LOOP_STATS_ACCUM_RATE = (1.0 / 100);

    //
    //  VARS
    //

    // status of link
    
    public enum PHI_LINK_STATUS {
      LINK_OFF,
      LINK_STARTED,
      LINK_CONNECTING,
      LINK_CONNECTED,
      LINK_DISCONNECTED,
      LINK_ERROR,
      LINK_CLOSED
    }

    public static PHI_LINK_STATUS linkStatus = PHI_LINK_STATUS.LINK_OFF;

    private static UInt32 loopCount = 0;
    private static double secsThisLoop = 0;
    private static double accum_secsPerLoop = DESIRED_SECS_PER_LOOP;
    private static double accum_sleepTime = 0;
    private static double sleepError = 0;

    private static UInt32 lastPacketID = 0;

    // acive Phi controllers
    private static IPhiController activeController = null;

    // sync object - this is used because some properties in this
    // class can be accessed by two different threads: by the commLoop()
    // thread and by the UI thread
    
    public static object dataLock = new object();

    //
    // CODE
    //

    static public void phiLinkTask() {

      // infinite loop - if code returns for a second iteration of this loop, 
      // the whole initialization procedure occurs again.

      while (true) {

        // create listener (accept connections from any address)
        TcpListener phiLinkListener = new TcpListener(IPAddress.Any, PHI_LINK_PORT);

        // change status to initializing
        setLinkStatus(PHI_LINK_STATUS.LINK_STARTED);

        // connect

        TcpClient client = null;

        try {
          // enable listener
          phiLinkListener.Start();

          // change status to waiting
          setLinkStatus(PHI_LINK_STATUS.LINK_CONNECTING);

          // wait for a phi to connect
          client = phiLinkListener.AcceptTcpClient();

        } catch (SocketException e) {
          // something went wrong
          setLinkStatus(PHI_LINK_STATUS.LINK_ERROR);
          Console.WriteLine("PhiLink.startListener: got exception when starting listener: SocketException: {0}", e);
          return;
        }

        // change status to connected
        setLinkStatus(PHI_LINK_STATUS.LINK_CONNECTED);

        // turn off "Nagle" algorithm so data is sent immediately
        Socket s = client.Client;
        s.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay, true);

        // Get a stream object for reading/writing
        NetworkStream stream = client.GetStream();

        // start comm loop  - only returns on error

        if (stream != null) {
          commLoop(stream);
          // commLoop returned - disconnected or error
          // TODO: just say disconnected for now and exit
          setLinkStatus(PhiLink.PHI_LINK_STATUS.LINK_DISCONNECTED);
        } else {
          // no stream? say error
          setLinkStatus(PhiLink.PHI_LINK_STATUS.LINK_ERROR);
          break;
        }
      }

      // only gets here if we can't get a strem for some weird reason
      return;
    }

    //
    // COMM LOOP
    //

    static void commLoop(NetworkStream phiStream) {

      // create PhiLink command and state packets
      PhiCmdPacket cmdPacket = new PhiCmdPacket();
      PhiStatePacket statePacket = new PhiStatePacket();

      // create startup tests controller and make active
      activeController = new StartupController();

      // initialize controller
      activeController.init(statePacket, cmdPacket);

      // init statistics 
      double tickPeriod = 1.0 / Stopwatch.Frequency;
      Stopwatch stopWatch = Stopwatch.StartNew();

      // loop forever
      
      long loopStartTicks = stopWatch.ElapsedTicks;
      
      while (true) {

        // get output of controller
        activeController.outputData(cmdPacket);

        // send controller outputs as command packet to PHI

        try {
          phiStream.Write(cmdPacket.packetData, 0, cmdPacket.Length);
        } catch (Exception e) {
          // error - disconnected?
          setLinkStatus(PHI_LINK_STATUS.LINK_ERROR);
          Console.WriteLine("phiStream.Write: got exception : Exception: {0}", e);
          break;
        }

        // read PHI's state

        int nRead = 0;
        while (nRead < statePacket.Length) {
          nRead += phiStream.Read(statePacket.packetData, nRead, statePacket.Length - nRead);
        }

        // load controller with PHI's state
        activeController.loadData(statePacket);

        // step controller
        activeController.step();

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
      return;
    }

    //
    // Misc
    //

    public static void setLinkStatus(PHI_LINK_STATUS status) {
      linkStatus = status;
      // notify change for UI
      PhiGlobals.mainWindow.notifyPhiLinkChanged();
    }

    public static double getAvgIdle() {
      lock (dataLock) {
        return accum_sleepTime / DESIRED_SECS_PER_LOOP;
      }
    }

    public static double getAvgFrameRate() {
      lock (dataLock) {
        return 1.0 / accum_secsPerLoop;
      }
    }

    public static double getLoopCount() {
      lock (dataLock) {
        return loopCount;
      }
    }
    public static UInt32 getLastPacketID() {
      lock (dataLock) {
        return lastPacketID;
      }
    }

    public static double getSleepError() {
      lock (dataLock) {
        return sleepError;
      }
    }

    public static IPhiController getActiveController() {
      return activeController;
    }

  }  // class
}  // namespac