#include "state.h"

#include <kilolib.h>

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(SEARCH)(state_t *st)
{
}

/*! led_search
 *
 * Led function for current state
 */
void
LED_NAME(SEARCH)(state_t *st)
{
}

/*! state_search
 *
 * Handler function for S_SEARCH main loop functionality
 */
void
LOOP_NAME(SEARCH)(state_t *st)
{
}

/*! state_search_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_SEARCH
 */
void
MSG_RX_NAME(SEARCH)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}
