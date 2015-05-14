using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

//
// INCOMPLETE - ABANDONED - did some experimentation on MuPAD and didn't find this
// all that useful.  So setting aside.
//

//
// EVEGRIDSPACE - 2D space with triangular tiling. It also supports fields similar to grid cells.
// ============
//
// The space is quantized into equilateral triangles.  Since we are usually dealing
// with rectangular spaces and have to convert to triangular and back, it is useful
// to pick a size for the triangles that helps us deal with the effects of the conversion
// in terms of uncertainty and also with consequences of the rectangular space possibly
// being quantized.
//
// UNCERTAINTY
// ===========
//
// In a rect space, a quantized coordinate represents a unit square centered on the coordinate.
// We can think of this quantization in terms of the largest distance that we can move
// from the center that is guaranteed not to change the quantized coordinate.  In the case of
// a unit square, this is 1/2.  This is along the axes - a movement along a diagonal needs to go
// further, but we are only considering the maximum "guaranteed safe" displacement because
// we use it as an intuitive measure of quantization.  I.e. quantization means we guarantee
// that each individual element of the coordinates is within 1/2 of its true value.
//
// We can do something similar with hexagons.  If we are at the center of the hex, then there 
// is a minimum distance to an edge.  A displacement less than this is guaranteed to be in the
// hex.  In terms of coordinates, there are 3 "natural" directions to consider for axes (we only
// need 2 of the 3 for a 2D space) - the origin is at the center and each of the three axes crossing
// the edges at right angles.  In this setup, if we make the center-to-edge distance 1/2, then we
// can similarly guarantee that each individual coordinate is also less than 1/2.  This is true
// because if we take a movement greater than 1/2 in one axis, we leave the hexagon and the combined
// movement on the other two axes is always perpendicular to the first axis and thus can never 
// "re=enter" the hexagon.
//
// So, our strategy to deal with uncertainty is to pick a hexagon size of center to edge distance
// equal to 1/2.  This, in turn, implies that the constituent equilateral triangles must have
// *height* equal to 1/2. By algebra, we find that the *side* of a triangle of height 1/2 is 1/sqrt(3).
//
// We encode the position of a triangle using a fractal-like process in different scales. Consider that
// a triangle's position can be recursively specified:
//
//    1) Specify the max size of the rectangular space
//    2) Start with an equilateral triangle that encloses that space.  Call this the
//       level "-1" triangle (or root triangle) that will start the process.  It is a
//       "right-side up" triangle.
//    3) Subdivide the triangle into 4 equilateral sub-triangles and record which one
//       contains the point.
//    4) If we have not reached our desired precision, goto step 3 using this sub-triangle.
//    5) The output is a vector of base 4 elements with each element identifying the
//       sub-triangle that was chosen. The length of the vector is the number of iterations
//       (levels) of this process.
//
// To go from this vector to a hexagonal field, this recursive vector is only the first
// logical step.  The next step is to note that each triangle is the intersection of three
// hexagons so we need three hexagons to specify it.
//
// Homing in on the correct triangle of a 4 triangle set requires us to use a bit to slice
// the large triangle along one of the inner edges and specify which side of the edge the
// triangle is in.  We can imagine that the red hexagon specifies one side of this "red" slice
// and the not-red region specifies the other side.  If we do this for the other two interior
// edges using blue and green hexagons we can specify all for triangles with 3 bits - one
// for each color. Note that this is redundant because there are only 4 triangles so we only
// should need 2 bits.
// 
// The reason we need 3 bits in our encoding is that we not all combinations are
// valid.  To see this, imagine that the red, green and blue hexagons are the the large
// triangle's vertices.  Further let us say that if we are in the red hex, we will say "R"
// and if we are not in the red hex, then we will say "r". It is clear that if we are in the
// red region, you cannot be in the green or blue regions because they don't intersect. 
// In fact, the only possible "codes" are Rgb, rGb, rgB and rgb.
//
// Position is encoded for a point (x,y) by mapping it onto a binary vector, one element
// per field.  For each element, if the point (x,y) is within one of its hexagonal regions,
// the value of the element is "1", otherwise it is 0.  If all of these fields were AND'ed
// at every scale, only one small triangle would remain non-zero - thus identifying the
// region.  The length of this binary vector is 3 bits/level * number of levels.
//
// Characteristics of the representation
// =====================================
//
// Position is a GRAY code.  Since, at each scale, the large triangle is sub-divided 
// by slices of a field, an edge crossing to a neighboring triangle always changes only
// one field if we are crossing to another internal triangle. If we are crossing to an
// external triangle, then no fields change at the current scale but a single field
// changes at the next scale.  Thus we always have a single bit changing only - a gray code.
//
// 2) A type of distance can be computed by XOR'ing two vectors.  This identifies the changed
// fields and can be associated with speed and/or velocity. To see that this is true,
// imagine movement in a straight line along one of the 3 directions that cross hexagons
// perpendicular to the edges.  Only one of the fields changes, the other two remain
// constant.  Additionally, using two axes, we can compute a Manhattan distance by adding
// the weighted bit changes on each axis.  Consider that at a certain scale, no bit change
// means you have not left the hexagon of that scale, a bit change means that you have moved
// one hexagon over.  The next level is the same but with hexagons that are double in size.
// Thus, the xor'd bits of fields of the same type at different scales generate the distance
// along that axis in binary.  Adding them together generates the Manhattan distance.
//
//  See code for more details.
// 

namespace Eve {

  class EveGridSpace {

    //
    // CONST
    //

    //
    // STATICS
    //

    static double DEG_60 = Math.PI / 3;
    static double SIN_60 = Math.Sin(DEG_60);
    static double COS_60 = Math.Cos(DEG_60);
    static double TAN_60 = SIN_60 / COS_60;

    static double SQRT_2   = Math.Sqrt(2.0);
    static double SQRT_3   = Math.Sqrt(3.0);
    static double SQRT_2_THIRDS = SQRT_2 / SQRT_3;
    static double SQRT_2_DIV_2 = SQRT_2 / 2;

    //
    // VARS
    //

    int rectSpace_side;

    double triRoot_side;
    SizeF rectSpace_offset;
    double triPix_side;
    SizeF triPix_size;

    //
    // CODE
    //

    public EveGridSpace(int rectSpaceSize) {

      // the size of the square space is the length of the side of the square that contains it
      this.rectSpace_side = rectSpaceSize;

      // tri pix side 
      // Note: the height of the smallest triangle must be 1/2 so the side must
      // be 1/sqrt(3) (see above).

      triPix_side = 1 / Math.Sqrt(3.0);


//      triPix_size = new SizeF((float) triPix_side, (float) (triPix_side * SIN_60));



      // compute number of levels of fractal recursion needed (see class comments).
      // Note: We would like the final step to use a rectangle that fits the entire
      // rect space, so we must compute the side of that (root) triangle.  Later,
      // when we compute fields, we must make sure this triangle aligns properly
      // with the space, so we need to also compute the root triangle's origin
      // in triangular space.

      // side of triangle that fits a 1.0 square
      double fitPix_side = ((2.0/ 3.0) * SQRT_3) + 1;

      // side that fits whole space
      double fitSpace_side = fitPix_side * rectSpace_side;

      // compute number of levels required - each level is twice the
      // size of the previous one
      double numLevelsF = Math.Log(fitSpace_side / triPix_side, 2);

      // round up
      int numLevels = (int) Math.Ceiling(numLevelsF);

      // compute actual size of root triangle
      triRoot_side = triPix_side * Math.Pow(2, numLevels);

      // compute offset for rectangle space so it lines up with the root triangle
      // Note: if we didn't have to increase the size of the triangle by powers
      // of 2, the offset would be exactly rectSpace in the Y direction. Instead,
      // we put it at the same place a bigger rect space would have fit.  This
      // guarantees it will be inside the triangle.

      double off = triRoot_side / (1 + (2 * TAN_60));
      rectSpace_offset = new SizeF(0, (float) off);
    }

    //
    // CONVERSION
    //

    // find triangle that contains point
    // HACK - level not used yet

    Point rectToTri(PointF rect, int level) {

      // convert to parallelogram coords
      rect += rectSpace_offset;
      double Px = SQRT_2 * rect.X - SQRT_2_THIRDS * rect.Y;
      double Py = 2 * SQRT_2_THIRDS * rect.Y;

      // separate frac part
      int QPx = (int) Math.Floor(Px);
      int QPy = (int) Math.Floor(Py);
      double FPx = Px - QPx;
      double FPy = Py - QPy;

      // convert to tri
      Point tri = new Point(QPx, QPy);
      if (((QPx + QPy) % 2) == 1) {
        // odd - might be in a different triangle
        if (FPx + FPy >= SQRT_2_DIV_2) {
          // in next triangle
          tri.X++;
        }
      }

      // return triangle "name" (not exactly coords since jumps around)
      return tri;
    }

    /* void triToRect(Point tri, PointF rect) {
      // origin - memberwise copy so don't replace
      rect.X = triPix_org.X;
      rect.Y = triPix_org.Y;
      // calc draw pos
      rect.X += tri.X * triPix_step.Width;
      rect.Y += tri.Y * triPix_step.Height;
      if ((tri.X % 2) == 1) {
        // odd triangles are higher
        rect.Y += triPix_offsetOddY;
      }
    }

    void triToVerts(Point tri, PointF[] verts) {
      Debug.Assert(verts.Length == 3);
      // calc vert offsets from center
      verts[0].X = -triPix_offsetToCenter.Width;
      verts[0].Y = -triPix_offsetToCenter.Height;
      verts[1].X = verts[0].X + triPix_side;
      verts[1].Y = verts[0].Y;
      verts[2].X = verts[0].X + (triPix_side / 2);
      verts[2].Y = verts[0].Y + (triPix_size.Height);
      // odd ones are flipped
      if ((tri.X % 2) == 1) {
        // odd triangles are flipped in Y-dir
        verts[0].Y  *= -1;
        verts[1].Y  *= -1;
        verts[2].Y  *= -1;
      }
      // move to right place
      PointF center = new PointF();
      triToRect(tri, center);
      verts[0].X += center.X; verts[0].Y += center.Y;   // hmm - no easy way to add points?
      verts[1].X += center.X; verts[1].Y += center.Y;
      verts[2].X += center.X; verts[2].Y += center.Y;
    }

    void rectToVerts(PointF rect, PointF[] verts) {
      Debug.Assert(verts.Length == 4);
      // calc vert offsets from center
      float x = rect.X; float y = rect.Y;
      verts[0].X = x - 0.5f; verts[0].Y = y - 0.5f;
      verts[1].X = x + 0.5f; verts[1].Y = y - 0.5f;
      verts[2].X = x + 0.5f; verts[2].Y = y + 0.5f;
      verts[3].X = x - 0.5f; verts[3].Y = y + 0.5f;
    }

    void rectToClient(PointF rect, ref PointF client) {
      client.X = (rect.X * rectPix_scale) + rectPix_org.X;
      client.Y = (rect.Y * rectPix_scale) + rectPix_org.Y;
    }

    public void clientToRect(PointF client, ref PointF rect) {
      rect.X = (client.X - rectPix_org.X) / rectPix_scale;
      rect.Y = (client.Y - rectPix_org.Y) / rectPix_scale;
    } */

    //
    // DRAW
    //

    public void drawGrid(Graphics g, Point origin) {
    }

    void drawGridLine(Graphics g, Point origin, Size size, PointF p1, PointF p2) {
    }
    
    /*
    static PointF[] draw_rectVerts = new PointF[4];
    static PointF[] draw_triVerts = new PointF[3];
    static Point draw_triPos = new Point();

    public void drawPixelsAt(Graphics g, PointF rectPos) {
      // rect
      rectToVerts(rectPos, draw_rectVerts);
      drawRectPoly(g, rectPen, draw_rectVerts);
      // triangle
      rectToTri(rectPos, ref draw_triPos);
      triToVerts(draw_triPos, draw_triVerts);
      drawRectPoly(g, triPen, draw_triVerts);
    }

    static PointF[] drv_clientVerts = new PointF[4];

    void drawRectPoly(Graphics g, Pen pen, PointF[] verts) {
      int len = verts.Length;
      for (int i = 0 ; i < len ; i++) {
        // convert in place
        rectToClient(verts[i], ref drv_clientVerts[i]);
      }
      g.DrawPolygon(pen, drv_clientVerts);
    } */

  }
}
