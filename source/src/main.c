/**************************************************************************/
#include <stdio.h>
#include "LPC8xx.h"
#include "hw_lowlevel.h"
#include "hw_platform.h"
#include "other.h"


/*
 ./lpc21isp /home/jarin/workspacelpcxpresso/LPC810_CodeBase/Release/LPC810_CodeBase.hex  /dev/ttyUSB0 115200 12000
 */

#if defined(__CODE_RED)
  #include <cr_section_macros.h>
  #include <NXP/crp.h>
  __CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif




//#define	TEST
#define	GSM

#ifdef	GSM

  unsigned char keys, main_state, main_substate;
  extern char f_gsm_ok, f_gsm_cr, f_gsm_ring, f_gsm_nc;
  char number[24];
  unsigned char key,number_pointer;


  int main (void)
  {
  init_hw();
  LCD_init();
  keys_init();
  number_pointer = 0;
  number[number_pointer] = 0;
  main_substate = 0;
  //main_state = STATE_IDLE;
  main_state = STATE_INIT;
  //main_substate = 2;
  while (1)
  	{
//  	dly_ms(NORMAL_CYCLE_MS);
  	if (main_state==STATE_INIT)
  		{
  		f_gsm_ok = 0;
  		if (main_substate==0)
  			{
  	  		update_lcd("Init...");
  			uart_sendstr("AT\r\nAT\r\n");
  			main_state=STATE_INIT_W;
  			}
  		if (main_substate==1)
  			{
  	  		if (number_pointer>0)
  	  			update_lcd(number);
  	  		else
  	  			update_lcd("PIN?");
			if (keys_handling(&key))
				{
				if (key<10)
					{
					number[number_pointer++] = key + '0';
					number[number_pointer]=0;
					}
				if (key==KEY_YES)
					{
					f_gsm_ok = 0;
					main_state=STATE_WFCALL;
					uart_sendstr("AT+CPIN=");
					uart_sendstr(number);
					uart_sendstr("\r\n");
					main_state=STATE_INIT_W;
					update_lcd("PIN...");
					number_pointer = 0;
					number[number_pointer] = 0;
					}
				if (key==KEY_NO)
					{
					number_pointer = 0;
					number[number_pointer] = 0;
					}
				}
  			}
  		if (main_substate==2)
  			{
  			update_lcd("PIN OK");
  			uart_sendstr("AT+CLVL=100\r\n");
  			main_state=STATE_INIT_W;
  			}
  		if (main_substate==3)
  			{
  			update_lcd("S1 OK");
  			uart_sendstr("AT+CALM=0\r\n");
  			main_state=STATE_INIT_W;
  			}
  		if (main_substate==4)
  			{
  			update_lcd("S2 OK");
  			uart_sendstr("AT+IPR=38400\r\n");
  			main_state=STATE_INIT_W;
  			}
  		if (main_substate==5)
  			{
  			update_lcd("Network...");
  			if (f_gsm_cr)
  				main_state = STATE_IDLE;
  			}
  		}

  	if (main_state==STATE_INIT_W)
  		{
  		if (f_gsm_ok==1)
  			{
  			f_gsm_ok = 0;
  			main_state=STATE_INIT;
  			main_substate++;
  			}
  		}

  	if (main_state==STATE_IDLE)
  		{
  		if (f_gsm_ring)
  			{
  			f_gsm_ring=0;
  			main_state=STATE_WFINCALL;
  			}
  		if (number_pointer>0)
  			update_lcd(number);
  		else
  			update_lcd("Ready");
  		if (keys_handling(&key))
  			{
  			if (key<10)
  				{
  				number[number_pointer++] = key + '0';
  				number[number_pointer]=0;
  				}
  			if (key==KEY_YES)
  				{
  				f_gsm_ok = 0;
  				main_state=STATE_WFCALL;
  				uart_sendstr("ATD");
  				uart_sendstr(number);
  				uart_sendstr(";\r\n");
  				}
  			if (key==KEY_NO)
  				{
  				number_pointer = 0;
  				number[number_pointer] = 0;
  				}
  			}
  		}

  	if (main_state==STATE_WFCALL)
  		{
  		update_lcd("Wait");
		if (f_gsm_ok==1)
  			{
  			f_gsm_ok = 0;
  			main_state=STATE_CALLING;
  			}
  		}

  	if (main_state==STATE_CALLING)
  		{
  		update_lcd("Calling");
  		if (f_gsm_nc)
  			{
  			f_gsm_nc = 0;
  			main_state=STATE_IDLE;
  			}
  		if (keys_handling(&key))
  			{
  			if (key==KEY_YES)
  				{
  				}
  			if (key==KEY_NO)
  				{
  				f_gsm_ok = 0;
  				uart_sendstr("ATH\r\n");
  				main_state=STATE_WFECALL;
  				}
  			}
  		}
  	if (main_state==STATE_WFECALL)
  		{
  		update_lcd("Ending");
  		if (f_gsm_ok==1)
  			{
  			f_gsm_ok = 0;
  			main_state=STATE_IDLE;
  			}
  		}

  	if (main_state==STATE_WFINCALL)
  		{
  		update_lcd("RING");
  		if (f_gsm_nc)
  			{
  			f_gsm_nc = 0;
                          f_gsm_ring = 0;
  			main_state=STATE_IDLE;
  			}
  		if (keys_handling(&key))
  			{
  			if (key==KEY_YES)
  				{
  				f_gsm_ok = 0;
  				uart_sendstr("ATA\r\n");
  				main_state=STATE_WFAINCALL;
  				}
  			if (key==KEY_NO)
  				{
  				f_gsm_ok = 0;
  				uart_sendstr("ATH\r\n");
  				main_state=STATE_WFEINCALL;
  				}
  			}
  		}

  	if (main_state==STATE_WFEINCALL)
  		{
  		update_lcd("Ending");
  		if (f_gsm_ok==1)
  			{
  			f_gsm_ok = 0;
  			main_state=STATE_IDLE;
  			}
  		}

  	if (main_state==STATE_WFAINCALL)
  		{
  		update_lcd("Wait");
  		if (f_gsm_ok==1)
  			{
  			f_gsm_ok = 0;
  			main_state=STATE_INCALL;
  			}
  		}

  	if (main_state==STATE_INCALL)
  		{
  		update_lcd("Incall");
  		if (f_gsm_nc)
  			{
  			f_gsm_nc = 0;
  			main_state=STATE_IDLE;
  			}
  		if (keys_handling(&key))
  			{
  			if (key==KEY_NO)
  				{
  				f_gsm_ok = 0;
                                  f_gsm_ring = 0;
  				uart_sendstr("ATH\r\n");
  				main_state=STATE_WFEINCALL;
  				}
  			}
  		}
  	}
  }


#endif

#ifdef	TEST

  unsigned char keys,key;
  extern volatile char rcvd_flag;
  extern char rcvd_str[40];
  extern char f_gsm_ok, f_gsm_cr, f_gsm_ring, f_gsm_nc;


 int main (void)
  {
  init_hw();
  LCD_init();
  SR_DAT_IN;
  while (1)
  	{
	uart_sendchar('A');
	uart_sendstr("0123456789abcdef");
 // 	dly_ms(40);
  	if (rcvd_flag)
  		{
  		rcvd_flag=0;
  		uart_sendstr("R:");
  		if (f_gsm_ok)
  			{
  			f_gsm_ok=0;
  			uart_sendstr("G_OK");
  			}
  		if (f_gsm_nc)
  			{
  			f_gsm_nc=0;
  			uart_sendstr("G_NC");
  			}
  		if (f_gsm_cr)
  			{
  			f_gsm_cr=0;
  			uart_sendstr("G_CR");
  			}
  		if (f_gsm_ring)
  			{
  			f_gsm_ring=0;
  			uart_sendstr("G_RI");
  			}
  		uart_sendstr("\r\n");
  		}


  	keys = read_keys();

  	if (keys==0x01) 		update_lcd("K1");
  	else if (keys==0x02) 	update_lcd("K2");
  	else if (keys==0x04) 	update_lcd("K3");
  	else if (keys==0x08) 	update_lcd("K4");
  	else if (keys==0x10) 	update_lcd("K5");
  	else if (keys==0x20) 	update_lcd("K6");
  	else if (keys==0x40) 	update_lcd("K7");
  	else if (keys==0x00) 	update_lcd("0123456789ABCDEF");
  	else update_lcd("Kx");
/*
	if (keys_handling(&key))
		{
		keys = key;
		}
*/
  	}
  }

#endif
