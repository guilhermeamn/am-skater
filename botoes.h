#ifndef BOTOES_H
#define BOTOES_H

#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"

void destrava_PINO(uint32_t portalGPIO, uint8_t PINO);
void configura_PINO_PULLUP(uint32_t portalGPIO, uint8_t PINO);
void configuracoes(void);
int le_matriz(void);

#endif
