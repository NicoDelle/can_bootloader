#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

typedef struct 
{
    uint32_t id;
    uint8_t len;
    uint64_t data[8];
} CAN_message;

void can_init();
void can_send(CAN_message *msg);
void can_receive(CAN_message *msg);