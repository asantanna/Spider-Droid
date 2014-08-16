using Phi.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi.Utils {

  // This class implements a list that can be set to empty while still retaining the allocated elements.
  // The normal List<T> class has a Clear method but it frees all elements.  This class, instead, retains
  // them for reuse.  The main purpose of this class is to allow efficient reuse of the list without
  // wasteful reallocation of elements.

  public class ListWithReusableElements<T> : IEnumerable<T> where T : new() {

    // internal list used to store items
    // Note: we do NOT subclass because in C# apparently you can't override
    // interfaces so can't change the IEnumerable<T> in List<T> to make things
    // work right.
    List<T> baseList;

    // the highwater index is the index of last item provided to a caller
    private int highWaterIdx = -1;

    public int Capacity { get { return baseList.Capacity; } }

    //
    // CODE
    //

    public ListWithReusableElements() {
      baseList = new List<T>();
    }

    public ListWithReusableElements(int capacity) {
      baseList = new List<T>(capacity);
      // fill it
      for (int i = 0 ; i < capacity ; i++) {
        getNewItem();
      }
      // now clear it so it looks empty
      Clear();
    }

    public int Count { get { return highWaterIdx + 1; } }

    public void Clear() {
      // instead of clearing, just reset the high water mark
      highWaterIdx = -1;
    }

    public T getNewItem() {

      T newItem;

      // bump high water index to the index we will return
      highWaterIdx++;

      if (highWaterIdx < baseList.Count) {
        // prealloc not full yet - reuse existing item
        newItem = baseList[highWaterIdx];
      } else {
        // prealloc is full - alloc new item and add to end
        newItem = new T();
        baseList.Add(newItem);
      }

      // return item
      return newItem;
    }

    public override string ToString() {
      int i = 1;
      string s = "[";
      if (Count > 0) {
        foreach (T val in (this as ListWithReusableElements<T>)) {
          s += string.Format("{0}{1}", val.ToString(), (i++ < Count) ? " " : "");
        }
      }
      return s + "]";
    }

    public List<T> debugGetBaseList() {
      return baseList;
    }

    //
    // IEnumerable<T>
    //

    IEnumerator<T> IEnumerable<T>.GetEnumerator() {
      return new ListWithReusableElementsEnumerator<T>(this);
    }

    System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() {
      // call specific one
      return (this as IEnumerable<T>).GetEnumerator();
    }

    //
    // IEnumerator<T>
    //

    private class ListWithReusableElementsEnumerator<T2> : IEnumerator<T2> where T2 : new() {

      ListWithReusableElements<T2> enumList;
      int enumIdx;

      public ListWithReusableElementsEnumerator(ListWithReusableElements<T2> list) {
        enumList = list;
        reset();
      }

      void reset() {
        enumIdx = -1;
      }

      T2 IEnumerator<T2>.Current {
        get { return enumList.baseList[enumIdx]; }
      }

      void IDisposable.Dispose() {
        enumList = null;
      }

      object System.Collections.IEnumerator.Current {
        get { return (this as IEnumerator<T2>).Current; }
      }

      bool System.Collections.IEnumerator.MoveNext() {
        if (enumIdx < 0) {
          // first call to MoveNext - point to first available item
          enumIdx = 0;
          return (enumList.Count != 0);
        }

        // not first call - advance
        enumIdx++;

        return (enumIdx < enumList.Count);
      }

      void System.Collections.IEnumerator.Reset() {
        reset();
      }

    } // class ListWithReusableElementsEnumerator

  } // class ListWithReusableElements

} // namespace Phi.Utils
  
//
// TEST ROUTINE
//

namespace Phi {

  static partial class PhiGlobals {

    private class LWRE_TestBuff {
      public int val;
      public override string ToString() {
        return val.ToString();
      }
    }

    static public void runClassTest_ListWithReusableElements() {

      Console.WriteLine("*** Testing ListWithReusableElements<T>");

      ListWithReusableElements<LWRE_TestBuff> list = new ListWithReusableElements<LWRE_TestBuff>(4);

      Console.WriteLine("Prealloc'd 4 items.  Expect Capacity=4, Count=0, actually got Capacity={0}, Count={1}", list.Capacity, list.Count);

      LWRE_TestBuff[] newItem = new LWRE_TestBuff[4];

      newItem[0] = list.getNewItem(); newItem[0].val = 1;
      newItem[1] = list.getNewItem(); newItem[1].val = 2;
      newItem[2] = list.getNewItem(); newItem[2].val = 3;
      newItem[3] = list.getNewItem(); newItem[3].val = 4;

      Console.WriteLine("Added 4 items.  Expect Capacity=4, Count=4, actually got Capacity={0}, Count={1}", list.Capacity, list.Count);
      Console.WriteLine("Should say: [1 2 3 4], actually says: {0}", list.ToString());

      list.Clear();

      Console.WriteLine("Cleared list.  Expect base.Count=4, Count=0, actually got base.Count={0}, Count={1}", list.debugGetBaseList().Count, list.Count);

      Console.WriteLine("Reallocating and checking we reuse properly:");

      for (int i = 0 ; i < 4 ; i++) {
        LWRE_TestBuff checkItem = list.getNewItem();
        checkItem.val = i + 5;
        if (checkItem == newItem[i]) {
          Console.WriteLine("Item {0} reused OK", i);
        } else {
          Console.WriteLine("Item {0} reused FAILED!!!", i);
        }
      }

      Console.WriteLine("Added 4 (reused) items.  Expect Capacity=4, Count=4, actually got Capacity={0}, Count={1}", list.Capacity, list.Count);
      Console.WriteLine("Expect base.Count=4, Count=4, actually got base.Count={0}, Count={1}", list.debugGetBaseList().Count, list.Count);
      Console.WriteLine("Should say: [5 6 7 8], actually says: {0}", list.ToString());

      LWRE_TestBuff moreItem = list.getNewItem(); moreItem.val = 9;
      moreItem = list.getNewItem(); moreItem.val = 10;

      Console.WriteLine("Added 2 more items.  Expect Capacity >= 6, Count=6, actually got Capacity={0}, Count={1}", list.Capacity, list.Count);
      Console.WriteLine("Expect base.Count=6, Count=6, actually got base.Count={0}, Count={1}", list.debugGetBaseList().Count, list.Count);
      Console.WriteLine("Should say: [5 6 7 8 9 10], actually says: {0}", list.ToString());

      list.Clear();
      moreItem = list.getNewItem(); moreItem.val = 11;

      Console.WriteLine("Cleared list again and added 1 item.  Expect base.Count=6, Count=1, actually got base.Count={0}, Count={1}",
        list.debugGetBaseList().Count, list.Count);
      Console.WriteLine("Should say: [11], actually says: {0}", list.ToString());
    }

  } // class PhiGlobals

} // namespace Phi.Utils
