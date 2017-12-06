#include "netcomp.h"

#include "constants.h"
#include "err.h"
#include "kb_math.h"
#include "nbi.h"

/*! netcomp_init
 *
 * In-place constructor for netcomp_t
 */
void
netcomp_init(netcomp_t *comp)
{
    ASSERT_OR_ERR(comp, err, KB_ERR_INPUT);

    comp->start_angle = 0.0f;
    comp->coverage = 0.0f;
    comp->max_nbr = NULL;
    comp->min_nbr = NULL;

err:
    return;
}

/*! netcomp_check_full
 *
 * check if a component is full by checking the angle of the min
 * and max
 */
void
netcomp_check_full(nbrs_info_t *nbi, netcomp_t *comp)
{
    ASSERT_OR_ERR(comp, err, KB_ERR_INPUT);

    // from reference, most ccw
    float amax = norm_angle(atan2(comp->max_nbr->loc.y, comp->max_nbr->loc.x));
    // from reference, most cw
    float amin = norm_angle(atan2(comp->min_nbr->loc.y, comp->min_nbr->loc.x));

    comp->start_angle = center_angle(amin);
    comp->coverage = norm_angle(amax - amin);

    // if the coverage is more than 2pi and the start and end connect
    // to each other, call it full coverage
    if (comp->coverage > PI && comp->max_nbr != comp->min_nbr
        && nbi_is_adj(nbi, comp->max_nbr->idx, comp->min_nbr->idx))
    {
        comp->coverage = TWO_PI;
    }

err:
    return;
}

/*! netcomp_update
 *
 * updates a component to add a new point angle
 */
void
netcomp_update(
    nbrs_info_t *nbi,
    netcomp_t *comp,
    nbr_t *nbr)
{
    float angle = norm_angle(atan2(nbr->loc.y, nbr->loc.x));
    float neg_angle = neg_norm_angle(angle);

    // should be positive if angle is less than pi ccw from the end
    // of the componend, negative if less than pi cw
    float upper_diff = angle - comp->start_angle - comp->coverage;
    // should be negative if angle is less than pi cw from the start of
    // the component, negative if less than pi ccw
    float lower_diff = neg_angle - comp->start_angle;

    // first neighbor in a component
    if (comp->max_nbr == NULL && comp->min_nbr == NULL) {
        comp->max_nbr = nbr;
        comp->min_nbr = nbr;
        comp->start_angle = angle;
        comp->coverage = 0.0f;
    }

    // if this is true, should be outside the region
    else if (upper_diff > 0 && lower_diff < 0) {
        if (upper_diff < -lower_diff) {
            comp->max_nbr = nbr;
        } else {
            comp->min_nbr = nbr;
        }
    }

    netcomp_check_full(nbi, comp);
}

/*! netcomp_contains
 *
 * check if a given point is already contained in a component
 */
bool
netcomp_contains(
    netcomp_t *comp,
    point_t *pt)
{
    float angle = atan2(pt->y, pt->x);
    return ((angle >= comp->start_angle)
         && (angle <= (comp->start_angle + comp->coverage)));
}

/*! netcomp_print
 *
 * Print the component
 */
void
netcomp_print(netcomp_t *comp, char *pref)
{
    ASSERT_OR_ERR(comp && pref, err, KB_ERR_INPUT);

    printf("%scomponent: %p\n", pref, comp);
    printf("%s\tstart_angle: %0.4f\n", pref, comp->start_angle);
    printf("%s\tcoverage: %0.4f\n", pref, comp->coverage);
    printf("%s\tmax_nbr: %p\n", pref, comp->max_nbr);
    printf("%s\tmin_nbr: %p\n", pref, comp->min_nbr);

err:
    return;
}
