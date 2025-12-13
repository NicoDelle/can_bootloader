#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"


#define BOOT_FW_REQUEST 0x7F0 //Means we are requesting to receive code
#define BOOT_FW_ASSIGN 0X7F1
#define BOOT_FW_DATA 0X7F2
#define BOOT_FW_END 0X7F3
#define BOOT_FW_ERROR 0X7FF

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

typedef struct 
{
    uint32_t id;
    uint8_t len;
    uint8_t data[64];
} CAN_message;

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

