#include "state.h"

#include <kilolib.h>

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(COALESCE)(state_t *st)
{
}

/*! led_coalesce
 *
 * Led function for current state
 */
void
LED_NAME(COALESCE)(state_t *st)
{
}

/*! state_coalesce
 *
 * Handler function for S_COALESCE main loop functionality
 */
void
LOOP_NAME(COALESCE)(state_t *st)
{
}

/*! state_coalesce_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_COALESCE
 */
void
MSG_RX_NAME(COALESCE)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}

