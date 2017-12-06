#include "state.h"

#include <stdio.h>
#include <kilolib.h>

#include "led.h"

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(IDLE)(state_t *st)
{
}

/*! led_idle
 *
 * Led function for current state
 */
void
LED_NAME(IDLE)(state_t *st)
{
    LED_WHITE;
}

/*! state_idle
 *
 * Handler function for S_IDLE main loop functionality
 */
void
LOOP_NAME(IDLE)(state_t *st)
{
    STATE_SET(st, G_OHN);
}

/*! state_idle_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_IDLE
 */
void
MSG_RX_NAME(IDLE)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}
