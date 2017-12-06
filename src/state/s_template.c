#include "state.h"

#include <kilolib.h>

#include "led.h"

/*! setup_STATE_NAME
 *
 * Setup function for current state
 */
void
SETUP_NAME(STATE_NAME)(state_t *st)
{
}

/*! led_STATE_NAME
 *
 * Led function for current state
 */
void
LED_NAME(STATE_NAME)(state_t *st)
{
}

/*! state_STATE_NAME
 *
 * Handler function for state STATE_NAME main loop functionality
 */
void
LOOP_NAME(STATE_NAME)(state_t *st)
{
}

/*! state_STATE_NAME_msg_rx_handler
 *
 * Message receipt handler function for state
 * STATE_NAME
 */
void
MSG_RX_NAME(STATE_NAME)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}
