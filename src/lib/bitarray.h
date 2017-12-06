#ifndef BITARRAY_H
#define BITARRAY_H

// for uint32_t, uint8_t
#include <stdint.h>
#include <stdbool.h>

/*! bitarray_t
 *
 * Bit array type. Used for manipulating long arrays of flags
 */
typedef struct bitarray_t {
    /*! sz
     *
     * Size of the bit array
     */
    uint32_t sz;

    /*! data
     *
     * Array to hold the data
     */
    uint8_t *data;
} bitarray_t;

/*! bitmat_t
 *
 * Bit matrix type.
 */
typedef struct bitmat_t {
    /*! n,m
     * 
     * Dimensions of matrix
     */
    uint32_t n, m;

#define BM_DEFAULT   0x0
#define BM_SYMMETRIC 0x1

    /*! type
     *
     * Type of the bit array--affects access
     */
    uint8_t type;

    /*! rsvd
     *
     * Explicit Padding
     */
    uint8_t rsvd[3];

    /*! raw
     *
     * The raw bit array
     */
    bitarray_t raw;
} bitmat_t;

// Constructor/Destructor
bitarray_t* ba_create(uint32_t size);
void ba_init(bitarray_t *ba, uint32_t size,
             uint8_t *data, uint32_t data_sz);
void ba_clean(bitarray_t *ba);
void ba_delete(bitarray_t *ba);

// Accessor/Mutator
bool ba_is_set(bitarray_t *ba, uint32_t idx);
void ba_set(bitarray_t *ba, uint32_t idx);
void ba_clr(bitarray_t *ba, uint32_t idx);

// Debug
void ba_print(bitarray_t *ba, char *pref);

// Constructor/Destructor
bitmat_t *bm_create(uint32_t n, uint32_t m, uint8_t type);
void bm_init(bitmat_t *bm, uint32_t n, uint32_t m, uint8_t type,
             uint8_t *data, uint32_t data_sz);
void bm_clean(bitmat_t *bm);
void bm_delete(bitmat_t *bm);

// Accessor/Mutator
bool bm_is_set(bitmat_t *bm, uint32_t i, uint32_t j);
void bm_set(bitmat_t *bm, uint32_t i, uint32_t j);
void bm_clr(bitmat_t *bm, uint32_t i, uint32_t j);

// Operations
void bm_mult(bitmat_t *a, bitmat_t *b, bitmat_t *c);
void bm_copy(bitmat_t *dest, bitmat_t *src);
void bm_fill(bitmat_t *bm, uint8_t byte);

// Debug
void bm_print(bitmat_t *bm, char *pref);

#endif
