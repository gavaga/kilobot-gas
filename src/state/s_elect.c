#include "state.h"

#include <kilolib.h>

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(ELECT)(state_t *st)
{
}

/*! led_elect
 *
 * Led function for current state
 */
void
LED_NAME(ELECT)(state_t *st)
{
}

/*! state_elect
 *
 * Handler function for S_ELECT main loop functionality
 */
void
LOOP_NAME(ELECT)(state_t *st)
{
}

/*! state_elect_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_ELECT
 */
void
MSG_RX_NAME(ELECT)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}
