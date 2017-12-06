#include "nbi.h"

#include <kilolib.h>

#include <stdlib.h>
#include <string.h>

#include "bitarray.h"
#include "constants.h"
#include "kb_math.h"
#include "err.h"

/*! nbi_create
 *
 * Constructor for nbrs_info_t
 */
nbrs_info_t *
nbi_create(
    uint32_t max_nbrs,
    uint8_t *raw,
    uint8_t pol)
{
    return NULL;
    /*ASSERT_OR_ERR(max_nbrs > 0, err, KB_ERR_INPUT);*/

    /*nbrs_info_t *nbi = (nbrs_info_t*)malloc(sizeof(nbrs_info_t));*/
    /*ASSERT_OR_ERR(nbi, err, KB_ERR_OOM);*/

    /*nbi_init(nbi, raw, max_nbrs, pol);*/
    /*return nbi;*/
/*err:*/
    /*return NULL;*/
}

/*! nbi_init
 *
 * In-place constructor for nbrs_info_t
 *
 * @param[in] nbi       Pointer to nbrs_info_t to initialize
 * @param[in] max_nbrs  Maximum number of neighbors to allow
 * @param[in] pol       Eviction policy
 */
void
nbi_init(
    nbrs_info_t *nbi,
    uint32_t max_nbrs,
    uint8_t pol,
    nbr_t *nbrs, uint32_t nbrs_sz,
    matf_t *pd, float *pd_data, uint32_t pd_data_sz,
    bitmat_t *adj, uint8_t *adj_data, uint32_t adj_data_sz)
{
    ASSERT_OR_ERR(nbi && max_nbrs > 0 && nbrs && nbrs_sz == max_nbrs,
            err, KB_ERR_INPUT);

    // init direct parameters
    nbi->n_nbrs = 0;
    nbi->max_nbrs = max_nbrs;
    nbi->pol = pol;

    // setup the neighbor array
    nbi->nbrs = nbrs;

    // initialize the neighbor array with invalid indices
    for (uint32_t i = 0; i < max_nbrs; ++i) {
        nbr_init(&nbi->nbrs[i], KB_ID_INVALID);

        // setup the indices for each
        nbi->nbrs[i].idx = i;
    }

    // setup the pairwise distance matrix
    nbi->pd = pd;
    matf_init(nbi->pd, max_nbrs, 0, MATF_SYMMETRIC, pd_data, pd_data_sz);

    // setup the adjacency matrix
    nbi->adj = adj;
    bm_init(nbi->adj, max_nbrs, 0, BM_SYMMETRIC, adj_data, adj_data_sz);

    // initalize all the components
    for (uint32_t i = 0; i < 5; ++i) {
        netcomp_init(&nbi->comps[i]);
    }

    return;
err:
    nbi_clean(nbi);
    return;
}

/*! nbi_clean
 *
 * In-place destructor
 */
void
nbi_clean(nbrs_info_t *nbi)
{
    if (nbi == NULL) return;

    bm_delete(nbi->adj);
    matf_delete(nbi->pd);

    if (nbi->nbrs) {
        for (uint32_t i = 0; i < nbi->max_nbrs; ++i) {
            nbr_clean(&nbi->nbrs[i]);
        }
        free(nbi->nbrs);
    }
}

/*! nbi_delete
 *
 * Destructor
 */
void
nbi_delete(nbrs_info_t *nbi)
{
    if (nbi == NULL) return;

    nbi_clean(nbi);
    free(nbi);
}

/*---------- Access/Manipulate Neighbors -----------*/

/*! nbi_get_nbr
 *
 * Get a pointer to a neighbor object by index
 */
nbr_t *
nbi_get_nbr(
    nbrs_info_t *nbi,
    uint32_t idx)
{
    ASSERT_OR_ERR(nbi && idx < nbi->n_nbrs, err, KB_ERR_INPUT);

    return &nbi->nbrs[idx];
err:
    return NULL;
}

/*! nbi_get_nbr_by_id
 *
 * Get a pointer to a neighbor object by id
 */
nbr_t *
nbi_get_nbr_by_id(
    nbrs_info_t *nbi,
    kb_id_t id)
{
    ASSERT_OR_ERR(nbi && id != KB_ID_INVALID, err, KB_ERR_INPUT);

    uint32_t idx = nbi_get_nbr_idx(nbi, id);
    ASSERT_OR_ERR(idx < nbi->n_nbrs, err, KB_ERR_BOUNDS);

    return nbi_get_nbr(nbi, idx);
err:
    return NULL;
}

/*! nbi_get_nbr_idx
 *
 * Traverse the neighbor array to find an index
 */
uint32_t
nbi_get_nbr_idx(
    nbrs_info_t *nbi,
    kb_id_t id)
{
    ASSERT_OR_ERR(nbi && id != KB_ID_INVALID, err, KB_ERR_INPUT);

    for (uint32_t i = 0; i < nbi->n_nbrs; ++i) {
        if (nbi->nbrs[i].id == id) {
            return i;
        }
    }

err:
    return INVALID_INDEX;
}

/*! nbi_update_id
 *
 * Add/update a neighbor with the given ID to the list of neighbors.
 */
uint32_t
nbi_update_id(
    nbrs_info_t *nbi,
    kb_id_t id,
    kb_time_t ticks)
{
    ASSERT_OR_ERR(nbi && id != KB_ID_INVALID, err, KB_ERR_INPUT);

    nbr_t *nbr = NULL;

    // check if the nbr is already in the array
    uint32_t idx = nbi_get_nbr_idx(nbi, id);
    if (idx != INVALID_INDEX) {
        nbr = nbi_get_nbr(nbi, idx);
    } else {
        // increment the number of neighbors and
        // initialize it
        nbr = &nbi->nbrs[nbi->n_nbrs++];
        nbr_init(nbr, id);

        // mark the last updated time
        nbi->last_new_ticks = ticks;
    }

    return nbr->idx;

err:
    return INVALID_INDEX;
}

/*! nbi_add_nbr
 *
 * Add a neighbor to the array
 *
 * @return Index of the newly added neighbor
 */
uint32_t
nbi_add_nbr(
    nbrs_info_t *nbi,
    nbr_t *nbr)
{
    ASSERT_OR_ERR(nbi && nbr, err, KB_ERR_INPUT);

    uint32_t idx = INVALID_INDEX;
    // if we're not at capacity yet, we can just n_nbrs as index
    if (nbi->n_nbrs < nbi->max_nbrs) {
        idx = nbi->n_nbrs;
        ++nbi->n_nbrs;
    }
    // otherwise we have to evict someone
    else {
        idx = nbi_evict_nbr(nbi);
        ASSERT_OR_ERR(idx != INVALID_INDEX, err, KB_ERR_EVICT);
    }

    // once the index is decided, we can copy the data
    memcpy(&nbi->nbrs[idx], nbr, sizeof(nbr_t));
    if (nbr->last_time > nbi->last_new_ticks) {
        nbi->last_new_ticks = nbr->last_time;
    }

    return idx;
err:
    return INVALID_INDEX;
}

/*! nbi_nbr_exists
 *
 * Check if there's a neighbor with that index that already exists
 */
bool
nbi_nbr_exists(
    nbrs_info_t *nbi,
    uint32_t idx)
{
    ASSERT_OR_ERR(nbi && idx < nbi->n_nbrs, err, KB_ERR_INPUT);

    return nbi->nbrs[idx].idx != KB_ID_INVALID;
err:
    return false;
}

/*! nbi_find_nbr
 *
 * Find a different neighbor that is adjacent to nbr_i, starting
 * at index st and return its index.
 *
 * If there is no such neighbor, return INVALID_INDEX
 */
uint32_t
nbi_find_nbr(
    nbrs_info_t *nbi,
    uint32_t nbr_i,
    uint32_t st,
    uint8_t cond)
{
    for (uint32_t i = st; i < nbi->n_nbrs; ++i) {
        if (nbi_is_adj(nbi, nbr_i, i)
                && nbi_nbr_flag_is_set(nbi, i, cond))
        {
            return i;
        }
    }

    return INVALID_INDEX;
}

/*! nbi_get_nnbrs
 *
 * Return the number of neighbors
 */
uint32_t
nbi_get_nnbrs(nbrs_info_t *nbi)
{
    return nbi->n_nbrs;
}

/*! nbi_evict_nbr
 *
 * Eviction function
 */
uint32_t
nbi_evict_nbr(nbrs_info_t *nbi)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    // if the neighbor array isn't full, just return the first empty one
    if (nbi->n_nbrs < nbi->max_nbrs) {
        return nbi->n_nbrs++;
    }

    // otherwise use the eviction policy
    uint32_t idx = INVALID_INDEX;
    switch (nbi->pol) {
        case POL_EVICT_OLDEST: {
            // find the oldest
            kb_time_t oldest = (kb_time_t)-1;
            for (uint32_t i = 0; i < nbi->n_nbrs; ++i) {
                if (nbi->nbrs[i].last_time < oldest) {
                    oldest = nbi->nbrs[i].last_time;
                    idx = i;
                }
            }
            ASSERT_OR_GOTO(idx != INVALID_INDEX, err, "Error finding oldest");

            // clear the info structure
            nbr_clean(&nbi->nbrs[idx]);
            // clear recorded distances
            for (uint32_t i = 0; i < MAX_NEIGHBORS; ++i) {
                nbi_clr_dist(nbi, i, idx);
                nbi_clr_adj(nbi, i, idx);
            }
            break;
        }
        default:
            goto err;
    }

    return idx;
err:
    return INVALID_INDEX;
}

/*! nbi_is_adj
 *
 * Check if i and j are adjacent
 */
bool
nbi_is_adj(
    nbrs_info_t *nbi,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    return bm_is_set(nbi->adj, i, j);
err:
    return 0;
}

/*! nbi_is_connected
 *
 * Determine if i is connected to j by any known route.
 * Currently uses Depth-first search
 */
bool
nbi_is_connected(
    nbrs_info_t *nbi,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    // objects to act as raw array for the resultant/temp bitmats
    uint8_t _acc_raw[STATIC_SIZE_NBI_ADJ_DATA];
    uint8_t _next_raw[STATIC_SIZE_NBI_ADJ_DATA];

    // short circuit if they're trivially connected
    if (nbi_is_adj(nbi, i, j)) {
        return true;
    }

    // accumulator matrix, statically allocated
    bitmat_t acc;
    bm_init(&acc, nbi->adj->n, 0, BM_SYMMETRIC,
            _acc_raw, STATIC_SIZE_NBI_ADJ_DATA);
    bm_copy(&acc, nbi->adj);

    bitmat_t next;
    bm_init(&next, nbi->adj->n, 0, BM_SYMMETRIC,
            _next_raw, STATIC_SIZE_NBI_ADJ_DATA);

    // Use properties of adjacency matrices
    //
    // 1. Nilpotent of order <= dimension
    //
    // 2. adj^k is the adj matrix of length-k paths
    for (uint32_t k = 0; k < nbi_get_nnbrs(nbi); ++k)
    {
        // multiply the accumulated value
        bm_mult(&acc, nbi->adj, &next);
        bm_copy(&acc, &next);

        // if for some power k the two are adjacent,
        // then the two are connected by a path of length k
        if (bm_is_set(&acc, i, j)) {
            return true;
        }
    }

err:
    return false;
}

/*! nbi_segment_nbrs
 *
 * Segment neighbors into their disconnected components using adjacency
 * matrix
 */
void
nbi_segment_nbrs(nbrs_info_t *nbi)
{
    // objects to act as raw array for the resultant/temp bitmats
    uint8_t _acc_raw[STATIC_SIZE_NBI_ADJ_DATA];
    uint8_t _next_raw[STATIC_SIZE_NBI_ADJ_DATA];

    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    uint32_t comps[MAX_NEIGHBORS];

    // accumulator matrix, statically allocated
    bitmat_t acc;
    bm_init(&acc, nbi->adj->n, 0, BM_SYMMETRIC,
            _acc_raw, STATIC_SIZE_NBI_ADJ_DATA);
    bm_copy(&acc, nbi->adj);

    bitmat_t next;
    bm_init(&next, nbi->adj->n, 0, BM_SYMMETRIC,
            _next_raw, STATIC_SIZE_NBI_ADJ_DATA);

    uint32_t nnbrs = nbi_get_nnbrs(nbi);
    for (uint32_t i = 0; i < nnbrs; ++i) {
        // initially set components equal to themselves
        comps[i] = i;
    }

    //
    // Use properties of adjacency matrices
    //
    // 1. adj^k is the adj matrix of length-k paths
    //
    for (uint32_t k = 0; k < nnbrs; ++k)
    {
        //
        // prune components
        //
        // For each neighbor, set our value to the smallest index
        // of neighbors that we're connected to
        //
        for (uint32_t i = 1; i < nnbrs; ++i) {
            for (uint32_t j = 0; j < i; ++j) {
                if (bm_is_set(&acc, i, j)) {
                    if (j < comps[i]) {
                        comps[i] = j;
                    }
                    // break because we only need to consider
                    // the smallest for each round
                    break;
                }
            }
        }

        // multiply the accumulated value
        bm_mult(&acc, nbi->adj, &next);
        bm_copy(&acc, &next);
        // clear the result array
        bm_fill(&next, 0x0);
    }

    //
    // setup components
    //
    // Anything in the list that still has its own index for a value
    // is the smallest neighbor with a given component. For those we
    // increase the number of components. Anything that doesn't
    // have its own index for value is a neighbor which is connected
    // to a neighbor of a smaller index through some chain, so since
    // we set components in increasing index order, we can safely
    // set their components to those already set for the smaller index.
    //
    uint32_t ncomps = 0;
    for (uint32_t i = 0; i < nnbrs; ++i) {
        if (comps[i] == i) {
            // if we have too many components, we don't have enough information to compute anything
            if (ncomps >= 5) {
                nbi->nbrs[i].comp = NULL;
                nbi->n_comps = 5;
                return;
            }

            netcomp_t *comp = &nbi->comps[ncomps++];
            // clear any prevous component info
            netcomp_init(comp);
            // add nbr i to the component
            netcomp_update(nbi, comp, nbi_get_nbr(nbi, i));

            nbi->nbrs[i].comp = comp;
        } else {
            nbi->nbrs[i].comp = nbi->nbrs[comps[i]].comp;
        }
    }
    nbi->n_comps = ncomps;

err:
    return;
}

/*! nbi_set_adj
 *
 * Mark i and j adjacent
 */
void
nbi_set_adj(
        nbrs_info_t *nbi,
        uint32_t i,
        uint32_t j)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    if (nbi_is_adj(nbi, i, j)) {
    }
    bm_set(nbi->adj, i, j);
err:
    return;
}

/*! nbi_clr_adj
 *
 * Mark i and j nonadjacent
 */
void
nbi_clr_adj(
    nbrs_info_t *nbi,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    bm_clr(nbi->adj, i, j);
err:
    return;
}

/*! nbi_get_dist
 *
 * Get the distance between neighbors i and j. If i == j 
 * this returns the distance from this node to neighbor i
 */
kb_dist_t
nbi_get_dist(
    nbrs_info_t *nbi,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    return matf_get(nbi->pd, i, j);
err:
    return KB_DIST_INVALID;
}

/*! nbi_set_dist
 *
 * Update the distance from neighbor i to neighbor j. If i == j, this
 * updates the distance from this node to neighbor i.
 */
void
nbi_set_dist(
    nbrs_info_t *nbi,
    uint32_t i,
    uint32_t j,
    kb_dist_t dist)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    matf_set(nbi->pd, i, j, dist);
err:
    return;
}

/*! nbi_clr_dist
 *
 * Clears the distance from neighbor i to j.
 */
void
nbi_clr_dist(
    nbrs_info_t *nbi,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    matf_set(nbi->pd, i, j, KB_DIST_INVALID);
err:
    return;
}

/*! nbi_get_furthest_idx
 *
 * Get the index of the furthest neighbor
 */
nbr_t *
nbi_get_furthest(nbrs_info_t *nbi)
{
    uint32_t furthest;
    kb_dist_t tmp_dist;
    kb_dist_t furthest_dist = 0.0f;

    for (uint32_t i = 0; i < nbi->n_nbrs; ++i) {
        tmp_dist = nbi_get_dist(nbi, i, i);

        if (tmp_dist > furthest_dist) {
            furthest = i;
            furthest_dist = tmp_dist;
        }
    }

    return nbi_get_nbr(nbi, furthest);
}

/*! nbi_flag_is_set
 *
 * Check if flag is set
 */
bool
nbi_flag_is_set(
    nbrs_info_t *nbi,
    uint32_t flag)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

    return nbi->flags & flag;
err:
    return false;
}

/*! nbi_set_flag
 *
 * Set the flag
 */
void
nbi_set_flag(
    nbrs_info_t *nbi,
    uint32_t flag)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    nbi->flags |= flag;
err:
    return;
}

/*! nbi_clr_flag
 *
 * Clear the flag
 */
void
nbi_clr_flag(
    nbrs_info_t *nbi,
    uint32_t flag)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    nbi->flags &= ~flag;
err:
    return;
}

/*! nbi_nbr_flag_is_set
 *
 * Check if the neighbor flag is set for the given index
 */
bool
nbi_nbr_flag_is_set(
    nbrs_info_t *nbi,
    uint32_t idx,
    uint32_t flag)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    nbr_t *n = nbi_get_nbr(nbi, idx);
    return nbr_flag_is_set(n, flag);
err:
    return false;
}

/*! nbi_nbr_set_flag
 *
 * Mark flag for the given index neighbor
 */
void
nbi_nbr_set_flag(
    nbrs_info_t *nbi,
    uint32_t idx,
    uint32_t flag)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    nbr_t *n = nbi_get_nbr(nbi, idx);
    nbr_set_flag(n, flag);
err:
    return;
}

/*! nbi_nbr_clr_flag
 *
 * Clear flag for the given index nbr
 */
void
nbi_nbr_clr_flag(
    nbrs_info_t *nbi,
    uint32_t idx,
    uint32_t flag)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);
    nbr_t *n = nbi_get_nbr(nbi, idx);
    nbr_clr_flag(n, flag);
err: 
    return;
}

/*! nbi_is_localized
 *
 * Check the NBI_LOCALIZED flag
 */
bool
nbi_is_localized(nbrs_info_t *nbi) {
    return nbi->flags & NBI_LOCALIZED;
}

/*! nbi_set_localized
 *
 * Mark the NBI_LOCALIZED flag
 */
void
nbi_set_localized(nbrs_info_t *nbi) {
    nbi->flags |= NBI_LOCALIZED;
}

/*! nbi_clr_localized
 *
 * Clear the NBI_LOCALIZED flag
 */
void
nbi_clr_localized(nbrs_info_t *nbi) {
    nbi->flags &= ~(NBI_LOCALIZED);
}

/*! nbi_nbr_is_localized
 *
 * Check if nbr idx is localized
 */
bool
nbi_nbr_is_localized(
    nbrs_info_t *nbi,
    uint32_t idx)
{
    return nbi_nbr_flag_is_set(nbi, idx, NBR_LOCALIZED);
}

/*! nbi_nbr_set_localized
 *
 * Set nbr idx localized
 */
void
nbi_nbr_set_localized(
    nbrs_info_t *nbi,
    uint32_t idx)
{
    nbi_nbr_set_flag(nbi, idx, NBR_LOCALIZED);
}

/*! nbi_nbr_clr_localized
 *
 * Clear nbr idx localized
 */
void
nbi_nbr_clr_localized(
    nbrs_info_t *nbi,
    uint32_t idx)
{
    nbi_nbr_clr_flag(nbi, idx, NBR_LOCALIZED);
}

/*! nbi_guess_orientation
 *
 * Guess the current orientation of the robot
 * relative to the current neighbor information,
 * given that since the last timestep action has been
 * taken
 *
 * TODO: implement
 */
float
nbi_guess_orientation(
    nbrs_info_t *nbi,
    uint8_t action)
{
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

err:
    return 0.0f;
}

/*! nbi_repulsion_vec
 *
 * Compute a repulsion vector as a weighted sum over the neighbors
 * in each component, with some informed guess as to the relationship
 * between components
 */
point_t
nbi_repulsion_vec(nbrs_info_t *nbi)
{
    point_t pt = {0,0};
    ASSERT_OR_ERR(nbi, err, KB_ERR_INPUT);

err:
    return pt;
}

/*! nbi_print
 *
 * Print the whole neighbors info array
 */
void
nbi_print(nbrs_info_t *nbi, char* pref)
{
    ASSERT_OR_ERR(nbi && pref, err, KB_ERR_INPUT);

    char next_pref[16];
    strcpy(next_pref, pref);
    strcat(next_pref, "\t\t");

    printf("%snbi: %p\n", pref, nbi);
    printf("%s\tn_nbrs: %d\n", pref, nbi->n_nbrs);
    printf("%s\tmax_nbrs: %d\n", pref, nbi->max_nbrs);
    printf("%s\tpol: ", pref);
    switch (nbi->pol) {
        case POL_EVICT_OLDEST:
            printf("evict_oldest");
            break;

        default:
            printf("unknown");
    }
    printf("\n");
    printf("%s\tflags: 0x%x\n", pref, nbi->flags);

    printf("%s\tnbrs[%d]: %p\n", pref, nbi->n_nbrs, nbi->nbrs);
    for (uint32_t i = 0; i < nbi->n_nbrs; ++i) {
        nbr_print(&nbi->nbrs[i], next_pref);
    }
    printf("\n");

    printf("%s\tpd: %p\n", pref, nbi->pd);
    matf_print(nbi->pd, next_pref);
    printf("\n");

    printf("%s\tadj: %p\n", pref, nbi->adj);
    bm_print(nbi->adj, next_pref);
    printf("\n");

    printf("%s\tcomps[%d]:\n", pref, nbi->n_comps);
    for (uint32_t i = 0; i < nbi->n_comps; ++i) {
        netcomp_print(&nbi->comps[i], next_pref);
    }
    printf("\n");
err:
    return;
}
