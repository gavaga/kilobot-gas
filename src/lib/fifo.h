#ifndef FIFO_H
#define FIFO_H

#include "types.h"

// Forward declarations
typedef struct list_t list_t;
typedef struct list_node_t list_node_t;

/*! fifo_t
 *
 * Generic FIFO queue container
 */
typedef struct fifo_t {
    /*! data
     *
     * The list used to keep the queue
     */
    list_t *l;
} fifo_t;

// Constructors/Destructors
fifo_t *fifo_create(uint32_t cap);
void fifo_init(fifo_t *q, uint32_t cap,
               list_t *l, list_node_t *data, uint32_t data_sz);
void fifo_clean(fifo_t *q);
void fifo_delete(fifo_t *q);

// Accessors
bool fifo_is_empty(fifo_t *q);

// Manipulators
void fifo_push(fifo_t *q, void *data);
void *fifo_pop(fifo_t *q);

// Debug
void fifo_print(fifo_t *q, char *pref, void (*)(void*, char*));

#endif
