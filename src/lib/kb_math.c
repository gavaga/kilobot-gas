#include "kb_math.h"

// acos
#include <math.h>

#include "constants.h"

/*! l2_sq
 *
 * L2 norm squared distance between a and b
 */
float
l2_sq(
    point_t a,
    point_t b)
{
    float diffx = b.x - a.x;
    float diffy = b.y - a.y;
    return diffx*diffx + diffy*diffy;
}


/*! angled
 *
 * Compute law of cosines angle BAC
 *
 * @param[in] ab    Distance A to B
 * @param[in] ac    Distance A to C
 * @param[in] bc    Distance B to C
 *
 * @return Angle BAC
 */
float
angled(
    uint32_t ab,
    uint32_t ac,
    uint32_t bc)
{
    float dab = (float) ab;
    float dac = (float) ac;
    float dbc = (float) bc;
    float gamma = (dab*dab + dac*dac - dbc*dbc)/(2.0*dab*dac);
    gamma = acos(gamma);
    return gamma;
}

/*! norm_angle
 *
 * return angle between 0 and 2pi, rather than -pi and pi
 */
float
norm_angle(float theta) {
    int nrot = (int)(theta/TWO_PI);
    return (theta < 0)? theta - (nrot - 1)*TWO_PI : theta - nrot*TWO_PI;
}

/*! neg_norm_angle
 *
 * return angle between 0 and -2pi
 */
float
neg_norm_angle(float theta) {
    float angle = norm_angle(theta);
    return angle - TWO_PI;
}

/*! center_angle
 *
 * return angle between -pi and pi
 */
float
center_angle(float theta) {
    float angle = norm_angle(theta);
    return (angle > PI)? angle - TWO_PI : angle;
}

/*! angle_diff
 *
 * Normalized difference between two angles
 */
float
angle_diff(float th1, float th2) {
    float diff = center_angle(th2 - th1);
    return diff;
}
