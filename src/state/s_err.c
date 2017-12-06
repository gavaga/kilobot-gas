#include "state.h"

#include <kilolib.h>

#include "led.h"

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(ERR)(state_t *st)
{
}

/*! led_err
 *
 * Led function for current state
 */
void
LED_NAME(ERR)(state_t *st)
{
    LED_RED;
}

/*! state_err
 *
 * Handler function for S_ERR main loop functionality
 */
void
LOOP_NAME(ERR)(state_t *st)
{
}

/*! state_err_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_ERR
 */
void
MSG_RX_NAME(ERR)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}
