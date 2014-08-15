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
    }

    public static TreeView getLogTreeView() {
      return PhiGlobals.logForm.LogTree;
    }

    /* DELETE ME SOON
     * 
    public static void addNodeToNamedParent(TreeNode node, string parentName) {

      // find parent node

      TreeView treeView = getLogTreeView();
      TreeNode[] matchNodes = treeView.Nodes.Find(parentName, true);

      if (matchNodes.Length == 0) {
        Console.WriteLine("addNodeToNamedParent: node \"" + parentName + "\" not found.");

      } else if (matchNodes.Length > 1) {
        throw new InvalidOperationException("more than one node with same name found in log treeview");

      } else {

        // go ahead and add node
        TreeNode parentNode = matchNodes[0];
        parentNode.Nodes.Add(node);
      }
    } */

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

  }
}
