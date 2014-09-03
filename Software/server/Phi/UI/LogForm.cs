using Phi.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Phi.UI {
  public partial class LogForm : Form {

    // log tree view root name (key)
    public const string LOG_NODENAME_ROOT = "logNode_Root";

    // log tree view root node
    public TreeNode logTreeRootNode;

    public LogForm() {

      // init components
      InitializeComponent();

      //
      // TreeView
      //

      // create root tree node
      logTreeRootNode = new TreeNode(LOG_NODENAME_ROOT);
      logTreeRootNode.Text = "Logs";
      logTreeRootNode.Tag = null;

      // add to view
      LogTree.Nodes.Add(logTreeRootNode);

      //
      // CHART
      //

      // turn off grids
      LogChart.ChartAreas[0].AxisX.MajorGrid.Enabled = false;
      LogChart.ChartAreas[0].AxisY.MajorGrid.Enabled = false;
      LogChart.ChartAreas[0].AxisY2.MajorGrid.Enabled = false;
    }

    public void ExpandAll() {
      LogTree.ExpandAll();
    }

    private void OnFormClosing(object sender, FormClosingEventArgs e) {
      // do not allow closing of form - hide instead
      e.Cancel = true;
      Hide();
    }

    private void LogChart_Click(object sender, EventArgs e) {
    }

    private void LogTree_AfterSelect(object sender, TreeViewEventArgs e) {
      object log = e.Node.Tag;
      if (e.Node.Tag != null) {
        (log as PhiLogBase).populateChart(LogChart);
      }
    }

    private void LogTree_BeforeSelect(object sender, TreeViewCancelEventArgs e) {
      TreeNode selNode = LogTree.SelectedNode;
      if (selNode != null) {
        object log = selNode.Tag;
        if (log != null) {
          // clear previous log being displayed
          (log as PhiLogBase).clearChart(LogChart);
        }
      }
    }

    private void recursiveDisposeLogs(TreeNode currNode) {
      // make a copy for iteration in case things get added/removed by children
      int numChildren = currNode.Nodes.Count;
      TreeNode[] nodesCopy = new TreeNode[numChildren];
      currNode.Nodes.CopyTo(nodesCopy,0);
      // iterate copy
      foreach (TreeNode node in nodesCopy) {
        // recursively dispose all child logs
        recursiveDisposeLogs(node);
      }
      // free self
      if (currNode.Tag != null) {
        // dispose log (also removes from view)
        (currNode.Tag as PhiLogBase).disposeLog();
      }
    }

    private void ClearLog_Click(object sender, EventArgs e) {
      // select tree root
      LogTree.SelectedNode  = logTreeRootNode;
      // recursively dispose all logs starting at tree root
      recursiveDisposeLogs(logTreeRootNode);
    }

    private void panel1_Paint(object sender, PaintEventArgs e) {
    }

    //
    // INVOKE-TYPE METHODS FOR NON-UI THREADS
    //

    private delegate int delegate_nodeAdd(TreeNode nodeToAdd);

    public static void invokeAddNode(TreeNode logTreeNode, TreeNode parentNode) {
      TreeView control = PhiGlobals.logForm.LogTree;
      control.Invoke(new delegate_nodeAdd(parentNode.Nodes.Add), logTreeNode);
    }

    private delegate void delegate_remove();

    public static void invokeRemove(TreeNode logTreeNode) {
      TreeView control = PhiGlobals.logForm.LogTree;
      control.Invoke(new delegate_remove(logTreeNode.Remove));
    }

  }
}
