#include "botoes.h"

/* Destrava o SW2 */
void destrava_PINO(uint32_t portalGPIO, uint8_t PINO){
    ESC_REG(portalGPIO + GPIO_O_LOCK) |= GPIO_LOCK_KEY;
    ESC_REG(portalGPIO + GPIO_O_CR) |= PINO;
}

/* Configura um pino de entrada como pull up */
void configura_PINO_PULLUP(uint32_t portalGPIO, uint8_t PINO){
    ESC_REG(portalGPIO + GPIO_O_PUR) |= PINO;
}

/* Configura os portais e pinos */
void configuracoes(void){
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
   destrava_PINO(GPIO_PORTF_BASE,0x01);
   GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
   configura_PINO_PULLUP(GPIO_PORTF_BASE,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
   GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
   GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5);
   GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
   GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 , GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

/* Envia sinal alto para a linha 1 e verifica as colunas */
int le_matriz(void){
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_4,0x00);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0,0xFF);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_1,0xFF);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_5,0xFF);

    //OBS.: SW1 nao ta funcionando, usando ultima linha de botoes
    /*GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_4,0xFF);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0,0xFF);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_1,0xFF);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_5,0x00);*/

    if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0) != GPIO_PIN_0){ //SW1
        //SCORE
        return 1;
    }
    else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_1) != GPIO_PIN_1){ //SW2
        //AJUDA
        return 2;
    }
    else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_2) != GPIO_PIN_2){ //SW3
        //PLAY
        return 3;
    }
    else if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_3) != GPIO_PIN_3){ //SW4
        return 4;
    }

    return 0;
}


