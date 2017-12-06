#ifndef __NBI_H__
#define __NBI_H__

#include "constants.h"
#include "types.h"
#include "matf.h"
#include "bitarray.h"
#include "nbr.h"
#include "netcomp.h"

/*! nbrs_info_t
 *
 * Type used to keep track of all nbr information
 */
typedef struct nbrs_info_t {
    /*! n_nbrs
     *
     * Size of nbr info array (number of current nbrs)
     */
    uint32_t n_nbrs;

    /*! max_nbrs
     *
     * Size of nbr info array (capacity)
     */
    uint32_t max_nbrs;

#define POL_EVICT_OLDEST    0x1

    /*! pol
     *
     * Eviction policy.
     */
    uint8_t pol;

#define NBI_LOCALIZED        0x1

    /*! flags
     *
     * Various flags
     */
    uint8_t flags;

    /*! rsvd
     *
     * Reserved space--currently unused. Explicit padding
     */
    uint8_t rsvd[2];

    /*! last_new_ticks
     *
     * Time we last added a new neighbor
     */
    kb_time_t last_new_ticks;

    /*! repulse
     *
     * Repulsion vector from last time we localized the whole neighborhood
     */
    point_t repulse;

    /*! nbrs
     *
     * Neighbor info array. Allocated size determined by max_nbrs
     */
    nbr_t *nbrs;

    /*! pd
     *
     * pairwise distance array
     * 
     * size given by 0.5*n_sz(n_sz + 1)
     */
    matf_t *pd;

    /*! adj
     *
     * One-hop nbr bit matrix
     *
     * size given by ceil(0.5*n_sz(n_sz+1) / 8)
     */
    bitmat_t *adj;

    /*! comps
     *
     * Component array. Keeps track of disconnected components as
     * nbrs are added.
     */
    netcomp_t comps[5];

    /*! n_comps
     *
     * Size of component array. How many disconnected components
     * there are in the local subgraph, based on current beliefs
     */
    uint32_t n_comps;
} nbrs_info_t;

// Constructors/Destructors
nbrs_info_t *nbi_create(uint32_t max_nbrs, uint8_t *raw, uint8_t pol);
void nbi_init(nbrs_info_t *nbi, uint32_t max_nbrs, uint8_t pol,
              nbr_t *nbrs, uint32_t nbrs_sz,
              matf_t *pd, float *pd_data, uint32_t pd_data_sz,
              bitmat_t *adj, uint8_t *adj_data, uint32_t adj_data_sz);
void nbi_clean(nbrs_info_t *nbi);
void nbi_delete(nbrs_info_t *nbi);

// Access/Manipulate Neighbors
nbr_t *nbi_get_nbr(nbrs_info_t *nbi, uint32_t idx);
nbr_t *nbi_get_nbr_by_id(nbrs_info_t *nbi, kb_id_t id);
uint32_t nbi_get_nbr_idx(nbrs_info_t *nbi, kb_id_t id);
uint32_t nbi_update_id(nbrs_info_t *nbi, kb_id_t id, kb_time_t ticks);
uint32_t nbi_add_nbr(nbrs_info_t *nbi, nbr_t *nbr);
bool nbi_nbr_exists(nbrs_info_t *nbi, uint32_t idx);
uint32_t nbi_find_nbr(nbrs_info_t *nbi, uint32_t nbr_i, uint32_t st, uint8_t cond);
uint32_t nbi_get_nnbrs(nbrs_info_t *nbi);

// Eviction function
uint32_t nbi_evict_nbr(nbrs_info_t *nbi);

// Adjacency functions
bool nbi_is_adj(nbrs_info_t *nbi, uint32_t i, uint32_t j);
void nbi_set_adj(nbrs_info_t *nbi, uint32_t i, uint32_t j);
void nbi_clr_adj(nbrs_info_t *nbi, uint32_t i, uint32_t j);
bool nbi_is_connected(nbrs_info_t *nbi, uint32_t i, uint32_t j);
void nbi_segment_nbrs(nbrs_info_t *nbi);

// Distance functions
kb_dist_t nbi_get_dist(nbrs_info_t *nbi, uint32_t i, uint32_t j);
void nbi_set_dist(nbrs_info_t *nbi, uint32_t i, uint32_t j, kb_dist_t dist);
void nbi_clr_dist(nbrs_info_t *nbi, uint32_t i, uint32_t j);
nbr_t *nbi_get_furthest(nbrs_info_t *nbi);

// Check/manipulate flag functions
bool nbi_flag_is_set(nbrs_info_t *nbi, uint32_t flag);
void nbi_set_flag(nbrs_info_t *nbi, uint32_t flag);
void nbi_clr_flag(nbrs_info_t *nbi, uint32_t flag);
bool nbi_nbr_flag_is_set(nbrs_info_t *nbi, uint32_t idx, uint32_t flag);
void nbi_nbr_set_flag(nbrs_info_t *nbi, uint32_t idx, uint32_t flag);
void nbi_nbr_clr_flag(nbrs_info_t *nbi, uint32_t idx, uint32_t flag);

// Specific flag functions
bool nbi_is_localized(nbrs_info_t *nbi);
bool nbi_nbr_is_localized(nbrs_info_t *nbi, uint32_t idx);
void nbi_nbr_set_localized(nbrs_info_t *nbi, uint32_t idx);
void nbi_nbr_clr_localized(nbrs_info_t *nbi, uint32_t idx);

// Orientation
float nbi_guess_orientation(nbrs_info_t *nbi, uint8_t action);

// Flocking functions
point_t nbi_repulsion_vec(nbrs_info_t *nbi);

// Debug
void nbi_print(nbrs_info_t *nbi, char* pref);

#endif
