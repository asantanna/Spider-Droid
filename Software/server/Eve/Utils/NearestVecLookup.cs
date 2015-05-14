using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Eve.Utils {

  class NearestVecLookup {

    class NvlListElem {
      float[] addr;
      object val;
      NvlListElem(float[] addr, object val) {
        this.addr = addr;
        this.val = val;
      }
    }

    List<NvlListElem> list;
    int lenAddrVec;

    NearestVecLookup(int vecLen) {
      list = new List<NvlListElem>();
      this.lenAddrVec = vecLen;
    }

    void append(float[] addr, object val) { }
  }
}
