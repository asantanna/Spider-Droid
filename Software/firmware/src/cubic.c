//
// Cubic Extrapolator
//

#define STANDALONE

#ifdef STANDALONE

#define UINT32          unsigned long
#define UINT64          unsigned long long

#include <math.h>

#else

#include "phi.h"

#endif

double cubic(
  double x1,
  double y1,
  double x2,
  double y2,
  double x3,
  double y3,
  double x4,
  double y4,
  double x5)  {

  //shift all x coordinates so x5 equals zero
  x1 -= x5;
  x2 -= x5;
  x3 -= x5;
  x4 -= x5;

  //compute squares and cubes of each coordinate
  double x1_2 = pow(x1, 2);
  double x1_3 = pow(x1, 3);
  double y1_2 = pow(y1, 2);
  double y1_3 = pow(y1, 3);
  double x2_2 = pow(x2, 2);
  double x2_3 = pow(x2, 3);
  double y2_2 = pow(y2, 2);
  double y2_3 = pow(y2, 3);
  double x3_2 = pow(x3, 2);
  double x3_3 = pow(x3, 3);
  double y3_2 = pow(y3, 2);
  double y3_3 = pow(y3, 3);
  double x4_2 = pow(x4, 2);
  double x4_3 = pow(x4, 3);
  double y4_2 = pow(y4, 2);
  double y4_3 = pow(y4, 3);

  //compute y5 and return it
  return (- y4*x1_3*x2_2*x3 + y3*x1_3*x2_2*x4 + y4*x1_3*x2*x3_2 - y3*x1_3*x2*x4_2 - y2*x1_3*x3_2*x4
        + y2*x1_3*x3*x4_2 + y4*x1_2*x2_3*x3 - y3*x1_2*x2_3*x4 - y4*x1_2*x2*x3_3 + y3*x1_2*x2*x4_3
        + y2*x1_2*x3_3*x4 - y2*x1_2*x3*x4_3 - y4*x1*x2_3*x3_2 + y3*x1*x2_3*x4_2 + y4*x1*x2_2*x3_3
        - y3*x1*x2_2*x4_3 - y2*x1*x3_3*x4_2 + y2*x1*x3_2*x4_3 + y1*x2_3*x3_2*x4 - y1*x2_3*x3*x4_2
        - y1*x2_2*x3_3*x4 + y1*x2_2*x3*x4_3 + y1*x2*x3_3*x4_2 - y1*x2*x3_2*x4_3)
       /
       (- x1_3*x2_2*x3 + x1_3*x2_2*x4 + x1_3*x2*x3_2 - x1_3*x2*x4_2 - x1_3*x3_2*x4 + x1_3*x3*x4_2
        + x1_2*x2_3*x3 - x1_2*x2_3*x4 - x1_2*x2*x3_3 + x1_2*x2*x4_3 + x1_2*x3_3*x4 - x1_2*x3*x4_3
        - x1*x2_3*x3_2 + x1*x2_3*x4_2 + x1*x2_2*x3_3 - x1*x2_2*x4_3 - x1*x3_3*x4_2 + x1*x3_2*x4_3
        + x2_3*x3_2*x4 - x2_3*x3*x4_2 - x2_2*x3_3*x4 + x2_2*x3*x4_3 + x2*x3_3*x4_2 - x2*x3_2*x4_3);
}