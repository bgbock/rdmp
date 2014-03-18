
void set_sr (unsigned char data);
unsigned char read_keys (void);
void pulseE (void);
void LCD_command4 (unsigned char cmd);
void LCD_command8 (unsigned char cmd);
void LCD_init(void);
void LCD_char(char data);
void update_lcd (char * str);
void init_hw (void);
void rx_isr_callback (unsigned char data);
void dly_ms (unsigned int ms);
unsigned char keys_handling (unsigned char * keyp);
void keys_init (void);


#define	KEY_YES	11
#define	KEY_NO	12
