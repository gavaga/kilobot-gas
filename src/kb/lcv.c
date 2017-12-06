#include <kilolib.h>

#include "lcv.h"

#include <stdlib.h>

#include "constants.h"
#include "types.h"
#include "localize.h"

#include "err.h"

/*! check_lcv
 *
 * Algorithm from Fayed, et al. 2007
 *
 * Probabalistically determines whether we're in our
 * own local convex view (lcv).
 */
bool
nbi_check_lcv(nbrs_info_t *nbi)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    // if there's a single connected component, then we're only
    // in our own convex hull if it covers less than pi radians
    if (nbi->n_comps == 1) {
        return nbi->comps[0].coverage <= PI*(1+HULL_THRESHOLD);
    }

    // for two disconnected components: see if it's possible. if it is,
    // use a probabilistic approach. if not, just return 0.
    //
    // for three disconnected components: treat it almost like a large
    // component with just a smaller available region, for simplicity
    else if (nbi->n_comps == 2 || nbi->n_comps == 3)
    {
        float alpha = nbi->comps[0].coverage;
        float gamma, gap;
        if (nbi->n_comps == 2) {
            gamma = nbi->comps[1].coverage;
            gap = 2.0*PI/3.0;
        } else {
            gamma = nbi->comps[1].coverage + nbi->comps[2].coverage;
            gap = PI/3.0;
        }

        // gap between components demanded by geometry
        if (alpha + gamma > gap) {
            return false;
        }

        // otherwise need a probabilistic approach
        else {
            // probability of being in hull
            float prob = (4*PI/3.0) - 2.0*alpha - 2.0*gamma;
            prob /= (4*PI/3.0) - alpha - gamma;

            uint8_t probi = (int)(prob*100.0);
            if (rand_hard()%100 < probi) {
                return true;
            } else {
                return false;
            }
        }
    }

    // can't be in our own convex hull if there are more than 3
    // disconnected components
    else {
        return false;
    }

err:
    return false;
}
