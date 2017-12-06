#include "bitarray.h"

// for malloc
#include <stdlib.h>
// for memset
#include <string.h>

#include "constants.h"
#include "err.h"

/*! ba_create
 *
 * Constructor for a bit array
 */
bitarray_t*
ba_create(
    uint32_t size)
{
    bitarray_t *ba = (bitarray_t*)malloc(sizeof(bitarray_t));
    ASSERT_OR_ERR(ba, err, KB_ERR_OOM);

    // size/8 rounded up
    uint32_t nchars = (size + sizeof(uint8_t) - 1) / sizeof(uint8_t);
    uint8_t *data = (uint8_t*)calloc(nchars, sizeof(uint8_t));

    ba_init(ba, size, data, nchars);
    return ba;
err:
    return NULL;
}

/*! ba_init
 *
 * In-place constructor for bit array
 */
void
ba_init(
    bitarray_t *ba,
    uint32_t size,
    uint8_t *data,
    uint32_t data_sz)
{
    uint32_t nchars = (size + sizeof(uint8_t) - 1) / sizeof(uint8_t);
    ASSERT_OR_ERR(ba && size > 0 && data && data_sz == nchars,
            err, KB_ERR_INPUT);

    ba->sz = size;
    ba->data = data;
    memset(ba->data, 0, data_sz);
err:
    return;
}

/*! ba_clean
 *
 * In-place destructor
 */
void
ba_clean(bitarray_t *ba)
{
    if (!ba) return;

    free(ba->data);
}

/*! ba_delete
 *
 * Destructor for a bit array.
 */
void
ba_delete(bitarray_t *ba)
{
    if (!ba) return;

    free(ba);
}

/*! ba_set
 *
 * Set a single bit in the array
 */
void
ba_set(
    bitarray_t *ba,
    uint32_t idx)
{
    ASSERT_OR_ERR(ba, err, KB_ERR_INPUT);

    uint32_t ch = idx / 8;
    uint32_t bit = idx % 8;

    ba->data[ch] |= (0x1 << bit);
err:
    return;
}

/*! ba_clr
 *
 * Clear a single bit in the array
 */
void
ba_clr(
    bitarray_t *ba,
    uint32_t idx)
{
    ASSERT_OR_ERR(ba, err, KB_ERR_INPUT);
    uint32_t ch = idx / 8;
    uint32_t bit = idx % 8;

    ba->data[ch] &= ~(0x1 << bit);
err:
    return;
}

/*! ba_is_set
 *
 * Check if a single bit in the array is set
 */
bool
ba_is_set(
    bitarray_t *ba,
    uint32_t idx)
{
    ASSERT_OR_ERR(ba, err, KB_ERR_INPUT);
    uint32_t ch = idx / 8;
    uint32_t bit = idx % 8;

    return ((ba->data[ch] & (0x1 << bit)) != 0);
err:
    return false;
}

/*! ba_print
 *
 * Print the bitarray
 */
void
ba_print(bitarray_t *ba, char *pref)
{
    ASSERT_OR_ERR(ba, err, KB_ERR_INPUT);

    printf("%sbitarray: %p\n", pref, ba);
    printf("%s\tsize: %d\n", pref, ba->sz);

    printf("%s\tdata:\n", pref);
    for (uint32_t i = 0; i < ba->sz; ++i) {
        if (i%32 == 0) {
            if (i > 0) {
                printf("\n");
            }
            printf("%s\t\t", pref);
        }
        printf("%s", (ba_is_set(ba, i)? "1": "0"));
    }
    printf("\n\n");

err:
    return;
}

/*! _bm_get_idx
 *
 * Static helper function to compute the raw index from an index pair
 */
static uint32_t
_bm_get_idx(
    bitmat_t *bm,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(bm, err, KB_ERR_INPUT);

    switch (bm->type) {
        case BM_SYMMETRIC:
        case BM_DEFAULT:
        default:
            return i*bm->n + j;
    }

err:
    return INVALID_INDEX;
}

/*! _bm_get_size
 *
 * Static helper function to compute the raw array size from
 * the matrix dimensions and type
 */
static uint32_t
_bm_get_size(uint32_t n, uint32_t m, uint8_t type)
{
    ASSERT_OR_ERR(n > 0, err, KB_ERR_INPUT);
    switch(type) {
        case BM_SYMMETRIC:
            /*return bm->n*(bm->n+1) / 2;*/
        case BM_DEFAULT:
        default:
            return n*(m == 0? n: m);
    }

err:
    return INVALID_SIZE;
}

/*! bm_create
 *
 * Constructor
 */
bitmat_t *
bm_create(
    uint32_t n,
    uint32_t m,
    uint8_t type)
{
    bitmat_t *bm = (bitmat_t*)malloc(sizeof(bitmat_t));
    ASSERT_OR_ERR(bm, err, KB_ERR_OOM);

    uint32_t sz = _bm_get_size(n, m, type);
    uint32_t nchars = (sz + sizeof(uint8_t) - 1) / sizeof(uint8_t);
    uint8_t *data = (uint8_t*)calloc(nchars, sizeof(uint8_t));
    ASSERT_OR_ERR(data, err, KB_ERR_OOM);

    bm_init(bm, n, m, type, data, nchars);
    return bm;

err:
    return NULL;
}

/*! bm_init
 *
 * In-place constructor
 */
void
bm_init(
    bitmat_t *bm,
    uint32_t n,
    uint32_t m,
    uint8_t type,
    uint8_t *data,
    uint32_t data_sz)
{
    uint32_t sz = _bm_get_size(n, m, type);
    uint32_t nchars = (sz + sizeof(uint8_t) - 1) / sizeof(uint8_t);
    ASSERT_OR_ERR(bm && n > 0 && data && data_sz == nchars,
            err, KB_ERR_INPUT);

    bm->n = n;
    bm->m = (m == 0? n : m);
    bm->type = type;

    ba_init(&bm->raw, sz, data, data_sz);
err:
    return;
}

/*! bm_clean
 *
 * In-place destructor
 */
void
bm_clean(bitmat_t *bm)
{
    if (!bm) return;
    ba_clean(&bm->raw);
}

/*! bm_delete
 *
 * Destructor
 */
void
bm_delete(bitmat_t *bm)
{
    if (!bm) return;
    bm_clean(bm);
    free(bm);
}

/*! bm_is_set
 *
 * Check if bit is set
 */
bool
bm_is_set(
    bitmat_t *bm,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(bm, err, KB_ERR_INPUT);
    uint32_t idx = _bm_get_idx(bm, i, j);
    return ba_is_set(&bm->raw, idx);
err:
    return false;
}

/*! bm_set
 *
 * Set a bit
 */
void
bm_set(
    bitmat_t *bm,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(bm, err, KB_ERR_INPUT);
    uint32_t idx = _bm_get_idx(bm, i, j);
    ba_set(&bm->raw, idx);

    if (bm->type == BM_SYMMETRIC) {
        idx = _bm_get_idx(bm, j, i);
        ba_set(&bm->raw, idx);
    }
err:
    return;
}

/*! bm_clr
 *
 * clear a bit
 */
void
bm_clr(
    bitmat_t *bm,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(bm, err, KB_ERR_INPUT);
    uint32_t idx = _bm_get_idx(bm, i, j);
    ba_clr(&bm->raw, idx);

    if (bm->type == BM_SYMMETRIC) {
        idx = _bm_get_idx(bm, j, i);
        ba_clr(&bm->raw, idx);
    }
err:
    return;
}

/*! bm_mult
 *
 * Multiply two bit matrices
 *
 * Note: currently only implemented for symmetric matrices, also
 * requires dimension be a multiple of 8 so we can work in bytes
 */
void
bm_mult(
    bitmat_t *a,
    bitmat_t *b,
    bitmat_t *c)
{
    ASSERT_OR_ERR(a && b, err, KB_ERR_INPUT);
    ASSERT_OR_ERR(a->type == BM_SYMMETRIC, err, KB_ERR_INPUT);
    ASSERT_OR_ERR(b->type == BM_SYMMETRIC, err, KB_ERR_INPUT);
    ASSERT_OR_ERR(a->n % 8 == 0 && a->n / 8 >= 1, err, KB_ERR_INPUT);
    ASSERT_OR_ERR(a->n == b->n && a->n == c->n, err, KB_ERR_INPUT);

    uint32_t ch_per_row = (c->n / 8);
    // compute each byte one by one
    for (uint32_t ch = 0; ch < a->raw.sz / 8; ++ ch) {
        // go through each bit of the row and XOR with
        // the corresponding char from B only if it's set
        //
        // This represents summing
        //
        // For different kind of multiplication, can use
        // OR instead of XOR

        // compute the start of the row for the given byte
        uint32_t row_start = (ch/ch_per_row)*c->n;
        for (uint32_t j = 0; j < a->n; ++j) {
            if (ba_is_set(&a->raw, j + row_start)) {
                c->raw.data[ch] |= b->raw.data[ch%ch_per_row + j*ch_per_row];
            }
        }
    }

err:
    return;
}

/*! bm_copy
 *
 * Copy one matrix into another.
 */
void
bm_copy(
    bitmat_t *dest,
    bitmat_t *src)
{
    ASSERT_OR_ERR(dest && src
            && dest->n == src->n && dest->m == src->m
            && dest->type == src->type,
            err, KB_ERR_INPUT);

    uint32_t nchars = (src->raw.sz + sizeof(uint8_t) - 1) / sizeof(uint8_t);
    memcpy(dest->raw.data, src->raw.data, nchars);

err:
    return;
}

/*! bm_fill
 *
 * Fill the raw array with a single byte--useful for clearing
 * a reused matrix
 */
void
bm_fill(
    bitmat_t *bm,
    uint8_t byte)
{
    ASSERT_OR_ERR(bm, err, KB_ERR_INPUT);

    uint32_t nchars = (bm->raw.sz + sizeof(uint8_t) - 1) / sizeof(uint8_t);
    memset(bm->raw.data, byte, nchars);

err:
    return;
}

/*! bm_print
 *
 * Print the bit matrix
 */
void
bm_print(bitmat_t *bm, char *pref)
{
    ASSERT_OR_ERR(bm, err, KB_ERR_INPUT);

    printf("%sbitmat: %p\n", pref, bm);
    printf("%s\tdim: %dx%d\n", pref, bm->n, bm->m);

    printf("%s\ttype:", pref);
    switch (bm->type) {
        case BM_DEFAULT:
            printf("default");
            break;

        case BM_SYMMETRIC:
            printf("symmetric");
            break;
            
        default:
            printf("unknown");
    }
    printf("\n");

    printf("%s\tdata: %p\n", pref, bm->raw.data);
    for (uint32_t i = 0; i < bm->n; ++i) {
        printf("%s\t\t", pref);
        for (uint32_t j = 0; j < bm->m; ++j) {
            printf("%s", (bm_is_set(bm, i, j)? "1": "0"));
        }
        printf("\n");
    }
    printf("\n");

err:
    return;
}
