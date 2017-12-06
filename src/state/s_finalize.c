#include "state.h"

#include <kilolib.h>

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(FINALIZE)(state_t *st)
{
}

/*! led_finalize
 *
 * Led function for current state
 */
void
LED_NAME(FINALIZE)(state_t *st)
{
}

/*! state_finalize
 *
 * Handler function for S_FINALIZE main loop functionality
 */
void
LOOP_NAME(FINALIZE)(state_t *st)
{
}

/*! state_finalize_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_FINALIZE
 */
void
MSG_RX_NAME(FINALIZE)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}

