#include "localize.h"

#include "constants.h"
#include "types.h"
#include "kb_math.h"
#include "nbi.h"
#include "state.h"

#include "err.h"

/*! place
 *
 * Place a neighbor on the x-axis, and update the component array
 * appropriately
 */
static void
_place(
    nbrs_info_t *nbi,
    uint32_t pt_i)
{
    point_t pt = {0,0};
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    nbr_t *n = nbi_get_nbr(nbi, pt_i);
    ASSERT_OR_ERR(n, err, KB_ERR_INPUT);

    // place it on the x-axis
    pt.x = nbi_get_dist(nbi, pt_i, pt_i);
    pt.y = 0.0f;

    // covers only a single point at this time
    n->comp->coverage = 0.0f;
    n->comp->max_nbr = n;
    n->comp->min_nbr = n;

    // update the location
    n->last_loc = n->loc;
    n->loc = pt;

    // mark this neighbor localized
    nbr_set_localized(n);

err:
    return;
}

/*! Triangulation algorithm
 *
 * Triangulates a neighbor using ourselves and one reference point.
 */
static void
_triangulate(
    nbrs_info_t *nbi,
    uint32_t pt_i,
    uint32_t ref_i)
{
    point_t pt = {0,0};
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    // retrieve the reference from the array
    nbr_t *n = nbi_get_nbr(nbi, pt_i);
    nbr_t *ref = nbi_get_nbr(nbi, ref_i);

    ASSERT_OR_ERR(n && ref, err, KB_ERR_INPUT);

    // distnace to pt to localize
    kb_dist_t ab = nbi_get_dist(nbi, pt_i, pt_i);
    // distance to ref pt
    kb_dist_t ac = nbi_get_dist(nbi, ref_i, ref_i);
    // distance between pt to triangulate and ref
    kb_dist_t bc = nbi_get_dist(nbi, pt_i, ref_i);

    // compute angle subtended by pt and ref through origin
    // choice of theta versus -theta means we choose to place
    // this point counterclockwise from its reference
    // rather than clockwise
    float theta = angled(ab, ac, bc);
    float costheta = cos(theta);
    float sintheta = sin(theta);

    // compute relative coordinates assuming reference lies along
    // x-axis
    pt.x = ab*costheta;
    pt.y = ab*sintheta;

    // compute angle of reference point
    float reftheta = atan2(ref->loc.y, ref->loc.x);
    costheta = cos(reftheta);
    sintheta = sin(reftheta);

    // rotate point by theta in either direction to get
    // possible locations
    point_t ptccw = {
        pt.x*costheta - pt.y*sintheta,
        pt.x*sintheta + pt.y*costheta
    };
    point_t ptcw = {
        pt.x*costheta + pt.y*sintheta,
       -pt.x*sintheta + pt.y*costheta
    };

    pt = ptccw;
    float cov_angle = reftheta + theta;
    // check other neighbors of the reference
    // to disambiguate the choice
    for (uint32_t i = 0; i < nbi_get_nnbrs(nbi); ++i) {
        if (i == ref_i || i == pt_i) {
            continue;
        }

        nbr_t *nbr = nbi_get_nbr(nbi, i);

        // if ref has a neighbor which should neighbor
        // one of our points, then return the other. in cases
        // where the choice is symmetrical, we pick counterclockwise
        if (nbi_is_adj(nbi, ref_i, i)) {
            if (l2_sq(nbr->loc, ptcw) <= COMM_RANGE*COMM_RANGE) {
                pt = ptccw;
            }
            if (l2_sq(nbr->loc, ptccw) <= COMM_RANGE*COMM_RANGE) {
                cov_angle = reftheta - theta;
                pt = ptcw;
            }
        }
    }

    if (isnan(pt.x) || isnan(pt.y)) {
        DEBUG_PRINT("found nan: (%.2f, %.2f)", pt.x, pt.y);
        return;
    }

    // update location
    n->last_loc = n->loc;
    n->loc = pt;

    // update the component
    n->comp = ref->comp;
    netcomp_update(nbi, ref->comp, n);
    nbr_set_localized(n);

err:
    return;
}

/*! Trilateration algorithm
 *
 * Trilaterate point i using ourselves and points j and k as reference
 * points
 *
 * Computation is taken from
 * https://en.wikipedia.org/wiki/Trilateration#Derivation
 */
static void
_trilaterate(
    nbrs_info_t *nbi,
    uint32_t pt_i,
    uint32_t ref1_i, 
    uint32_t ref2_i)
{
    point_t point = {0,0};

    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    // get point and references
    nbr_t *nbr = nbi_get_nbr(nbi, pt_i);
    nbr_t *ref1 = nbi_get_nbr(nbi, ref1_i);
    nbr_t *ref2 = nbi_get_nbr(nbi, ref2_i);
    ASSERT_OR_ERR(nbr && ref1 && ref2, err, KB_ERR_INPUT);

    point_t ref1_pt = ref1->loc;
    point_t ref2_pt = ref2->loc;
    // if the second one is aligned to the x axis, switch them
    // to avoid computing the angle
    if (ref2_pt.y == 0 && ref1_pt.y != 0) {
        // swap pts
        point_t tmp = ref1_pt;
        ref1_pt = ref2_pt;
        ref2_pt = tmp;

        // swap indices
        uint32_t tmp_idx = ref1_i;
        ref1_i = ref2_i;
        ref2_i = tmp_idx;

        // swap nbrs
        nbr_t *tmp_nbr = ref1;
        ref1 = ref2;
        ref2 = tmp_nbr;
    }

    // distance from self to the point to trilaterate
    float r1 = nbi_get_dist(nbi, pt_i, pt_i);
    // distance from point to the first reference point
    float r2 = nbi_get_dist(nbi, pt_i, ref1_i);
    // distance from point to the second reference point
    float r3 = nbi_get_dist(nbi, pt_i, ref2_i);

    float theta = 0.0f;
    float sintheta = 0.0f;
    float costheta = 1.0f;

    // rotate only things if we're not using an aligned reference
    if (ref1_pt.y != 0) {
        theta = atan2((float)ref1_pt.y, (float)ref1_pt.x);
        sintheta = sin(theta);
        costheta = cos(theta);
    }

    // distance from self to first reference point
    float d =  nbi_get_dist(nbi, ref1_i, ref1_i);
    // rotate points of second reference clockwise
    float i =  ref2_pt.x*costheta + ref2_pt.y*sintheta;
    float j = -ref2_pt.x*sintheta + ref2_pt.y*costheta;

    // relative x value, assuming ref1 is on the x axis
    float relx = (r1*r1 - r2*r2 + d*d) / (2.0 * d); 
    float rely = ((r1*r1 - r3*r3 + i*i + j*j) / (2.0 * j));
    rely -= (i*(r1*r1 - r2*r2 + d*d)) / (j*(2.0 * d)); 

    // if it's a problem, it's also on the x-axis
    if (isnan(rely) || isinf(rely)) {
        rely = 0.0f;
    }

    // rotate back by -theta (cos(-x) = cos(x), sin(-x) = -sin(x))
    if (theta != 0.0f) {
        point.x = relx*costheta - rely*sintheta;
        point.y = relx*sintheta + rely*costheta;
    } else {
        point.x = relx;
        point.y = rely;
    }

    // compute angle of point relative to component local coordinates
    float angle = atan2(point.y, point.x);

    if (isnan(point.x) || isnan(point.y)) {
        DEBUG_PRINT("found nan: (%.2f, %.2f), r1: %.2f, r2: %.2f, r3: %.2f, d: %.2f, i: %.2f, j: %.2f", point.x, point.y, r1, r2, r3, d, i, j);
        return;
    }

    // update the point
    nbr->last_loc = nbr->loc;
    nbr->loc = point;

    // compute angle of point relative to ref1
    float relangle = atan2(rely, relx);
    // update the component
    nbr->comp = ref1->comp;
    // if relative to our fixed reference point we're closer cw than ccw,
    // use it as a cw angle
    if (relangle > PI) {
        angle -= TWO_PI;
    }
    netcomp_update(nbi, nbr->comp, nbr);
    nbr_set_localized(nbr);

err:
    return;
}

/*! localize_one
 *
 * Attempt to completely localize a single neighbor
 */
static void
_localize_one(
    nbrs_info_t *nbi,
    uint32_t nbr_i) 
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    nbr_t *nbr = nbi_get_nbr(nbi, nbr_i);
    ASSERT_OR_ERR(nbr, err, KB_ERR_INPUT);

    // try to find two localized references
    uint32_t nrefs = 0;
    uint32_t refs[2];
    refs[0] = nbi_find_nbr(nbi, nbr_i, 0, NBR_LOCALIZED);
    if (refs[0] != INVALID_INDEX) {
        refs[1] = nbi_find_nbr(nbi, nbr_i, refs[0]+1, NBR_LOCALIZED);

        // set number of references found
        if (refs[1] != INVALID_INDEX) {
            nrefs = 2;
        } else {
            nrefs = 1;
        }
    }

    // if we found both references, we can trilaterate
    if (nrefs == 2) {
        _trilaterate(nbi, nbr_i, refs[0], refs[1]);
    }

    // otherwise if we only found one, we can just triangulate
    // making a choice--if it's wrong that's ok
    else if (nrefs == 1) {
        _triangulate(nbi, nbr_i, refs[0]);
    }
    
    // if we didn't find any references we failed to
    // localize them this round
    else {
        nbr_clr_localized(nbr);
    }

err:
    return;
}

/*! localize_all
 *
 * This function updates the local coordinate system.
 * Picks the furthest neighbor to lie on the x-axis
 * for a canonical choice.
 *
 * Algorithm:
 *
 * 1. Build components
 *      - Determines local neighborhood segmentation
 * 2. Place one element of each component
 *      - For each component of the network, place a single nbr
 *      canonically.
 * 3. Localize all remaining neighbors
 *      - Maximum (comp_sz* - 1) rounds (comp_sz* is maximum number
 *      of neighbors in a single component)
 *      - For each newly localized neighbor, update detailed component
 *      information to keep track of the network layout.
 */
void
localize_all(state_t *st)
{
    ASSERT_OR_ERR(st, err, KB_ERR_INPUT);

    nbrs_info_t *nbi = st->nbi;

    // short circuit if we don't have any neighbors at all
    if (nbi->n_nbrs == 0) {
        return;
    }

    // 1: Build components
    nbi_segment_nbrs(nbi);

    //
    // If there are too many components, we need more
    // information to take a stab at localization
    //
    if (nbi->n_comps >= 5) {
        nbi->flags &= ~(NBI_LOCALIZED);
        return;
    }

    // Clear localization status of all neighbors
    for (uint32_t i = 0; i < nbi->n_nbrs; ++i) {
        nbi_nbr_clr_localized(nbi, i);
    }

    // Place the first (by index) element in each component
    for (uint32_t i = 0; i < nbi->n_comps; ++i) {
        _place(nbi, nbi->comps[i].min_nbr->idx);
    }

    // then try to do all of them
    bool someone_changed = true;
    uint32_t nnbrs = nbi_get_nnbrs(nbi);
    for (uint32_t i = 0; i < nnbrs - 1; ++i) {
        // if nothing changed last round we can't do any better this round
        if (!someone_changed) {
            break;
        }
        someone_changed = false;

        // try to place all MAX_NEIGHBORS in the coordinate system
        for (uint32_t j = 0; j < nnbrs; ++j) {
            nbr_t *nbr = nbi_get_nbr(nbi, j);

            // don't need to try to relocalize already-localized
            // points
            if (nbr_is_localized(nbr)) {
                continue;
            }

            // try to localize j
            _localize_one(nbi, j);

            // if we failed to localize this point, we need at least
            // one more round
            if (!nbr_is_localized(nbr)) {
                someone_changed = true;
            }
        }
    }

    // once everyone has been localized, check to see if any components
    // are full
    for (uint32_t i = 0; i < nbi->n_comps; ++i) {
        netcomp_check_full(nbi, &nbi->comps[i]);
    }

    // if we got here, we can call everything localized
    nbi_set_flag(nbi, NBI_LOCALIZED);

err:
    return;
}
