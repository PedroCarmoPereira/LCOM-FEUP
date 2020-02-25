#ifndef _KBD_H
#define _KBD_H

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

void kbd_ih();

bool is_make();

bool is_twoByte();

int kbd_scan();

int kbd_poll();

int kbd_cmd(uint8_t cmd);

int read_OB(uint32_t * data);

#define ESC_MAKE	   	0x01
#define ESC_BREAK	   	0x81
#define TWO_BYTE	   	0xE0

#define KBD_IRQ 	   	1
#define OUT_BUF		   	0x60
#define STATUS_REG		0x64

#define OBF			    0x1
#define IBF           	BIT(1)
#define KBD_READ_CMD  	0x20
#define KBD_WRITE_CMD 	0x60
#define TOPARR_ERROR  	BIT(7) | BIT(6)

#define DELAY_US	   	20000
#endif
