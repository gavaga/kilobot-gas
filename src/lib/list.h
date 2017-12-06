#ifndef LIST_H
#define LIST_H

#include "types.h"

/*! list_node_t
 *
 * List node object.
 */
typedef struct list_node_t {
    /*! data
     *
     * Generic data stored in the node
     */
    void *data;

    /*! next
     *
     * Pointer to next item in the list
     */
    struct list_node_t *next;

    /*! prev
     *
     * Pointer to previous item in the list
     */
    struct list_node_t *prev;
} list_node_t;

/*! list_t
 *
 * Generic linked list type
 */
typedef struct list_t {
    /*! size
     *
     * Number of elements in the list
     */
    uint32_t size;

    /*! capacity
     *
     * Maximum number of nodes in the list
     */
    uint32_t capacity;

#define LF_DALLOC  0x1

    /*! flags
     *
     * Some flags
     */
    uint8_t flags;

    /*! rsvd
     *
     * Explicit padding
     */
    uint8_t rsvd[3];

    /*! raw_list
     *
     * Raw list
     */
    list_node_t *raw_list;

    /*! head
     *
     * Head of the list
     */
    list_node_t *head;

    /*! tail
     *
     * Tail of the list
     */
    list_node_t *tail;
} list_t;

// Constructors/Destructors
list_t *list_create(uint32_t capacity);
void list_init(list_t *l, uint32_t capacity,
               list_node_t *data, uint32_t data_sz);
void list_clean(list_t *l);
void list_delete(list_t *l);

// Manipulators
void list_append(list_t *l, void *data);
void list_push(list_t *l, void *data);

void *list_pop_head(list_t *l);
void *list_pop_tail(list_t *l);

bool list_find(list_t *l, void *data);
bool list_is_empty(list_t *l);

void list_remove(list_t *l, list_node_t *node);
void list_remove_data(list_t *l, void *data);

// Debug
void list_print(list_t *l, char* pref, void (*p_elt)(void*, char*));

#endif
