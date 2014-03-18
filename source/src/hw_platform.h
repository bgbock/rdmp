#include "gpio.h"

#define	SR_CLK	0
#define	SR_DAT	4
#define	SR_OUT	3

#define	SR_CLK_HI	gpioSetValue(0,SR_CLK,1)
#define	SR_CLK_LO	gpioSetValue(0,SR_CLK,0)
#define	SR_DAT_HI	gpioSetValue(0,SR_DAT,1)
#define	SR_DAT_LO	gpioSetValue(0,SR_DAT,0)
#define	SR_DAT_VAL	gpioGetPinValue(0,SR_DAT)
#define	SR_DAT_IN	gpioSetDir(0,SR_DAT,0)
#define	SR_DAT_OUT	gpioSetDir(0,SR_DAT,1)
#define	DISP_EN_HI	gpioSetValue(0,SR_OUT,1)
#define	DISP_EN_LO	gpioSetValue(0,SR_OUT,0)

#define	K1_MASK		0x01
#define	K2_MASK		0x02
#define	K3_MASK		0x04
#define	K4_MASK		0x40
#define	K5_MASK		0x20
#define	K6_MASK		0x10
#define	KS_MASK		0x08

#define	RDEN_MASK	0x08
#define	RDCK_MASK	0x04

#define	USE_KEYS
//#define	USE_RDIAL


void bdly (void);
void dly_1ms (void);
void init_hw (void);
void uart_sendchar(char c);
void uart_sendstr(char *s);



