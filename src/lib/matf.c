#include "matf.h"

// malloc
#include <stdlib.h>
// memset
#include <string.h>

#include "constants.h"
#include "err.h"

static uint32_t
_matf_get_size(n, m, type)
{
    switch (type) {
        case MATF_SYMMETRIC:
        case MATF_ANTISYMM:
            return n*(n+1) / 2;
        case MATF_DEFAULT:
        default:
            return n*m;
    }
}

static uint32_t
_matf_get_idx(matf_t *m, uint32_t i, uint32_t j)
{
    ASSERT_OR_ERR(m, err, KB_ERR_INPUT);

    uint32_t sm = (i > j? j : i);
    uint32_t lg = (i > j? i : j);

    switch (m->type) {
        case MATF_SYMMETRIC:
        case MATF_ANTISYMM:
            return m->n*(m->n - 1) / 2 - (m->n - sm)*(m->n - sm - 1) / 2 + lg;
        case MATF_DEFAULT:
        default:
            return i*m->n + j;
    }
err:
    return INVALID_INDEX;
}

/*! matf_create
 *
 * Constructor
 */
matf_t *
matf_create(
    uint32_t n,
    uint32_t m,
    uint8_t type)
{
    return NULL;
    /*matf_t *mat = (matf_t*)malloc(sizeof(matf_t));*/
    /*matf_init(mat, n, m, type);*/
    /*return mat;*/
}

/*! matf_init
 *
 * In-place constructor
 */
void 
matf_init(
    matf_t *mat,
    uint32_t n,
    uint32_t m,
    uint8_t type,
    float *data,
    uint32_t data_sz)
{
    ASSERT_OR_ERR(mat && n > 0
            && data && data_sz == _matf_get_size(n, m, type),
            err, KB_ERR_INPUT);

    mat->n = n;
    mat->m = (m == 0? n : m);
    mat->type = type;
    mat->data = data;

err:
    return;
}

/*! matf_delete
 *
 * Destructor
 */
void
matf_delete(matf_t *mat)
{
    if (mat) {
        if (mat->data) {
            free(mat->data);
        }
        free(mat);
    }
}

/*! matf_get
 *
 * Accessor
 */
float
matf_get(
    matf_t *m,
    uint32_t i,
    uint32_t j)
{
    ASSERT_OR_ERR(m && i < m->n && j < m->m,
            err, KB_ERR_INPUT); 

    uint32_t idx = _matf_get_idx(m, i, j);
    return m->data[idx];
err:
    return 0.0;
}

/*! matf_set
 *
 * Mutator
 */
void
matf_set(
    matf_t *m,
    uint32_t i,
    uint32_t j,
    float val)
{
    ASSERT_OR_ERR(m && i < m->n && j < m->m,
            err, KB_ERR_INPUT); 

    uint32_t idx = _matf_get_idx(m, i, j);
    m->data[idx] = val;

err:
    return;
}

/*! matf_clr
 *
 * Clear the matrix by filling with 0
 */
void
matf_clr(matf_t *m)
{
    ASSERT_OR_ERR(m, err, KB_ERR_INPUT);

    matf_fill(m, 0x0);

err:
    return;
}

/*! mat_fill
 *
 * Fill with a single byte
 */
void
matf_fill(
    matf_t *m,
    uint8_t byte)
{
    ASSERT_OR_ERR(m, err, KB_ERR_INPUT);

    memset(m->data, byte, _matf_get_size(m->n, m->m, m->type));

err:
    return;
}

/*! matf_print
 *
 * Print the matrix
 */
void
matf_print(matf_t *m, char *pref)
{
    ASSERT_OR_ERR(m && pref, err, KB_ERR_INPUT);

    printf("%smatf: %p\n", pref, m);
    printf("%s\tdim: %dx%d\n", pref, m->n, m->m);
    printf("%s\ttype: ", pref);
    switch (m->type) {
        case MATF_DEFAULT:
            printf("default");
            break;

        case MATF_SYMMETRIC:
            printf("symmetric");
            break;
            
        case MATF_ANTISYMM:
            printf("antisymmetric");
            break;

        default:
            printf("unknown");
    }
    printf("\n");
    printf("%s\tdata: %p\n", pref, m->data);
    for (uint32_t i = 0; i < m->n; ++i) {
        printf("%s\t\t", pref);
        for (uint32_t j = 0; j < m->m; ++j) {
            printf("%-6.2f", matf_get(m, i, j));
        }
        printf("\n");
    }

err:
    return;
}
