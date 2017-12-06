#include "nbr.h"

// malloc
#include <stdlib.h>
#include <kilolib.h>

#include "constants.h"
#include "err.h"

/*! nbr_create
 *
 * Constructor for nbr_t
 */
nbr_t *
nbr_create(kb_id_t id)
{
    nbr_t *n = (nbr_t*)malloc(sizeof(nbr_t));
    ASSERT_OR_ERR(n, err, KB_ERR_OOM);

    nbr_init(n, id);
    return n;
err:
    return NULL;
}

/*! nbr_init
 *
 * In-place constructor for nbr_t
 */
void
nbr_init(
    nbr_t *n,
    kb_id_t id)
{
    ASSERT_OR_ERR(n, err, KB_ERR_INPUT);

    n->id = id;
    n->last_time = kilo_ticks;
    n->flags = 0;
    n->loc.x = 0.0f;
    n->loc.y = 0.0f;
    n->comp = NULL;

err:
    return;
}

/*! nbr_clean
 *
 * in-place destructor for nbr_t
 */
void
nbr_clean(nbr_t *n)
{
    return;
}

/*! nbr_delete
 *
 * Destructor for nbr_t
 */
void
nbr_delete(nbr_t *n)
{
    nbr_clean(n);
    free(n);
}

/*! nbr_flag_is_set
 *
 * Check if the flag is set
 */
bool
nbr_flag_is_set(
    nbr_t *n,
    uint32_t flag)
{
    ASSERT_OR_ERR(n, err, KB_ERR_INPUT);

    return (n->flags & flag) != 0;
err:
    return false;
}

/*! nbr_set_flag
 *
 * Mark the flag set
 */
void
nbr_set_flag(
    nbr_t *n,
    uint32_t flag)
{
    ASSERT_OR_ERR(n, err, KB_ERR_INPUT);

    n->flags |= flag;
err:
    return;
}

/*! nbr_clr_flag
 *
 * Clear the flag
 */
void
nbr_clr_flag(
    nbr_t *n,
    uint32_t flag)
{
    ASSERT_OR_ERR(n, err, KB_ERR_INPUT);

    n->flags &= ~flag;
err:
    return;
}

/*! nbr_is_localized
 *
 * Check if neighbor has been localized
 */
bool
nbr_is_localized(nbr_t *n)
{
    return nbr_flag_is_set(n, NBR_LOCALIZED);
}

/*! nbr_set_localized
 *
 * Mark neighbor localized
 */
void
nbr_set_localized(nbr_t *n)
{
    nbr_set_flag(n, NBR_LOCALIZED);
}

/*! nbr_clr_localied
 *
 * Clear localized flag
 */
void
nbr_clr_localized(nbr_t *n)
{
    nbr_clr_flag(n, NBR_LOCALIZED);
}

/*! nbr_print
 *
 * Print the neighbor information
 */
void
nbr_print(nbr_t *n, char *pref)
{
    ASSERT_OR_ERR(n && pref, err, KB_ERR_INPUT);

    printf("%snbr: %p\n", pref, n);
    printf("%s\tid: %x\n", pref, n->id);
    printf("%s\tidx: %d\n", pref, n->idx);
    printf("%s\tlast_time: %d\n", pref, n->last_time);
    printf("%s\tflags: %x\n", pref, n->flags);
    printf("%s\thopct: %d\n", pref, n->hopct);
    printf("%s\tloc: (%0.2f, %0.2f)\n", pref, n->loc.x, n->loc.y);
    printf("%s\tlast_loc: (%0.2f, %0.2f)\n", pref, n->last_loc.x, n->last_loc.y);
    printf("%s\tcomponent: %p\n", pref, n->comp);

err:
    return;
}
