#ifndef __NETCOMP_H__
#define __NETCOMP_H__

#include "types.h"

//Forward Declarations
typedef struct nbrs_info_t nbrs_info_t;
typedef struct nbr_t nbr_t;

/*! netcomp_t
 *
 * Type used to keep track of local network graph components
 */
typedef struct netcomp_t {
    /*! start_angle
     *
     * In component reference frame, angle of most cw neighbor.
     */
    float start_angle;
    
    /*! coverage
     *
     * Angle covered by component
     */
    float coverage;

    /*! max_nbr
     *
     * Pointer to most ccw neighbor in component
     */
    nbr_t *max_nbr;

    /*! min_nbr
     *
     * Pointer to most cw neighbor in component
     */
    nbr_t *min_nbr;
} netcomp_t;

// Constructor
void netcomp_init(netcomp_t *comp);

// Other functions
void netcomp_check_full(nbrs_info_t *nbi, netcomp_t *comp);
void netcomp_update(nbrs_info_t *nbi, netcomp_t *comp, nbr_t *nbr);
bool netcomp_contains(netcomp_t *comp, point_t *pt);

// Debug
void netcomp_print(netcomp_t *comp, char* pref);

#endif
