#include "state.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "constants.h"
#include "msg.h"
#include "list.h"

#include "err.h"
#include "nbi.h"
#include "nbr.h"
#include "fifo.h"

#include <kilolib.h>

// States (possible values for s)
DEFINE_STATE(ERR,      0x0);
DEFINE_STATE(IDLE,     0x1);
DEFINE_STATE(WAIT,     0x2);
DEFINE_STATE(G_OHN,    0x3);
DEFINE_STATE(LCV,      0x4);
DEFINE_STATE(ELECT,    0x5);
DEFINE_STATE(TWIDDLE,  0x6);
DEFINE_STATE(COALESCE, 0x7);
DEFINE_STATE(FINALIZE, 0x8);
DEFINE_STATE(SEARCH,   0x9);
DEFINE_STATE(AVOID,    0x10);
DEFINE_STATE(DONE,     0xff);

// Roles
DEFINE_ROLE(NONE,   0x0);
DEFINE_ROLE(BORDER, 0x1);
DEFINE_ROLE(SEED,   0x2);

//
// Static objects to use to use when we don't want to use malloc
//
#ifdef STATIC_ALLOC
//
// message static objects
//
message_t _st_msg;

//
// msg_q static objects
//
fifo_t _st_msg_q;
list_t _st_msg_q_l;
list_node_t _st_msg_q_raw_list[STATIC_SIZE_MSG_Q_RAW_LIST];

//
// Static allocation for msg data
//
msg_data_t _st_md_list[STATIC_SIZE_MD_LIST];

//
// nbi static objects
//
nbrs_info_t _st_nbi;
nbr_t _st_nbi_nbrs[STATIC_SIZE_NBI_NBRS];

matf_t _st_nbi_pd;
float _st_nbi_pd_data[STATIC_SIZE_NBI_PD_DATA];

bitmat_t _st_nbi_adj;
uint8_t _st_nbi_adj_data[STATIC_SIZE_NBI_ADJ_DATA];
#endif

/*! state_init
 *
 * Initialize the global state object to be used
 * throughout the program.
 */
void
state_init(state_t *st)
{
    // Set default state to IDLE
    STATE_SET(st, IDLE);

#ifdef STATIC_ALLOC
    st->msg = &_st_msg;
    // initialize message
    message_init(st->msg);

    st->msg_q = &_st_msg_q;
    // init the message queue
    fifo_init(st->msg_q, STATIC_SIZE_MSG_Q_RAW_LIST,
            &_st_msg_q_l, _st_msg_q_raw_list, STATIC_SIZE_MSG_Q_RAW_LIST);

    // message data list
    st->md_list_sz = STATIC_SIZE_MD_LIST;
    st->md_list = _st_md_list;

    st->nbi = &_st_nbi;
    // init the neighbor info object
    nbi_init(st->nbi, MAX_NEIGHBORS, POL_EVICT_OLDEST,
            _st_nbi_nbrs, STATIC_SIZE_NBI_NBRS,
            &_st_nbi_pd, _st_nbi_pd_data, STATIC_SIZE_NBI_PD_DATA,
            &_st_nbi_adj,  _st_nbi_adj_data, STATIC_SIZE_NBI_ADJ_DATA);
#else
    // dynamic allocation
#endif
}

/*! state_delete
 *
 * Clean up everything that was initialized in init
 */
void
state_delete(state_t *st)
{
    fifo_clean(st->msg_q);
    nbi_clean(st->nbi);
}

/*! state_reset
 *
 * Reset the state--clears everything, cleans it up, reinitializes.
 */
void
state_reset(state_t *st)
{
    state_delete(st);
    state_init(st);
}

/*! state_set_role
 *
 * Set the role to the provided one. If it's different from
 * the old one, mark the role new
 */
void
state_set_role(
    state_t *st,
    uint8_t r)
{
    if (r != st->r) {
        state_set_flag(st, SF_NEW_ROLE);
        st->r = r;
    }
}

/*! state_flag_is_set
 *
 * Check if the flag is set
 */
bool
state_flag_is_set(
    state_t *st,
    uint8_t flag)
{
    ASSERT_OR_ERR(st, err, KB_ERR_INPUT);

    return st->flags & flag;

err:
    return false;
}

/*! state_set_flag
 *
 * Set the flag
 */
void
state_set_flag(
    state_t *st,
    uint8_t flag)
{
    ASSERT_OR_ERR(st, err, KB_ERR_INPUT);
    st->flags |= flag;

err:
    return;
}

/*! state_clr_flag
 *
 * Clear the flag
 */
void
state_clr_flag(
    state_t *st,
    uint8_t flag)
{
    ASSERT_OR_ERR(st, err, KB_ERR_INPUT);

    st->flags &= ~flag;

err:
    return;
}

/*! state_get_next_msg
 *
 * Update the message with data from the queue and free the queued
 * data
 */
message_t*
state_get_next_msg(state_t *st)
{
    ASSERT_OR_ERR(st, err, KB_ERR_INPUT);

    // pop the data off the queue
    msg_data_t *msg_data = (msg_data_t*)fifo_pop(st->msg_q);

    //
    // if there was no message on the queue, either send nothing, or
    // repeat the last message
    //
    if (!msg_data) {
        if (st->flags & SF_REPEAT_LAST) {
            return st->msg;
        } else {
            return NULL;
        }
    }

    // if there was, update the data in the message object
    else {
        message_update_data(st->msg, msg_data);
        // free the data
        msg_data_delete(msg_data);
    }

    // return ptr to the message
    return st->msg;

err:
    return NULL;
}

/*! state_push_msg
 *
 * Push a new message onto the queue. Assumes that
 * data is allocated such that it will survive its time
 * on the queue without being corrupted.
 */
void
state_push_msg(
    state_t *st,
    msg_data_t *data)
{
    ASSERT_OR_ERR(st && data, err, KB_ERR_INPUT);

    // push it onto the queue
    fifo_push(st->msg_q, data);

err:
    return;
}

/*! state_set_motion
 *
 * Change robot motion
 */
void
state_set_motion(
    state_t *st,
    uint8_t a)
{
    ASSERT_OR_ERR(st, err, KB_ERR_INPUT);

    // only actually change things if we aren't already doing the
    // same thing
    if (a == st->a) {
        return;
    }

    st->a = a;
    switch (st->a) {
        case SA_FORWARD:
            // spinup both motors
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right);
            break;

        case SA_RIGHT:
            // only spinup right motor to go right
            set_motors(0, 255);
            set_motors(0, kilo_turn_right);
            break;

        case SA_LEFT:
            // only spinup left motor
            set_motors(255, 0);
            set_motors(kilo_turn_left, 0);
            break;

        case SA_FWD_RIGHT:
            // spinup both motors
            spinup_motors();
            set_motors(kilo_straight_left/2, kilo_straight_right);
            break;

        case SA_FWD_LEFT:
            // spinup both motors
            spinup_motors();
            set_motors(kilo_straight_left, kilo_straight_right/2);
            break;

        case SA_STOP:
        default:
            set_motors(0, 0);
            break;
    }

err:
    return;
}

/*! led_NONE
 */
void
LED_NAME(NONE)(state_t *st)
{
    LED_OFF;
}

/*! led_BORDER
 */
void
LED_NAME(BORDER)(state_t *st)
{
    LED_TURQOISE;
}

/*! led_SEED
 */
void
LED_NAME(SEED)(state_t *st)
{
    LED_ORANGE;
}

/*! state_getstr
 *
 * Get debug string for state type
 */
char *
state_getstr(uint8_t s)
{
    if (s == STATE_NAME(ERR)) {
        return "err";
    } else if (s == STATE_NAME(IDLE)) {
        return "idle";
    } else if (s == STATE_NAME(WAIT)) {
        return "wait";
    } else if (s == STATE_NAME(G_OHN)) {
        return "g_ohn";
    } else if (s == STATE_NAME(LCV)) {
        return "lcv";
    } else if (s == STATE_NAME(ELECT)) {
        return "elect";
    } else if (s == STATE_NAME(TWIDDLE)) {
        return "twiddle";
    } else if (s == STATE_NAME(COALESCE)) {
        return "coalesce";
    } else if (s == STATE_NAME(FINALIZE)) {
        return "finalize";
    } else if (s == STATE_NAME(SEARCH)) {
        return "search";
    } else if (s == STATE_NAME(AVOID)) {
        return "avoid";
    } else if (s == STATE_NAME(DONE)) {
        return "done";
    } else {
        return "unknown";
    }
}

/*! role_getstr
 *
 * Get debug string for role type
 */
char *
role_getstr(uint8_t r)
{
    if (r == ROLE_NAME(NONE)) {
        return "none";
    } else if (r == ROLE_NAME(BORDER)) {
        return "border";
    } else if (r == ROLE_NAME(SEED)) {
        return "seed";
    } else {
        return "unknown";
    }
}

/*! action_getstr
 *
 * Get debug string for action type
 */
char *
action_getstr(uint8_t a)
{
    switch(a) {
        case SA_STOP:
            return "stop";
            
        case SA_FORWARD:
            return "forward";

        case SA_RIGHT:
            return "right";

        case SA_LEFT:
            return "left";

        case SA_FWD_RIGHT:
            return "forward right";

        case SA_FWD_LEFT:
            return "forward left";

        default:
            return "unknown";
    }
}

/*! state_print
 *
 * Print the entire current state
 */
void
state_print(state_t *st)
{
    // Global state information
    printf("state: 0x%x\n", kilo_uid);
    printf("\ts: %s\n", state_getstr(st->s));
    printf("\tr: %s\n", role_getstr(st->r));
    printf("\ta: %s\n", action_getstr(st->a));
    printf("\tflags: 0x%x\n", st->flags);
    printf("\tticks: %d\n", st->ticks);

    // Print sub objects
    message_print(st->msg, "\t");
    fifo_print(st->msg_q, "\t", msg_data_print);
    nbi_print(st->nbi, "\t");
}
