/**
 * This header defines primitives of communication meant to be used by both bootloader and application code, both on the jetson and on the MCUs.
 * Thus, it shall remain as much general as possibile, and shall not makke use of any hardware-specific type. These must be used in the respective .c files, which implement the hereby defined interface
 */

#include <stdint.h>
#include "stm32g4xx_hal.h" //should not be used here!!
#include "stm32g4xx_hal_fdcan.h" //should not be used here as well
#include <assert.h>

typedef enum 
{
    OK = 0x0U,
    FIFO_FULL = 0x01U,
    GENERIC_ERROR = 0x02U,
} CAN_TX_STATE;

typedef enum
{
    OK = 0x0U,
    NO_MESSAGE = 0x01U,
    GENERIC_ERROR = 0x02U
} CAN_RX_STATE;

//<----- CAN ARBITRATION FIELD SEMANTICS ----->
#define CAN_PRIORITY_MASK   0x700  // bits 10:8
#define CAN_PRIORITY_SHIFT  8
#define CAN_MSG_CLASS_MASK  0x0F0  // bits 7:4
#define CAN_MSG_CLASS_SHIFT 4
#define CAN_NODE_ID_MASK    0x00F  // bits 3:0
#define CAN_NODE_ID_SHIFT   0

#define CAN_PRIORITY(id)    (((id) & CAN_PRIORITY_MASK) >> CAN_PRIORITY_SHIFT)
#define CAN_MSG_CLASS(id)   (((id) & CAN_MSG_CLASS_MASK) >> CAN_MSG_CLASS_SHIFT)
#define CAN_NODE_ID(id)     ((id) & CAN_NODE_ID_MASK >> CAN_NODE_ID_SHIFT)

typedef enum uint8_t
{
    BOOT_FW_REQUEST = 0x0U,
    BOOT_FW_ASSIGN = 0x01U,
    BOOT_FW_DATA = 0x02U,
    BOOT_FW_END = 0x03U,
    BOOT_FW_ERROR = 0x04U
} boot_msg_class_t;
typedef uint16_t can_std_id_t;
typedef enum uint8_t {               // 3-bit priority (0-7)
    PRIORITY_LOW = 0,
    PRIORITY_MED,
    PRIORITY_HIGH,
    PRIORITY_CRITICAL
} priority_t;

typedef uint8_t node_ID_t;
#define NO_NODE_ID    0xF
#define BROADCAST_ID  0xE

typedef struct 
{
    can_std_id_t id;
    uint8_t len;
    uint8_t data[64];
} CAN_message;
//<----- END SECTION ----->

void CAN_init();
/**
 * @brief Queue a CAN(-FD) frame for transmission.
 *
 * Non-blocking primitive that queues the given message in the FDCAN TX FIFO.
 * If the function returns OK, the message is guaranteed to be queued and must
 * never be resent. The FDCAN controller handles arbitration, retries, and CRC
 * correctness.
 *
 * This function performs no retries, delays, or protocol-level logic.
 *
 * @param msg  Pointer to the CAN_message to be transmitted.
 *
 * @return CAN_TX_STATE
 */
CAN_TX_STATE CAN_send(const CAN_message *msg);

/**
 * @brief Retrieve the next received CAN(-FD) frame, if any.
 *
 * Non-blocking primitive that reads one frame from the FDCAN RX FIFO.
 * If a frame is available, it is copied into rsp and OK is returned.
 * If the FIFO is empty, NO_MESSAGE is returned and rsp is not modified.
 *
 * This function performs no filtering, waiting, or protocol-level logic.
 *
 * @param msg  Pointer to a CAN_message structure to receive the frame.
 *
 * @return CAN_RX_STATE
 *
 * @note If and only if this function returns OK, rsp is guaranteed to be valid.
 */
CAN_RX_STATE CAN_receive(CAN_message *msg);

static inline can_std_id_t CAN_make_ID(boot_msg_class_t msg_class, priority_t priority, node_ID_t addr);