#ifndef __MATH_H__
#define __MATH_H__

#include <math.h>

#include "types.h"

/*------------ Vector Functions ------------*/

float l2_sq(point_t a, point_t b);


/*------------ Angle Functions -------------*/

float angled(uint32_t ab, uint32_t ac, uint32_t bc);

float norm_angle(float theta);
float neg_norm_angle(float theta);
float center_angle(float theta);
float angle_diff(float th1, float th2);

#endif
