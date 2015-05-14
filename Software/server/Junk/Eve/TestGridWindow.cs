using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Eve {
  public partial class TestGridWindow : Form {

    //
    // STATICS
    //

    static Color bkgndColor = Color.LightBlue;
    static Pen gridPen = Pens.LightYellow;
    static Pen rectPen = Pens.LightSkyBlue;
    static Pen triPen  = Pens.LightYellow;

    //
    // VARS
    //

    const string CRLF = "\r\n";

    int gridDim;
    EveGridSpace gridSpace;
    Point currMousePos;

    //
    // CODE
    //

    public TestGridWindow() {

      // init UI
      InitializeComponent();

      // init
      gridDim = 500;
      gridSpace = new EveGridSpace(5);
      currMousePos = new Point();
      
      // update controls
      tbGridLevels.Text = gridDim.ToString();
    }

    private void GridWindow_MouseDown(object sender, MouseEventArgs e) {
    }

    private void GridWindow_MouseMove(object sender, MouseEventArgs e) {
      currMousePos = e.Location;
      GridWindow.Invalidate();
    }

    private void GridWindow_MouseUp(object sender, MouseEventArgs e) {
    }

    private void GridWindow_Paint(object sender, PaintEventArgs e) {

      tbStatus.Text = 
        " Client: " + currMousePos.X + ", " + currMousePos.Y + CRLF +
        " ";

      if (cbShowGrid.Checked == true) {
        gridSpace.drawGrid(e.Graphics, new Point(0, 0));
      }

      if (cbShowPixels.Checked == true) {
        // gridSpace.clientToRect(currMousePos, ref rectPos);
        // gridSpace.drawPixelsAt(e.Graphics, rectPos);
      }

      if (cbShowFields.Checked == true) {
      }
    }

  }
}
