#include "libCAN.h"


CAN_TX_STATE CAN_send(const CAN_message* msg)
{
    return;
}

CAN_RX_STATE CAN_receive(CAN_message* msg)
{
    return;
}

static inline can_std_id_t CAN_make_ID(boot_msg_class_t msg_class, priority_t priority, node_ID_t addr)
{
    // Sanity checks
    assert(priority <= 7);
    assert(msg_class <= 0xF);
    assert(addr <= 0xF);

    return ((priority << CAN_PRIORITY_SHIFT) & CAN_PRIORITY_MASK) |
           ((msg_class << CAN_MSG_CLASS_SHIFT) & CAN_MSG_CLASS_MASK) |
           ((addr << CAN_NODE_ID_SHIFT) & CAN_NODE_ID_MASK);
}