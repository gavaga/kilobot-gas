#ifndef __STATIC_H__
#define __STATIC_H__

// Neighbor info array static allocations
extern nbr_t       _static_nbi_nbrs[];
extern float       _static_nbi_pd_data[];
extern float       _static_nbi_last_pd_data[];
extern uint8_t     _static_nbi_adj_data[];

// msg queue static objects
extern list_node_t _static_msg_q_raw_list[];

#endif //__STATIC_H__
