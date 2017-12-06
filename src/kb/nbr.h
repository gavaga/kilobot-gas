#ifndef __NBR_H__
#define __NBR_H__

#include "types.h"
#include "netcomp.h"

// Forward Declarations
typedef struct netcomp_t netcomp_t;

/*! nbr_t
 *
 * Type used to keep track of most info for a single neighbor
 */
typedef struct nbr_t {
    /*! id
     *
     * Universal id of the neighbor used to distinguish
     * from other neighbors
     */
    kb_id_t id;

    /*! idx
     *
     * Index of the neighbor in the neighbor array. Used to go
     * from pointer to array index
     */
    uint32_t idx;

    /*! last_time
     *
     * Time (ticks) that we last received a message from them.
     * Used primarily for eviction policy.
     */
    kb_time_t last_time;

#define NBR_LOCALIZED 0x1

    /*! flags
     *
     * Various flags
     */
    uint8_t flags;

    /*! hopct
     *
     * Hop count from the seed
     */
    uint8_t hopct;

    /*! rsvd
     *
     * Explicit padding
     */
    uint8_t rsvd[2];

    /*! loc
     *
     * Assigned position in component-local coordinate system
     */
    point_t loc;

    /*! last_loc
     *
     * Assigned position in component-local coordinate system
     */
    point_t last_loc;
    
    /*! comp
     *
     * Pointer to the network component this neighbor is a part of.
     */
    netcomp_t *comp;
} nbr_t;

// Constructors/Destructors
nbr_t *nbr_create(kb_id_t id);
void nbr_init(nbr_t *n, kb_id_t id);
void nbr_clean(nbr_t *n);
void nbr_delete(nbr_t *n);

// Check/manipulate flag functions
bool nbr_flag_is_set(nbr_t *n, uint32_t flag);
void nbr_set_flag(nbr_t *n, uint32_t flag);
void nbr_clr_flag(nbr_t *n, uint32_t flag);

// Specific flag functions
bool nbr_is_localized(nbr_t *n);
void nbr_set_localized(nbr_t *n);
void nbr_clr_localized(nbr_t *n);

// Debug
void nbr_print(nbr_t *n, char *pref);

#endif
