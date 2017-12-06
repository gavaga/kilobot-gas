#include "msg.h"

#include <kilolib.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "err.h"
#include "nbi.h"
#include "state.h"

DEFINE_MSG(NONE, 0x0);
DEFINE_MSG(ID,   0x1);
DEFINE_MSG(OHN,  0x2);

/*! message_init
 *
 * initialize the message
 */
void
message_init(message_t *m)
{
    ASSERT_OR_ERR(m, err, KB_ERR_INPUT);
    m->type = NORMAL;
    msg_data_init((msg_data_t*)m->data, MSG_NAME(NONE));
    m->crc = message_crc(m);
err:
    return;
}

/*! message_update_data
 *
 * Update message data and crc
 */
void
message_update_data(
    message_t *m,
    msg_data_t *data)
{
    ASSERT_OR_ERR(m && data, err, KB_ERR_INPUT);
    memcpy(m->data, data, sizeof(msg_data_t));
    m->crc = message_crc(m);
err:
    return;
}

/*! msg_data_create
 *
 * Create a new message data
 */
msg_data_t *
msg_data_create(state_t *st, uint8_t type)
{
    ASSERT_OR_ERR(st, err, KB_ERR_INPUT);

    // allocate space for the data
    msg_data_t *msg = NULL;
    if (st->md_list) {
        // find a free element of the list
        for (uint32_t i = 0; i < st->md_list_sz; ++i) {
            if (st->md_list[i].type == MSG_NAME(NONE)) {
                msg = &st->md_list[i];
                break;
            }
        }
    } else {
        msg = (msg_data_t*)malloc(sizeof(msg_data_t));
    }
    ASSERT_OR_ERR(msg, err, KB_ERR_OOM);

    msg_data_init(msg, type);
    return msg;
err:
    return NULL;
}

/*! msg_data_init
 *
 * In-place constructor for basic message data
 */
void
msg_data_init(msg_data_t *md, uint8_t type)
{
    ASSERT_OR_ERR(md, err, KB_ERR_INPUT);
    // set it all to 0
    memset(md, 0, sizeof(msg_data_t));
    // write the type
    md->type = type;
err:
    return;
}

/*! msg_data_delete
 *
 * Free the message data
 */
void
msg_data_delete(msg_data_t *msg)
{
    ASSERT_OR_ERR(msg, err, KB_ERR_INPUT);

    memset(msg, 0, sizeof(msg_data_t));
err:
    return;
}

/*! msg_data_getstr
 *
 * Get message data string
 */
char*
msg_data_getstr(uint8_t type)
{
    if (type == MSG_NAME(NONE)) {
        return "none";
    } else if (type == MSG_NAME(ID)) {
        return "id";
    } else if (type == MSG_NAME(OHN)) {
        return "ohn";
    } else {
        return "unknown";
    }
}


/*! msg_data_print
 *
 * Print the message data
 */
void
msg_data_print(void *msg, char *pref)
{
    ASSERT_OR_ERR(msg, err, KB_ERR_INPUT);

    msg_data_t *m = (msg_data_t*)msg;

    printf("%smsg_data: %p\n", pref, m);
    printf("%s\ttype: %d(%s)\n", pref, m->type, msg_data_getstr(m->type));

    if (m->type == MSG_NAME(NONE)) {
        // nothing else to print
    } else if (m->type == MSG_NAME(ID)) {
        msg_data_id_t *m_id = (msg_data_id_t*)m;
        printf("%s\tsender: %x\n", pref, m_id->sender_id);
    } else if (m->type == MSG_NAME(OHN)) {
        msg_data_ohn_t *m_ohn = (msg_data_ohn_t*)m;
        printf("%s\tsender: %x\n", pref, m_ohn->sender_id);
        printf("%s\tohn_id: %x\n", pref, m_ohn->ohn_id);
        printf("%s\tdist: %d\n", pref, m_ohn->ohn_dist);
    } else {
        // nothing else to print
    }

err:
    return;
}


/*! message_print
 *
 * debug print function for a raw kilobot message
 */
void
message_print(
    message_t *m,
    char *pref)
{
    ASSERT_OR_ERR(m && pref, err, KB_ERR_INPUT);

    char next_pref[16];
    strcpy(next_pref, pref);
    strcat(next_pref, "\t");

    printf("%smessage: %p\n", pref, m);
    printf("%s\ttype: %d\n", pref, m->type);
    printf("%s\tcrc: %x\n", pref, m->crc);
    msg_data_print(m->data, next_pref);

err:
    return;
}

/*! msg_rx_handler_NONE
 *
 * Default handle a NONE neighbor message
 */
void
MSG_DEFAULT_RX_NAME(NONE)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
}

/*! msg_rx_handler_id
 *
 * Handle an ID neighbor message, using it to update
 * our neighbor info arrays and start gossiping
 */
void
MSG_DEFAULT_RX_NAME(ID)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
    ASSERT_OR_ERR(st && msg, err, KB_ERR_INPUT);
    ASSERT_OR_ERR(msg->type == MSG_NAME(ID), err, KB_ERR_INPUT);

    msg_data_id_t *m_id = (msg_data_id_t*)msg;

    // update our arrays
    uint32_t nbr_idx = nbi_update_id(st->nbi, m_id->sender_id, st->ticks);
    nbi_set_dist(st->nbi, nbr_idx, nbr_idx, dist);

    // gossip result if it was new information
    if (st->nbi->last_new_ticks == st->ticks) {
        msg_data_ohn_t *m = (msg_data_ohn_t*)msg_data_create(st, MSG_NAME(OHN));
        m->sender_id = kilo_uid;
        m->ohn_id = m_id->sender_id;
        m->ohn_dist = dist;
        state_push_msg(st, (msg_data_t*)m);
    }
err:
    return;
}

/*! msg_rx_handler_ohn
 *
 * Handle a one-hop neighbor message, using it to update
 * our neighbor info arrays
 */
void
MSG_DEFAULT_RX_NAME(OHN)(
    state_t *st,
    msg_data_t *msg,
    kb_dist_t dist)
{
    ASSERT_OR_ERR(st && msg, err, KB_ERR_INPUT);
    ASSERT_OR_ERR(msg->type == MSG_NAME(OHN), err, KB_ERR_INPUT);

    msg_data_ohn_t *m_ohn = (msg_data_ohn_t*)msg;

    // update our arrays
    uint32_t nbr_idx = nbi_update_id(st->nbi, m_ohn->sender_id, st->ticks);
    uint32_t ohn_idx = nbi_get_nbr_idx(st->nbi, m_ohn->ohn_id);
    nbi_set_dist(st->nbi, nbr_idx, nbr_idx, dist);
    if (ohn_idx != INVALID_INDEX) {
        if (!nbi_is_adj(st->nbi, nbr_idx, ohn_idx)) {
            nbi_set_adj(st->nbi, nbr_idx, ohn_idx);
            st->nbi->last_new_ticks = st->ticks;
        }
        nbi_set_dist(st->nbi, nbr_idx, ohn_idx, m_ohn->ohn_dist);
    }

    // gossip result if we got new information
    if (st->nbi->last_new_ticks == st->ticks)
    {
        msg_data_ohn_t *m = (msg_data_ohn_t*)msg_data_create(st, MSG_NAME(OHN));
        m->sender_id = kilo_uid;
        m->ohn_id = m_ohn->sender_id;
        m->ohn_dist = dist;
        state_push_msg(st, (msg_data_t*)m);
    }
err:
    return;
}
