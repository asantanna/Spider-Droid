using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Phi {

  public class PhiAction {

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

    // delegate for RunBlockAction
    public delegate void PhiActionCodeBlock();
    
    //
    // STATICS
    //

    // all actions are stored in a global dictionary with an id as the key.
    static private int nextId = ROOT_ACTION_ID;
    static private Dictionary<int, PhiAction> dict = new Dictionary<int, PhiAction>();

    //
    // VARS
    //

    string name;
    protected ACTION_STATE actionState;
    protected int id;
    protected int parentId;
    protected PhiActionCodeBlock codeBlock;

    //
    // CODE
    //

    public PhiAction(string name = "") {
      this.name = name;
      actionState = ACTION_STATE.INIT;
      id = nextId++;
      parentId = NO_PARENT_ID;          // assigned during addChildAction
      codeBlock = null;                 // assigned by creator method
      dict.Add(id, this);
    }

    ~PhiAction() {
      // DEBUG
      // Console.WriteLine("Action {0} collected", id);
    }

    public int getId() {
      return id;
    }

    public static void resetActionFramework() {
      // only used when aborting everything    
      nextId = ROOT_ACTION_ID;
      dict.Clear();
    }

    public ACTION_STATE getState() {
      return actionState;
    }

    public void setParentId(int _parentId) {
      parentId = _parentId;
    }

    public void step() {
      // "step" the action - i.e. run its code block
      codeBlock();
    }

    public virtual void addChildAction(PhiAction child) {
      throw new NotSupportedException("Cannot add children to simple actions - only to groups and sequences");
    }

    public void removeFromParent() {
      // find our parent
      PhiAction parent;
      if (dict.TryGetValue(parentId, out parent) == false) {
        throw new NotImplementedException("cannot find parent action! This is a bug");
      }
      // make sure it is a group
      if (parent.GetType() != typeof(PhiAction_Group)) {
        throw new NotImplementedException("parent is not a group! This is a bug");
      }
      // ask parent to remove us from its list
      (parent as PhiAction_Group).removeChildAction(this);
      // note: no need to remove from dictionary - destructor does this
      // so all simple actions also get removed
    }
    
    public void reset() {
      // reset action
      actionState = ACTION_STATE.INIT;
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

    public virtual void discard() {
      parentId = NO_PARENT_ID;
      dict.Remove(id);
    }

  } // class PhiAction

  //
  // PHI ACTIONS
  //

  // GROUP ACTION CLASS

  public class PhiAction_Group : PhiAction {

    public const int REPEAT_FOREVER = -1;
    int repeatCount;
    protected int execCount;

    protected List<PhiAction> children = new List<PhiAction>();

    public PhiAction_Group(PhiAction[] actions, int repeatCount = 1) : base() {
      this.repeatCount = repeatCount;
      codeBlock = groupCodeBlock;
      // add children to list
      foreach (PhiAction action in actions) {
        addChildAction(action);
      }
    }

    void groupCodeBlock() {

      if (actionState == ACTION_STATE.INIT) {
        execCount = 0;
        actionState = ACTION_STATE.RUNNING;
      }

      if (actionState == ACTION_STATE.RUNNING) {

        // make a copy for iteration in case things get added/removed by children
        List<PhiAction> childrenCopy = children.ToList();

        // step all children
        // Note: they are stepped even if DONE - this is simpler and possibly faster than testing
        foreach (PhiAction child in childrenCopy) {
          child.step();
        }

        // now iterate again through real list to see if there's anybody left that's not DONE

        foreach (PhiAction child in children) {
          if (child.getState() != ACTION_STATE.DONE) {
            // found a child that's not DONE - means group isn't DONE
            return;
          }
        }

        // if we get here, all children are DONE so group is also
        notifyDone();
      }
    }

    // notifyDone(): called when all children of a group are done
    //
    // Note: when repeating, all children are reset when ALL of them are done.
    // For sequences, this is simple.  For groups, this imples an added subtlety:
    // even though group actions run concurrently, the repeat only occurs when all
    // of them are done.  So they "synchronize their restarts".

    protected void notifyDone() {
      execCount ++;
      if ((repeatCount == REPEAT_FOREVER) || (execCount < repeatCount)) {
        // group will repeat - reset all actions in our tree to INIT state so they run again
        resetAllChildren();
      } else {
        // group will not repeat on its own - mark it DONE
        actionState = ACTION_STATE.DONE;

        if (parentId == ROOT_ACTION_ID) {
          // our parent is the root so we are NOT part of a bigger hierarchy
          // that might get restarted due to an enclosing repeating group.
          // Go ahead and remove us completely.
          removeFromParent();
        }
      }
    }

    override public void addChildAction(PhiAction child) {
      child.setParentId(id);
      children.Add(child);
    }

    public void removeChildAction(PhiAction child) {
      children.Remove(child);
      child.discard();
    }
    
    public override void discard() {
      base.discard();
      // make list copy for iteration
      List<PhiAction> childrenCopy = children.ToList();
      foreach (PhiAction child in childrenCopy) {
        removeChildAction(child);
      }
    }

    public void resetAllChildren() {
      foreach (PhiAction child in children) {
        child.reset();
      }
    }

    protected virtual void dumpGroup(string indentString, string header) {
      dumpCommon(indentString, header);
      Console.WriteLine(", children: {0}", children.Count);
      indentString += "| ";
      foreach (PhiAction child in children) {
        child.dump(indentString);
      }
    }

    public override void dump(string indentString = "") {
      dumpGroup(indentString, "Group");
    }

  } // PhiAction_Group class

  // SEQUENCE ACTION

  public class PhiAction_Sequence : PhiAction_Group {

    int currChildIdx;

    public PhiAction_Sequence(PhiAction[] actions) : base(actions) {
      codeBlock = sequenceCodeBlock;
      // children already added by group ctor
    }

    void sequenceCodeBlock() {

      if (actionState == ACTION_STATE.INIT) {
        // SEQUENCE not started
        execCount = 0;
        currChildIdx = 0;
        actionState = ACTION_STATE.RUNNING;
      }

      if (actionState == ACTION_STATE.RUNNING) {
        // SEQUENCE running
        if (children.Count() > currChildIdx) {
          // child avail - step current child
          PhiAction child = children.ElementAt(currChildIdx);
          child.step();
          // check if child finished
          if (child.getState() == PhiAction.ACTION_STATE.DONE) {
            // child finished - advance to next child
            currChildIdx++;
          }
        } else {
          // no more children - say SEQUENCE is done
          notifyDone();
        }
      }
    }

    public override void dump(string indentString = "") {
      dumpGroup(indentString, "Sequence");
    }

  } // PhiAction_Sequence class

  // RUNBLOCK ACTION

  public class PhiAction_RunBlock : PhiAction {

    PhiActionCodeBlock userCodeBlock;

    public PhiAction_RunBlock(PhiActionCodeBlock _userCodeBlock, string name = "") : base(name) {

      userCodeBlock = _userCodeBlock;

      codeBlock = delegate() {
        if (actionState == ACTION_STATE.INIT) {
          // run block
          actionState = ACTION_STATE.RUNNING;
          // call user code
          userCodeBlock();
          // action done
          actionState = ACTION_STATE.DONE;
        }
      };
    }

    public override void dump(string indentString = "") {
      dumpCommon(indentString, "RunBlock");
      Console.WriteLine(", codeBlock = {0}", userCodeBlock.Method.ToString());
    }

  } // PhiAction_RunBlock

  // WAIT ACTION

  public class PhiAction_Wait : PhiAction {
    UInt64 wakeup_uSecs;

    public PhiAction_Wait(UInt64 mSecs) {

      codeBlock = delegate() {

        switch (actionState) {
          case ACTION_STATE.INIT: {
              // save wake time
              wakeup_uSecs = PhiGlobals.model.getPhiTime() + (mSecs * 1000);
              actionState = ACTION_STATE.RUNNING;
            }
            break;

          case ACTION_STATE.RUNNING: {
            if (wakeup_uSecs >= PhiGlobals.model.getPhiTime()) {
                // time to make up
                actionState = ACTION_STATE.DONE;
              }
            }
            break;
        }
      };
    }

    public override void dump(string indentString = "") {
      dumpCommon(indentString, "Wait");
      Console.WriteLine();
    }

  } // PhiAction_Wait class

} // namespace
