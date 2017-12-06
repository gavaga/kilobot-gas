#include "state.h"

#include <kilolib.h>

#include "localize.h"
#include "msg.h"
#include "led.h"
#include "fifo.h"
#include "nbi.h"

#include "debug.h"

/*! STATIC_INTERVAL
 *
 * Amount of time in ticks we have to go without change to the
 * neighborhood to consider it completely understood
 */
#define STATIC_INTERVAL     128

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(G_OHN)(state_t *st)
{
}

/*! led_g_ohn
 *
 * Led function for current state. Blinks when we send a message.
 */
void
LED_NAME(G_OHN)(state_t *st)
{
    if (state_flag_is_set(st, SF_MSG_SENT)) {
        LED_OFF;
        delay(100);
        LED_VIOLET;
    } else {
        LED_VIOLET;
    }
}

/*! state_g_ohn
 *
 * Handler function for S_G_OHN main loop functionality
 *
 * 1. Try to compute
 */
void
LOOP_NAME(G_OHN)(state_t *st)
{
    // Add an ID message to the queue if it's empty
    if (fifo_is_empty(st->msg_q)) {
        msg_data_id_t *msg = (msg_data_id_t*)msg_data_create(st, MSG_NAME(ID));
        msg->sender_id = kilo_uid;
        state_push_msg(st, (msg_data_t*)msg);
    }

    // Compute local coordinate system
    localize_all(st);

    // if there's enough info to localize
    // and we haven't gotten any new neighbors
    // in enough time, transition to the LCV state
    if (nbi_is_localized(st->nbi)
            && st->ticks - st->nbi->last_new_ticks > STATIC_INTERVAL)
    {
        STATE_SET(st, LCV);
    }
}

/*! state_g_ohn_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_G_OHN
 */
void
MSG_RX_NAME(G_OHN)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
    if (msg->type == MSG_NAME(OHN)) {
        MSG_DEFAULT_RX_NAME(OHN)(st, msg, dist);
    } else if (msg->type == MSG_NAME(ID)) {
        MSG_DEFAULT_RX_NAME(ID)(st, msg, dist);
    } else {
        MSG_DEFAULT_RX_NAME(NONE)(st, msg, dist);
    }
}
