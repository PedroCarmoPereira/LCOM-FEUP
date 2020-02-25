// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "kbd.h"
#include "i8254.h"
#include "mouse.h"

uint8_t kbd_irq_set;
uint8_t timer_irq_set;
uint8_t mouse_irq_set;
extern uint8_t byte_no;
extern struct packet p;
extern int counter;
extern bool pintou;
extern state_t state;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    int ipc_status;
    message msg;
    uint32_t tmp;
    unsigned long r1, r2;
    set_stream();
    if ((r1 = mouse_subscribe_int(&mouse_irq_set)) != OK){
      printf("Error subscribing to mouse interrupts: %d\n", r1);
      return 1;
    }
    read_OB(&tmp);
    while(cnt){
      if ((r2 = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r2);
        continue;
    }
     if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
                if (msg.m_notify.interrupts & mouse_irq_set) { /* subscribed interrupt */
                  mouse_ih();
                  if(byte_no == 0 && pintou) {
                    mouse_print_packet(&p);
                    cnt--;
                }
                break;
            default:
                break;
        }
    }
    }
  }
    read_OB(&tmp);
    mouse_unsubscribe_int();
    mouse_cmd(DIS_DATA_R);
    return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    uint32_t tmp;
    uint8_t cmd;
    read_OB(&tmp);
    cmd = minix_get_dflt_kbc_cmd_byte();
    sys_outb(STATUS_REG, KBD_READ_CMD);
    read_OB(&tmp);
    sys_outb(STATUS_REG, KBD_WRITE_CMD);
    sys_outb(OUT_BUF, tmp | BIT(5));

    set_remote();
    read_OB(&tmp);
    while(cnt){
        mouse_poll();
        if (pintou) {
          mouse_print_packet(&p);
          cnt--;
        }
        tickdelay(micros_to_ticks(period*1000));

    }
    read_OB(&tmp);
    set_stream();
    kbd_cmd(KBD_WRITE_CMD);
    sys_outb(OUT_BUF, cmd);
    mouse_cmd(DIS_DATA_R);
    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  uint32_t tmp;
  message msg;
  int idle = 0;
  unsigned long r1, r2;
  set_stream();
  if ((r1 = timer_subscribe_int(&timer_irq_set)) != OK){
    printf("Error subscribing to timer interrupts: %d\n", r1);
    return 1;
  }
  if ((r1 = mouse_subscribe_int(&mouse_irq_set)) != OK){
    printf("Error subscribing to mouse interrupts: %d\n", r1);
    return 1;
  }

  read_OB(&tmp);
  read_OB(&tmp);
  while(idle != idle_time){
    if ((r2 = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r2);
      continue;
  }
   if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */
              if (msg.m_notify.interrupts & mouse_irq_set) { /* subscribed interrupt */
                mouse_ih();
                if(byte_no == 0 && pintou) {
                  mouse_print_packet(&p);
                  idle = 0;
                  counter = 0;
                }
              }
                else if(msg.m_notify.interrupts & timer_irq_set){
                  timer_int_handler();
                  if(counter % sys_hz() == 0){
                     idle++;
                   }
                }
                break;
            default:
                break;
        }
    }
  }
  read_OB(&tmp);
  mouse_unsubscribe_int();
  mouse_cmd(DIS_DATA_R);
  timer_unsubscribe_int();
  return 0;
 }


int (mouse_test_gesture)(uint8_t len, uint8_t tolerance) {
    int ipc_status;
    message msg;
    uint32_t tmp;
    unsigned long r1, r2;
    set_stream();
    if ((r1 = mouse_subscribe_int(&mouse_irq_set)) != OK){
      printf("Error subscribing to mouse interrupts: %d\n", r1);
      return 1;
    }
    read_OB(&tmp);
    while(state != COMP){
      if ((r2 = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r2);
        continue;
    }
     if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:  /*hardware interrupt notification */
                if (msg.m_notify.interrupts & mouse_irq_set) { /*subscribed interrupt */
                  mouse_ih();
                  if(byte_no == 0 && pintou) {
                    mouse_process_event(mouse_detect_event(&p), len, tolerance);
                    mouse_print_packet(&p);
                    /*detect event && process event*/
                }
                break;
            default:
                break;
        }
    }
    }
  }
    //read_OB(&tmp);
    mouse_unsubscribe_int();
    mouse_cmd(DIS_DATA_R);
    return 0;
}
