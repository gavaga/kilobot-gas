#include "fifo.h"

// for malloc
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "list.h"

/*! fifo_create
 *
 * Constructor.
 *
 * @return FIFO queue object
 */
fifo_t *
fifo_create(uint32_t cap)
{
    return NULL;
    /*fifo_t *q = (fifo_t*)malloc(sizeof(fifo_t));*/
    /*ASSERT_OR_ERR(q, err, KB_ERR_OOM);*/

    /*list_t *l = list_create(cap);*/

    /*fifo_init(q, cap, l, NULL, cap);*/
    /*return q;*/
/*err:*/
    /*return NULL;*/
}

/*! fifo_init
 *
 * In-place constructor.
 *
 * @param q Pointer to a queue to initialize
 */
void
fifo_init(
    fifo_t *q,
    uint32_t cap,
    list_t *l, list_node_t *data, uint32_t data_sz)
{
    ASSERT_OR_ERR(q && cap > 0 && l && data && data_sz == cap,
            err, KB_ERR_INPUT);

    q->l = l;
    list_init(q->l, cap, data, data_sz);
err:
    return;
}

/*! fifo_clean
 *
 * In-place destructor
 */
void
fifo_clean(fifo_t *q)
{
    ASSERT_OR_ERR(q, err, KB_ERR_INPUT);
    list_clean(q->l);
err:
    return;
}

/*! fifo_delete
 *
 * Destructor
 *
 * @param q Pointer to a queue to free
 */
void
fifo_delete(fifo_t *q)
{
    ASSERT_OR_ERR(q, err, KB_ERR_INPUT);
    fifo_clean(q);
    free(q);
err:
    return;
}

/*! fifo_is_empty
 *
 * Check if the queue is empty
 */
bool
fifo_is_empty(fifo_t *q)
{
    ASSERT_OR_ERR(q, err, KB_ERR_INPUT);
    return list_is_empty(q->l);
err:
    return true;
}

/*! fifo_push
 *
 * Push an element onto the queue
 */
void
fifo_push(
    fifo_t *q,
    void *data)
{
    ASSERT_OR_ERR(q, err, KB_ERR_INPUT);
    list_append(q->l, data);
err:
    return;
}

/*! fifo_pop
 *
 * Pop the next element from the queue
 */
void *
fifo_pop(fifo_t *q)
{
    ASSERT_OR_ERR(q, err, KB_ERR_INPUT);

    return list_pop_head(q->l);
err:
    return NULL;
}

/*! fifo_print
 *
 * Print the fifo queue
 */
void
fifo_print(
    fifo_t *q,
    char *pref,
    void (*p_elt)(void *, char*))
{
    ASSERT_OR_ERR(q, err, KB_ERR_INPUT);

    char next_pref[16];
    strcpy(next_pref, pref);
    strcat(next_pref, "\t");

    printf("%sfifo: %p\n", pref, q);
    list_print(q->l, next_pref, p_elt);

err:
    return;
}
