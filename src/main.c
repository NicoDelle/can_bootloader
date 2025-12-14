/**
 * Implementation of a bootloader through FDCAN protocol
 */

#include <stdint.h>
#include "libCAN.h"

void bootloader();
uint8_t bootloader_requested();
uint8_t request_firmware();
void receive_and_flash_firmware(uint8_t nodeID);
void clear_bootloader_flag();
void system_reset();
void jump_to_application();

//Return 1 if the given uid matches the first 96 bits of the given message
uint8_t is_recipient(uint32_t uid[3], CAN_message *rsp);


/**
 * Bootloader code executed every time at startup. Main??
 * 
 */
void bootloader()
{
    uint8_t nodeID;
    can_init();
    if (bootloader_requested())
    {
        nodeID = request_firmware();
        receive_and_flash_firmware(nodeID);
        clear_bootloader_flag(); //This flag must be set by application code
        system_reset(); //Reboot into new firmware
    }
    else
    {
        jump_to_application();
    }
}

/**
 * Checks a flag in flash memory that detemrines wether the device was asked to enter bootloader mode at the previous boot
 */
uint8_t bootloader_requested()
{
    return 0;
}

/**
 * @brief Request firmware from the master (Jetson).
 *
 * Sends the MCU's 96-bit UID to the master and waits for a handshake
 * message assigning a logical node ID for the rest of the bootloader session.
 *
 * Uses non-blocking CAN primitives internally. Loops until the message
 * is successfully sent and a valid BOOT_FW_ASSIGN response for this UID
 * is received.
 *
 * @return uint8_t  The logical node ID assigned by the master for this MCU.
 */
uint8_t request_firmware()
{
    //These addresses hold the UID
    uint32_t uid[3];
    uid[0] = *((uint32_t *) 0x1FFF7590); //lsb
    uid[1] = *((uint32_t *) 0x1FFF7594);
    uid[2] = *((uint32_t *) 0x1FFF7598); //msb

    CAN_message msg;
    msg.id = CAN_make_ID(BOOT_FW_REQUEST, PRIORITY_HIGH, NO_NODE_ID);
    msg.len = 12;
    for (int i = 0; i < 3; i++)
    {
        msg.data[i*4 + 0] = (uid[i] >> 24) & 0xFF;
        msg.data[i*4 + 1] = (uid[i] >> 16) & 0xFF;
        msg.data[i*4 + 2] = (uid[i] >>  8) & 0xFF;
        msg.data[i*4 + 3] = (uid[i] >>  0) & 0xFF;
    }

 
    CAN_TX_STATE tx_state;
    do //TODO revise error handling here
    {
        tx_state = CAN_send(&msg);
        HAL_Delay(10);
    } while (tx_state != OK);

    //We must now wait for an high level acknowledgment
    CAN_RX_STATE rx_state;
    CAN_message resp;

    do
    {
        rx_state = CAN_receive(&resp);
        if (rx_state == NO_MESSAGE) HAL_Delay(5);
    } while (
        rx_state != OK || 
        CAN_MSG_CLASS(resp.id) != BOOT_FW_ASSIGN ||
        !is_recipient(uid, &resp)
    );

    //RIGHT Message received: return node ID and go to the next phase
    return resp.data[12];
}

/**
 * Receive from master the firmware and flashes it (chunk by chunk)
 */
void receive_and_flash_firmware(uint8_t nodeID)
{
    //1. filter messages based on nodeID
}

/**
 * Reset the value of the flag in persistent memory that signals the MCU it has to enter bootloader mode
 */
void clear_bootloader_flag()
{
    return;
}

/**
 * Jumps to application code, doing register setup
 */
void jump_to_application()
{
    return;
}


/**
 * Reset every peripheral (more on Obsidian) and reboot into new firmware
 */
void system_reset()
{
    return;
}


uint8_t is_recipient(uint32_t uid[3], CAN_message *rsp)
{
    if (rsp->len < 12) return 0; // Message too short to contain full UID

    for (int i = 0; i < 3; i++)
    {
        uint32_t payload_word = 0;
        payload_word |= ((uint32_t)rsp->data[i*4 + 0]) << 24;
        payload_word |= ((uint32_t)rsp->data[i*4 + 1]) << 16;
        payload_word |= ((uint32_t)rsp->data[i*4 + 2]) << 8;
        payload_word |= ((uint32_t)rsp->data[i*4 + 3]) << 0;

        if (payload_word != uid[i]) return 0;
    }

    return 1;
}