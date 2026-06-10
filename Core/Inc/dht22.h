#ifndef __DHT22_H__
#define __DHT22_H__

#include <stdint.h>
#include "main.h"

void DHT22_UsDelay(uint32_t time);
uint8_t DHT22_Check_Response(void);
uint8_t DHT22_Read(void);
void DHT22_Start(void);

#endif
