/*
 * Trabalho Final Microcontroladores 2019.2
 * Guilherme Araujo Machado do Nascimento
 * Matrícula: 17105750
 * Turma: 04605
 * Engenharia Mecatrônica
 * UFSC Campus Joinville
 */

#include <skater.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "utils/uartstdio.h"
#include "driverlib/timer.h"
#include "utils/ustdlib.h"
#include "driverlib/hibernate.h"
#include "Nokia5110.h"
#include "botoes.h"

void trataSysTick(void){
    background();
}

void main(void)
{
    /*================================== CLOCK ==============================================*/
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    /*================================== NOKIA ==============================================*/
    Nokia5110_Init();
    Nokia5110_Clear();
    Nokia5110_SetCursor(0,0);

    /*================================== JOGO ===============================================*/
    configuracoes();                //configura a matriz
    inicio();                       //animação de inicio
    while(menu() != 0){}            //menu

    /*================================== SYSTICK ============================================*/
    SysTickEnable();
    SysTickIntEnable();
    SysTickPeriodSet(37000);

    while(1)
    {
        start();
    }
}
