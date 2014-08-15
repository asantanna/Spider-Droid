using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  public class PhiActionBase {

    //
    // CONSTANTS
    //

    protected const int NO_PARENT_ID = 0;
    protected const int ROOT_ACTION_ID = 1;

    public enum ACTION_STATE {
      INIT,
      RUNNING,
      DONE,
    }

    [Flags]
    public enum ACTION_FLAGS {
      NONE          = 0,
      AUTO_REMOVE   = 0x0001,       // once action completes, it will remove itself automatically from its parent
    }

    // delegate for RunBlockAction
    public delegate void PhiActionCodeBlock();
    
    //
    // STATICS
    //

    // all actions are stored in a global dictionary with an id as the key.
    static private int nextId = ROOT_ACTION_ID;
    static private Dictionary<int, PhiActionBase> dict = new Dictionary<int, PhiActionBase>();

    //
    // VARS
    //

    string name;
    protected int id;
    protected int parentId;
    protected ACTION_STATE actionState;
    protected PhiActionCodeBlock codeBlock;
    ACTION_FLAGS flags;

    //
    // CODE
    //

    public PhiActionBase(string name = "") {
      this.name = name;
      actionState = ACTION_STATE.INIT;
      id = nextId++;
      parentId = NO_PARENT_ID;          // assigned during addChildAction
      codeBlock = null;                 // assigned by creator method
      flags = ACTION_FLAGS.NONE;
      dict.Add(id, this);
    }

    ~PhiActionBase() {
      // DEBUG
      // Console.WriteLine("Action {0} collected", id);
    }

    public int getId() {
      return id;
    }

    public ACTION_STATE getState() {
      return actionState;
    }

    public void setParentId(int _parentId) {
      parentId = _parentId;
    }

    public ACTION_FLAGS getFlags() {
      return flags;
    }

    public void clearFlagBits(ACTION_FLAGS bitsToClear) {
      flags &= ~bitsToClear;
    }

    public void setFlagBits(ACTION_FLAGS bitsToSet) {
      flags |= bitsToSet;
    }

    public void step() {
      // "step" the action - i.e. run its code block
      codeBlock();

      if ( (actionState == ACTION_STATE.DONE) &&
           (parentId != NO_PARENT_ID) &&
           ((getFlags() & ACTION_FLAGS.AUTO_REMOVE) != 0))
      {
        // action is done, has a parent and the auto remove flag is set
        removeFromParent();
      }
    }

    public virtual void addChild(PhiActionBase child) {
      throw new NotSupportedException("Cannot add children to simple actions - only to groups and sequences");
    }

    public void removeFromParent() {
      // find our parent
      PhiActionBase parent;
      if (dict.TryGetValue(parentId, out parent) == false) {
        throw new NotImplementedException("cannot find parent action! This is a bug");
      }
      // make sure it is a group
      if ((parent is PA_Group) == false) {
        throw new NotImplementedException("parent is not a group (or subclass of group)! This is a bug");
      }
      // ask parent to remove us from its list
      (parent as PA_Group).removeChild(this);
    }
    
    public virtual void reset() {
      // reset action
      actionState = ACTION_STATE.INIT;
    }

    public virtual void discard() {
      parentId = NO_PARENT_ID;
      dict.Remove(id);
    }

    public static void resetActionFramework() {
      // only used when aborting everything    
      nextId = ROOT_ACTION_ID;
      dict.Clear();
    }

    public static int getDictSize() {
      return dict.Count;
    }

    public void dumpCommon(string indentString, string header) {
      Console.Write(indentString);
      Console.Write("{0}({1})", header, name);
      Console.Write(": id={0}, parent={1}, state={2}", id, parentId, actionState.ToString());
    }

    public virtual void dump(string indentString = "") {
      dumpCommon("???", indentString);
      Console.WriteLine();
    }

  } // class PhiActionBase

} // namespace
