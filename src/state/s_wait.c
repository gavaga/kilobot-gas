#include "state.h"

#include <kilolib.h>

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(WAIT)(state_t *st)
{
}

/*! led_wait
 *
 * Led function for current state
 */
void
LED_NAME(WAIT)(state_t *st)
{
}

/*! state_wait
 *
 * Handler function for S_WAIT main loop functionality
 */
void
LOOP_NAME(WAIT)(state_t *st)
{
}

/*! state_wait_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_WAIT
 */
void
MSG_RX_NAME(WAIT)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}
