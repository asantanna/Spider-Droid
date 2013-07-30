using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Phi_Core {

  static class PhiLink {

    //
    // CONST
    //

    const string LOCAL_IP = "127.0.0.1";
    const Int32 PHI_LINK_PORT = 1122;

    //
    //  VARS
    //

    internal enum PhiLinkState {
      OFF,
      INITIALIZING,
      WAITING,
      CONNECTED,
      ERROR,
      CANCELLED
    }

    static PhiLinkState linkState = PhiLinkState.OFF;
    static Task commLoopTask = null;
    static UInt32 loopCount = 0;

    private static double msPerLoop = 0;
    private static object statLock = new object();

    //
    // CODE
    //

    async static internal Task<NetworkStream> startListenerAsync() {

      // start listener (ASYNC)

      IPAddress localAddr = IPAddress.Parse(LOCAL_IP);
      TcpListener phiLinkListener = new TcpListener(localAddr, PHI_LINK_PORT);
      Task<TcpClient> clientTask = null;

      // change status to initializing
      setLinkState(PhiLinkState.INITIALIZING);

      try {
        // enable listener
        phiLinkListener.Start();

        // change status to waiting
        setLinkState(PhiLinkState.WAITING);

        // wait async for a phi to connect
        clientTask = phiLinkListener.AcceptTcpClientAsync();

      } catch (SocketException e) {
        // something went wrong
        setLinkState(PhiLinkState.ERROR);
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
      setLinkState(PhiLinkState.CONNECTED);

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
      long lastTicks = stopWatch.ElapsedTicks;

      // loop forever
      while (true) {

        // grab Eve outputs
        // eve.writeOutputs(cmdPacket.packetData);

        // start task to send eve outputs as command packet to PHI
        Task writeTask = Task.Run(() => phiStream.Write(cmdPacket.packetData, 0, cmdPacket.Length));

        // start task to read PHI's status
        Task readTask = Task.Run(() => phiStream.Read(statePacket.packetData, 0, statePacket.Length));

        // HACK: for now just wait for read to complete right away
        // This is like doing a blocking read above.
        // In actuality, it would be better if Eve ran completely out of step with comms.  Both
        // sides just continuous spray packets at each other.  Whenever something is received
        // the receiver is affected by it.

        readTask.Wait();

        // write sensor states into Eve
        // eve.writeInputs(statePacket);

        // step Eve
        // eve.Step();

        // HACK - for now just wait for write to complete before we send the next packet
        writeTask.Wait();

        // count loop
        loopCount++;

        // compute statistics

        long currTicks = stopWatch.ElapsedTicks;

        lock (statLock) {
          // protect access
          // Note: not very important now but will be more important when
          // we keep arrays, lists, etc
          msPerLoop = (currTicks - lastTicks) * tickPeriod;
        }

        lastTicks = currTicks;
      }
      
      // if we get here, loop was cancelled
      // setLinkState(PhiLinkState.CANCELLED);
    }

    //
    // Misc
    //

    internal static void setLinkState(PhiLinkState state) {
      linkState = state;
      PhiGlobals.mainWindow.updateLinkStatus(state);
    }

    internal static double getMsPerLoop() {
      lock (statLock) {
        return msPerLoop;
      }
    }

  }  // class
}  // namespace

