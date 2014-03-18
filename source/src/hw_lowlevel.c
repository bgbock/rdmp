#include "hw_platform.h"
#include "hw_lowlevel.h"
#include <string.h>

char rcvd_str[40],rcvd_ptr;
volatile char rcvd_flag;

char f_gsm_ok, f_gsm_cr, f_gsm_ring, f_gsm_nc;

void set_sr (unsigned char data)
{
unsigned char i,u;
SR_DAT_OUT;
for (i=0;i<8;i++)
	{
	if (data&0x80)
		SR_DAT_HI;
	else
		SR_DAT_LO;
	data = data<<1;
	bdly();
	SR_CLK_HI;
	bdly();
	SR_CLK_LO;
	}
SR_DAT_IN;
bdly();
}

unsigned char read_keys (void)
{
unsigned char i,data,shift_mask;
data = 0;
shift_mask = 0x01;
for (i=0;i<7;i++)
	{
	set_sr(shift_mask);
	data = data << 1;
	shift_mask = shift_mask << 1;
	if (SR_DAT_VAL!=0)
		{
		data = data | 0x01;
//		DISP_EN_HI;
		}
//	else
//		DISP_EN_LO;
	}
return data;
}


void dly_ms (unsigned int ms)
{
int i;
for (i=0;i<ms;i++) dly_1ms();
}

void pulseE (void)
{
	DISP_EN_LO;
	bdly();
	DISP_EN_HI;
	bdly();
	DISP_EN_LO;
}

// Send 4-bit command to LCD
void LCD_command4 (unsigned char cmd)
{
	set_sr(cmd&0x0f);
	pulseE();
	dly_ms(1);
}

// Send 8-bit command to LCD
void LCD_command8 (unsigned char cmd)
{
	set_sr((cmd>>4)&0x0f);			// Send upper WORD
	pulseE();
	dly_ms(1);
	set_sr(cmd&0x0f);				// Send lower WORD
	pulseE();
	bdly();
	dly_ms(1);
}

// Initialization
void LCD_init(void)
{
//	delay50ms();
	LCD_command4(0b0011);		// Function set
	dly_ms(10);
	LCD_command4(0b0011);		// Function set
	dly_ms(10);
	LCD_command4(0b0011);		// Function set
	dly_ms(10);
	LCD_command4(0b0010);		// Function set
	dly_ms(10);
	LCD_command8(0b00101001);	// Function set
	LCD_command8(0b00010100);	// Bias
	LCD_command8(0b01111000);	// Contrast set
	LCD_command8(0b01011110);	// Power/ICON/Contrast
	LCD_command8(0b01101100);	// Follower control
	LCD_command8(0b00001100);	// Display ON
	LCD_command8(0b00000001);	// Clear display
	LCD_command8(0b00000110);	// Entry mode set
}


// Send character to LCD
void LCD_char(char data)
{
	set_sr(((data>>4)&0x0f)|0x10);			// Send upper WORD
	pulseE();
	bdly();
	set_sr((data&0x0f)|0x10);				// Send lower WORD
	pulseE();
	bdly();
}

void update_lcd (char * str)
{
unsigned char i,zero;
LCD_command8(0b00000010);
zero = 0;
for (i=0;i<16;i++)
	{
	if (zero==1) LCD_char(' ');
	if (zero==0)
		{
		if (*str!=0) LCD_char(*str++);
		else zero=1;
		}
	}
}

void rx_isr_callback (unsigned char data)
{
if (data<0x20)
	{
	if (rcvd_ptr>0)
		{
		rcvd_str[rcvd_ptr] = 0;
		rcvd_flag = 1;
		rcvd_ptr = 0;
		if (strcmp(rcvd_str,"call ready")==0)
			f_gsm_cr = 1;
		if (strcmp(rcvd_str,"no carrier")==0)
			f_gsm_nc = 1;
		if (strcmp(rcvd_str,"ok")==0)
			f_gsm_ok = 1;
		if (strcmp(rcvd_str,"ring")==0)
			f_gsm_ring = 1;
		}
	}
else
	{	
	if ((data>='A')&(data<='Z')) data = data - ('A'-'a');
	rcvd_str[rcvd_ptr++] = data;
	}
if (rcvd_ptr==sizeof(rcvd_str)) rcvd_ptr = 0;
}


/*
7/1		8/2		9/3
NO/4	0/5		YES/6	N/SH
*/
unsigned char keys_new, keys_old, keys_sm;
void keys_init (void)
{
keys_sm = 0;
}

#ifdef	USE_KEYS
unsigned char keys_handling (unsigned char * keyp)
{
unsigned char key,key_pres,shift;
key_pres = 0;
keys_new = read_keys();
if (keys_new&KS_MASK) shift = 1;
else shift = 0;
keys_new = keys_new & (~KS_MASK);
if (keys_sm==0)
	{
	if ((keys_new==keys_old)&(keys_new!=0))
		{
		keys_sm = 1;
		key_pres = 1;	
		if (shift)
			{
			if (keys_new&K1_MASK) key = 1;
			if (keys_new&K2_MASK) key = 2;
			if (keys_new&K3_MASK) key = 3;
			if (keys_new&K4_MASK) key = 4;
			if (keys_new&K5_MASK) key = 5;
			if (keys_new&K6_MASK) key = 6;
			}
		else	
			{
			if (keys_new&K1_MASK) key = 7;
			if (keys_new&K2_MASK) key = 8;
			if (keys_new&K3_MASK) key = 9;
			if (keys_new&K4_MASK) key = KEY_NO;
			if (keys_new&K5_MASK) key = 0;
			if (keys_new&K6_MASK) key = KEY_YES;
			}
		*keyp = key;
		}
	}
if (keys_sm==1)
	{
	if ((keys_new==keys_old)&(keys_new==0))
		keys_sm = 0;		
	}
keys_old = keys_new;
return key_pres;
}
#endif


#ifdef	USE_RDIAL
unsigned char rd_counter;
unsigned char keys_handling (unsigned char * keyp)
{
unsigned char key_pres;
key_pres = 0;
keys_new = read_keys();
if ((keys_new&RDEN_MASK)!=0)
	{
	if (keys_sm==0)
		{
		keys_sm=1;
		rd_counter = 0;
		}
	}
else
	{
	if (keys_new == keys_old)
		{
		if (keys_new&K4_MASK)
			{
			*keyp = KEY_NO;
			key_pres = 1;
			}
		if (keys_new&K6_MASK)
			{
			*keyp = KEY_YES;
			key_pres = 1;
			}

	}
	else if (rd_counter>0)
		{
		if (rd_counter==10)
			rd_counter = 0;
		*keyp = rd_counter;
		rd_counter = 0;
		keys_sm=0;
		key_pres = 1;
		}
	}
if (keys_sm==1)
	{
	if ((keys_new&RDCK_MASK)==0)
		{
		if (keys_new == keys_old)	//twice in a row active level
			{
			rd_counter++;
			keys_sm=2;
			}
		}
	}
if (keys_sm==2)
	{
	if ((keys_new&RDCK_MASK)!=0)
		keys_sm=1;
	}
keys_old = keys_new;
return key_pres;
}
#endif


