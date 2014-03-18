#include "hw_platform.h"
#include "hw_lowlevel.h"
#include "gpio.h"
#include "uart.h"
#include "mrt.h"

void configurePins();

void init_hw (void)
{
	gpioInit();
	uart0Init(38400);
	mrtInit(__SYSTEM_CLOCK/1000);
	configurePins();
	gpioSetDir(0,SR_DAT,1);
	gpioSetDir(0,SR_CLK,1);
	gpioSetDir(0,SR_OUT,1);

}

void configurePins()
{
    /* Pin Assign 8 bit Configuration */
    /* U0_TXD */
    /* U0_RXD */
	LPC_SWM->PINASSIGN0 = 0xffff0201UL;


    /* Pin Assign 1 bit Configuration */
    /* RESET */
    LPC_SWM->PINENABLE0 = 0xffffffbfUL;

    /* Enable UART clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<18);

    /* Pin I/O Configuration */
    LPC_IOCON->PIO0_4 = 0x80;


}


void bdly (void)
{

volatile unsigned int i;
for (i=0;i<50;i++)
	{}

//	mrtDelay(1);
}

void dly_1ms (void)
{
mrtDelay(1);
}

void uart_sendchar(char c) 
{
	uart0SendChar(c);
}
/* send string to the rs232 */
void uart_sendstr(char *s) 
{
while (*s)
	{
	uart_sendchar(*s);
	s++;
	}
}





