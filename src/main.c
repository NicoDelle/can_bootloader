/**
 * Implementation of a bootloader through FDCAN protocol
 */

#include <stdint.h>
#include "libCAN.h"

#define BOOTLOADER_ID 0x7F0

void bootloader();
uint8_t bootloader_requested();
void request_firmware();
void receive_and_flash_firmware();
void clear_bootloader_flag();
void system_reset();
void jump_to_application();


/**
 * Bootloader code executed every time at startup. Main??
 * 
 */
void bootloader()
{

    if (bootloader_requested())
    {
        request_firmware();
        receive_and_flash_firmware();
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
 * Send message to master (jetson in our case) with UID, asking for code.
 * Master will need to know in advance what MCU corresponds to the given UID.
 */
void request_firmware()
{
    //These addresses hold the UID
    uint32_t uid[3];
    uid[0] = *((uint32_t *) 0x1FFF7590); //lsb
    uid[1] = *((uint32_t *) 0x1FFF7594);
    uid[2] = *((uint32_t *) 0x1FFF7598); //msb

    CAN_message msg;
    msg.id = BOOTLOADER_ID;
    msg.len = 3;
    msg.data[0] = uid[0];
    msg.data[1] = uid[1];
    msg.data[2] = uid[2];

    can_send(&msg);
    //Wait for ACK (maybe can be done inside CAN send?) then prepare to receive the firmware
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
 * Receive from master the firmware and flashes it n(chunk by chunk)
 */
void receive_and_flash_firmware()
{
    return 0;
}

/**
 * Reset every peripheral (more on Obsidian) and reboot into new firmware
 */
void system_reset()
{
    return;
}