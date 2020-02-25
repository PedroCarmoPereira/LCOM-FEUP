// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "kbd.h"
#include "i8254.h"
#include "video.h"

extern uint8_t scancode[2];
extern vbe_mode_info_t vmi_p;
extern unsigned h_res;
extern unsigned v_res;
extern int counter;
uint8_t kbd_irq_set;
uint8_t timer_irq_set;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (video_test_init)(uint16_t mode, uint8_t delay) {
  if(set_mode(mode) == 1) printf("ERRO");
  tickdelay(micros_to_ticks(delay*1000000));
  vg_exit();
  return 0;
}

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                       uint16_t width, uint16_t height, uint32_t color) {
  int ipc_status;
  message msg;
  int r;

  if(set_mode(mode) == 1) printf("ERRO");
  vg_init(mode);


  for(int i = 0; i < height; i++){
    vg_draw_hline(x, y, width, color);
    y++;
  }

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
                 kbd_ih();
               if((is_twoByte() && scancode[1] != 0) || (!is_twoByte())){
                     if(is_twoByte()) scancode[0] = 0;
                   }
               }
               break;
           default:
               break;
       }
   }
 }


 kbd_unsubscribe_int();
 vg_exit();
 return 0;

}
/*
int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  int ipc_status;
  message msg;
  int r;


  vg_init(mode);
  unsigned rectWidth = h_res/no_rectangles;
  unsigned rectHeight = v_res/no_rectangles;
  if(set_mode(mode) == 1) printf("ERRO");


  for(unsigned int l = 0; l < no_rectangles; l++){
    for(unsigned int c = 0; c < no_rectangles; c++){
      for(unsigned int i = 0; i < rectHeight; i++){
         vg_draw_hline(c*no_rectangles, l*no_rectangles, rectWidth, color);
       }
    }
  }

kbd_subscribe_int(&kbd_irq_set);
 while(scancode[0] != ESC_BREAK){
   if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
       printf("driver_receive failed with: %d", r);
       continue;
   }
    if (is_ipc_notify(ipc_status)) {
       switch (_ENDPOINT_P(msg.m_source)) {
           case HARDWARE:
               if (msg.m_notify.interrupts & kbd_irq_set) {
                 kbd_ih();
               if((is_twoByte() && scancode[1] != 0) || (!is_twoByte())){
                     if(is_twoByte()) scancode[0] = 0;
                   }
               }
               break;
           default:
               break;
       }
   }
 }


 kbd_unsubscribe_int();
 vg_exit();
 return 0;

}*/

int (video_test_xpm)(const char *xpm[], uint16_t x, uint16_t y){
  int ipc_status;
  message msg;
  int r;
  vg_init(0x105);
  if(set_mode(0x105) == 1) printf("ERRO");
  int w, h;
  char * pixmap = read_xpm(xpm, &w, &h);
  for(int j = y; j < y + h; j++)
    for(int i = x; i < x + w; i++)
      putPixel(i, j, *pixmap++);
  kbd_subscribe_int(&kbd_irq_set);
   while(scancode[0] != ESC_BREAK){
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
         printf("driver_receive failed with: %d", r);
         continue;
     }
      if (is_ipc_notify(ipc_status)) {
         switch (_ENDPOINT_P(msg.m_source)) {
             case HARDWARE:
                 if (msg.m_notify.interrupts & kbd_irq_set) {
                   kbd_ih();
                 if((is_twoByte() && scancode[1] != 0) || (!is_twoByte())){
                       if(is_twoByte()) scancode[0] = 0;
                     }
                 }
                 break;
             default:
                 break;
         }
     }
   }


   kbd_unsubscribe_int();
   vg_exit();
   return 0;
}

int (video_test_move)(const char *xpm[], uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate){
  int ipc_status;
  message msg;
  int r;
  vg_init(0x105);
  if(set_mode(0x105) == 1) printf("ERRO");
  int w, h;
  char * pixmap = read_xpm(xpm, &w, &h);
  kbd_subscribe_int(&kbd_irq_set);
  timer_subscribe_int(&timer_irq_set);
  for(int j = yi; j < yi + h; j++)
    for(int i = xi; i < xi + w; i++)
      putPixel(i, j, *pixmap++);
  while(scancode[0] != ESC_BREAK){
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r);
        continue;
    }
     if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:
                if (msg.m_notify.interrupts & kbd_irq_set) {
                  kbd_ih();
                if((is_twoByte() && scancode[1] != 0) || (!is_twoByte())){
                      if(is_twoByte()) scancode[0] = 0;
                    }
                }
                else if (msg.m_notify.interrupts & timer_irq_set){
                  timer_int_handler();
                  puts("BBB");
                  if(counter == 1000/fr_rate){
                    if (xi < xf) xi += speed;
                    if (yi < yf) yi += speed;
                    puts("AAAA");
                    for(int j = yi; j < yi + h; j++)
                      for(int i = xi; i < xi + w; i++)
                        putPixel(i, j, *pixmap++);
                    counter = 0;
                  }
                }
                break;
            default:
                break;
        }
    }
  }


  kbd_unsubscribe_int();
  timer_unsubscribe_int();
  vg_exit();
  return 0;
}
