using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  public class CircularBuffer<T> : IEnumerable<T> where T : new() {

    T[] buffer; 
    int nextFree;

    public CircularBuffer(int length) {

      if (length <= 0) {
        throw new InvalidOperationException("CircularBuffer must have length >= 1");
      }

      nextFree = 0;
      buffer = new T[length];

      // circular buffers are always prefilled
      // Note: this mean that all types T must have a parameterless constructor
      for (int i = 0 ; i < length ; i++) {
        buffer[i] = new T();
      }
    }

    public void Add(T o) {
      buffer[nextFree] = o;
      nextFree = (nextFree + 1) % buffer.Length;
    }

    public int getStartIndex() {
      return nextFree;
    }

    public int Length {
      get { return buffer.Length; }
    }

    public override string ToString() {
      int i = 1;
      string s = "[";      
      if (Length > 0) {
        foreach (T val in this) {
          s += string.Format("{0}{1}", val.ToString(), (i++ < Length) ? " " : "");
        }
      }
      return s + "]";
    }

    //
    // IEnumerable<T>
    //

    IEnumerator<T> IEnumerable<T>.GetEnumerator() {
      return new CircBufferEnumerator<T>(this);
    }

    System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() {
      // call specific one
      return (this as IEnumerable<T>).GetEnumerator();
    }

    //
    // IEnumerator<T>
    //

    private class CircBufferEnumerator<T2> : IEnumerator<T2> where T2 : new() {

      CircularBuffer<T2> enumBuffer;
      int enumIdx;
      
      public CircBufferEnumerator(CircularBuffer<T2> buffer) {
        enumBuffer = buffer;
        reset();
      }

      void reset() {
        enumIdx = -1;
      }

      T2 IEnumerator<T2>.Current {
        get { return enumBuffer.buffer[enumIdx]; }
      }

      void IDisposable.Dispose() {
        enumBuffer = null;
      }

      object System.Collections.IEnumerator.Current {
        get { return (this as IEnumerator<T2>).Current; }
      }

      bool System.Collections.IEnumerator.MoveNext() {
        if (enumIdx < 0) {
          // first call to MoveNext - point to first item
          enumIdx = enumBuffer.getStartIndex();
          return (enumBuffer.Length != 0);
        }
        // not first call - advance
        enumIdx = (enumIdx + 1) % enumBuffer.Length;
        return (enumIdx != enumBuffer.getStartIndex());
      }

      void System.Collections.IEnumerator.Reset() {
        reset();
      }
    } // class CircBufferEnumerator
      
  } // CircularBuffer class

} // namespace Phi.Utils
  
//
// TEST ROUTINE
//

namespace Phi {

  static partial class PhiGlobals {

    public static void runClassTest_CircularBuffer() {

      Console.WriteLine("*** Testing CircularBuffer<T>");

      CircularBuffer<int> buff = new CircularBuffer<int>(4);

      buff.Add(1);
      buff.Add(2);
      buff.Add(3);
      buff.Add(4);

      Console.WriteLine("Should say: [1 2 3 4], actually says: {0}", buff.ToString());

      buff.Add(5);
      buff.Add(6);

      Console.WriteLine("Should say: [3 4 5 6], actually says: {0}", buff.ToString());
    }

  } // class PhiGlobals

} // namespace Phi.Utils
