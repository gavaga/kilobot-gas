#include "state.h"

#include <kilolib.h>

#include "led.h"
#include "fifo.h"
#include "localize.h"
#include "lcv.h"
#include "nbi.h"

/*! setup
 *
 * Setup function for current state
 */
void
SETUP_NAME(LCV)(state_t *st)
{
    /*state_print(st);*/
}

/*! led_lcv
 *
 * Led function for current state
 */
void
LED_NAME(LCV)(state_t *st)
{
    LED_BLUE;
}

/*! state_lcv
 *
 * Handler function for S_LCV main loop functionality
 */
void
LOOP_NAME(LCV)(state_t *st)
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
    if (nbi_is_localized(st->nbi))
    {
        if (nbi_check_lcv(st->nbi)) {
            STATE_SET_ROLE(st, BORDER);
        }
    }
}

/*! state_lcv_msg_rx_handler
 *
 * Message receipt handler function for state
 * S_LCV
 */
void
MSG_RX_NAME(LCV)(
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
