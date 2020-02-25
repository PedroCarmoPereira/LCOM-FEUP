// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
// Any header files included below this line should have been created by you
#include "kbd.h"
#include "i8254.h"

uint8_t kbd_irq_set;
uint8_t timer_irq_set;
extern unsigned int sys_inb_counter;
extern uint8_t scancode[2];
extern int counter;
extern unsigned short _asmScan;
void kbd_ih_asm();

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (kbd_test_scan)(bool assembly) {
  int ipc_status;
  message msg;
  int r;
  kbd_subscribe_int(&kbd_irq_set);
  while(scancode[0] != ESC_BREAK){
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r);
        continue;
    }
     if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
                if (msg.m_notify.interrupts & kbd_irq_set) { /* subscribed interrupt */

        			if(assembly) {
        				kbd_ih_asm();
        				if(scancode[0] != TWO_BYTE) scancode[0] = _asmScan;
						else scancode[1] = _asmScan;
        			}
            	    else kbd_ih();
            		if((is_twoByte() && scancode[1] != 0) || (!is_twoByte())){
                    	kbd_print_scancode(is_make(), is_twoByte() + 1, scancode);
                     	if(is_twoByte()) scancode[0] = 0;
                  	}
                }
                break;
            default:
                break;
        }
    }
  }

  kbd_print_no_sysinb(sys_inb_counter);
  kbd_unsubscribe_int();
  return 0;
}
int (kbd_test_poll)() {

  uint32_t tmp_cmdB;
  while(scancode[0] != ESC_BREAK){
    int x = kbd_poll();
    if(((is_twoByte() && scancode[1] != 0) || (!is_twoByte())) && x == 0){
       kbd_print_scancode(is_make(), is_twoByte() + 1, scancode);
       if(is_twoByte()) scancode[0] = 0;
    }
  }
  if(kbd_cmd(KBD_READ_CMD) != 0) return 1;
  read_OB(&tmp_cmdB);
  tmp_cmdB |= 1;
  if(kbd_cmd(KBD_WRITE_CMD) != 0) return 2;
  sys_outb(OUT_BUF, tmp_cmdB);
  kbd_print_no_sysinb(sys_inb_counter);
  return 0;
}
int (kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;
  message msg;
  int r;
  int idle = 0;
  timer_subscribe_int(&timer_irq_set);
  kbd_subscribe_int(&kbd_irq_set);
  while((scancode[0] != ESC_BREAK) && (idle != n)){
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r);
        continue;
    }
     if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
                if (msg.m_notify.interrupts & kbd_irq_set) { /* subscribed interrupt */
                  kbd_ih();
                  if((is_twoByte() && scancode[1] != 0) || (!is_twoByte())){
                     kbd_print_scancode(is_make(), is_twoByte() + 1, scancode);
                     if(is_twoByte()) scancode[0] = 0;
                     idle = 0;
                  }
                }
                else if(msg.m_notify.interrupts & timer_irq_set){
                  timer_int_handler();
                  if(counter % sys_hz() == 0) idle++;
                }
                break;
            default:
                break;
        }
    }
  }
  kbd_print_no_sysinb(sys_inb_counter);
  kbd_unsubscribe_int();
  timer_unsubscribe_int();
  return 0;
 }
