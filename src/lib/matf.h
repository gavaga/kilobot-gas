#ifndef MATF_H
#define MATF_H

#include "types.h"

/*! mat_t
 *
 * flat float matrix structure
 */
typedef struct matf_t {
    /*! n,m
     *
     * Matrix dimensions
     */
    uint32_t n;
    uint32_t m;

#define MATF_DEFAULT     0x0
#define MATF_SYMMETRIC   0x1
#define MATF_ANTISYMM    0x2

    /*! flags
     *
     * Matrix type, for various optimizations
     */
    uint8_t type;

    /*! rsvd
     *
     * Explicit padding
     */
    uint8_t rsvd[3];

    /*! data
     *
     * Raw data array
     */
    float *data;
} matf_t;

// Constructors/Destructor
matf_t *matf_create(uint32_t n, uint32_t m, uint8_t type);
void matf_init(matf_t *mat, uint32_t n, uint32_t m, uint8_t type,
               float *data, uint32_t data_sz);
void matf_delete(matf_t *mat);

// Accessor/Mutator
float matf_get(matf_t *m, uint32_t i, uint32_t j);
void matf_set(matf_t *m, uint32_t i, uint32_t j, float val);
void matf_clr(matf_t *m);
void matf_fill(matf_t *m, uint8_t byte);

// Debug
void matf_print(matf_t *m, char *pref);

#endif
