#include "list.h"

// malloc
#include <stdlib.h>
//memset
#include <string.h>

#include "err.h"

/*---------- Static Helper Functions -----------*/

/*! _list_find_unused
 *
 * Find an unused node in the raw list (assuming list was
 * cleared at some point
 */
static list_node_t*
_list_find_unused(list_t *l)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    for (uint32_t i = 0; i < l->capacity; ++i) {
        if (l->raw_list[i].data == NULL) {
            return &l->raw_list[i];
        }
    }

err:
    return NULL;
}

/*! _list_node_clear
 *
 * Clear a list node to be used as empty later
 */
static void
_list_node_clear(list_node_t *n)
{
    ASSERT_OR_ERR(n, err, KB_ERR_INPUT);

    memset(n, 0, sizeof(list_node_t));

err:
    return;
}


/*-------------- Library Functions --------------*/

/*! list_create
 *
 * Constructor
 */
list_t *
list_create(uint32_t capacity)
{
    return NULL;
    /*ASSERT_OR_ERR(raw && capacity >= 0, err, KB_ERR_INPUT);*/

    /*list_t *l = (list_t*)malloc(sizeof(list_t));*/
    /*ASSERT_OR_ERR(l, err, KB_ERR_OOM);*/

    /*list_init(l, capacity, raw);*/

/*err:*/
    /*return NULL;*/
}

/*! list_init
 *
 * In-place constructor.
 */
void 
list_init(
    list_t *l,
    uint32_t capacity,
    list_node_t *data,
    uint32_t data_sz)
{
    ASSERT_OR_ERR(l && capacity >= 0 && data && data_sz == capacity,
            err, KB_ERR_INPUT);

    l->size = 0;
    l->flags = 0;
    l->capacity = capacity;
    l->raw_list = data;
    l->head = NULL;
    l->tail = NULL;
err:
    return;
}

/*! list_clean
 *
 * In-place destructor
 */
void
list_clean(list_t *l)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    list_node_t *node, *next;
    while (l->head != NULL) {
        node = l->head;
        next = node->next;
        // remove the head of the list
        list_remove(l, node);
        node = next;
    }

    // if the list was dynamically allocated, free the
    // raw list
    if (l->flags & LF_DALLOC) {
        free(l->raw_list);
    }

err: 
    return;
}

/*! list_append
 *
 * Append an item to the end of the list
 */
void
list_append(
    list_t *l,
    void *data)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    list_node_t *new_tail = _list_find_unused(l);
    ASSERT_OR_ERR(new_tail, err, KB_ERR_FULL);

    new_tail->data = data;
    new_tail->next = NULL;
    new_tail->prev = l->tail;
    if (l->tail) {
        l->tail->next = new_tail;
    }

    if (l->head == NULL) {
        l->head = new_tail;
    }
    l->tail = new_tail;

    l->size++;

err:
    return;
}

/*! list_push
 *
 * Push an item to the front of the list
 */
void
list_push(
    list_t *l,
    void *data)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    list_node_t *new_head = _list_find_unused(l);
    ASSERT_OR_ERR(new_head, err, KB_ERR_FULL);

    new_head->data = data;
    new_head->next = l->head;
    new_head->prev = NULL;
    if (l->head) {
        l->head->prev = new_head;
    }

    l->head = new_head;
    if (l->tail == NULL) {
        l->tail = new_head;
    }

    l->size++;

err:
    return;
}

/*! list_pop_head
 *
 * Pop from the head of the list
 */
void *
list_pop_head(list_t *l)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    // short circuit on empty list
    if (l->head == NULL) {
        return NULL;
    }

    list_node_t *tmp = l->head;
    if (tmp) {
        list_remove(l, l->head);
        void *data = tmp->data;
        _list_node_clear(tmp);
        return data;
    }
err:
    return NULL;
}

/*! list_pop_tail
 *
 * Pop from the tail of the list
 */
void *
list_pop_tail(list_t *l)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    list_node_t *tmp = l->tail;
    if (tmp) {
        list_remove(l, l->tail);
        void *data = tmp->data;
        _list_node_clear(tmp);
        return data;
    }
err:
    return NULL;
}

/*! list_find
 *
 * Check if the provided data is present in the list
 */
bool
list_find(
    list_t *l,
    void *data)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    for (list_node_t *n = l->head; n != NULL; n = n->next) {
        if (n->data == data) {
            return true;
        }
    }
    return false;

err:
    return false;
}

/*! list_is_empty
 *
 * Check if the list is empty
 */
bool
list_is_empty(list_t *l)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    // list is empty if head is null
    return l->size == 0;
err:
    return true;
}

/*! list_remove
 *
 * Remove the node from the list
 */
void
list_remove(
    list_t *l,
    list_node_t *node)
{
    ASSERT_OR_ERR(l && node, err, KB_ERR_INPUT);

    if (node == l->head) {
        l->head = node->next;
    }
    if (node == l->tail) {
        l->tail = node->prev;
    }

    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }

    node->next = NULL;
    node->prev = NULL;

    // decrement size
    --l->size;

err:
    return;
}

/*! list_remove_data
 *
 * Remove the node with the given data from the list
 */
void
list_remove_data(
    list_t *l,
    void *data)
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    for (list_node_t *n = l->head; n != NULL; n = n->next) {
        if (n->data == data) {
            list_remove(l, n);
            _list_node_clear(n);
            break;
        }
    }

err:
    return;
}

/*! list_print
 *
 * Print the list
 */
void
list_print(
        list_t *l,
        char *pref,
        void (*p_elt)(void*, char*))
{
    ASSERT_OR_ERR(l, err, KB_ERR_INPUT);

    char next_pref[16];
    strcpy(next_pref, pref);
    strcat(next_pref, "\t\t");

    printf("%slist: %p\n", pref, l);
    printf("%s\tsize: %d\n", pref, l->size);
    printf("%s\tcapacity: %d\n", pref, l->capacity);
    printf("%s\tflags: %x\n", pref, l->flags);
    printf("%s\traw_list: %p\n", pref, l->raw_list);
    printf("%s\thead: %p\n", pref, l->head);
    printf("%s\ttail: %p\n", pref, l->tail);
    printf("%s\telements:\n", pref);

    for (list_node_t *node = l->head; node; node = node->next) {
        if (p_elt) {
            p_elt(node->data, next_pref);
        } else {
            printf("%s\t\tdata: %p\n", pref, node->data);
        }
    }
err:
    return;
}
