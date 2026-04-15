#include "mixer.h"

/*
 Motor layout (X configuration):

       Front
         ^
         |
   M4         M2
      \     /
       \   /
        \ /
        / \
       /   \
      /     \
   M3         M1
*/

void mixer_x_configuration(float t, float r, float p, float y, float *m) 
//mixing here might be wrong as well - switched the signs in front of p on all
{
    m[0] = t + r - p - y;  // M1 (Back Right, CCW)
    m[1] = t + r + p + y;  // M2 (Front Right, CW)
    m[2] = t - r - p + y;  // M3 (Back Left, CW)
    m[3] = t - r + p - y;  // M4 (Front Left, CCW)

    //cap vals
    for (int i = 0; i < 4; i++) {
        if (m[i] < 0) m[i] = 0;
        if (m[i] > 1) m[i] = 1;
    }
}
