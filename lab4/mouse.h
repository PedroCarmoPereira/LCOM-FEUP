#ifndef _MOUSE_H
#define _MOUSE_H

/*
struct packet{
  uint8_t bytes[3]; //mouse packet raw bytes
  bool rb;          //right mouse button pressed
  bool mb;          //middle mouse button pressed
  bool lb;          //left mouse button pressed
  uint16_t delta_x; //mouse x-displacement: rightwards is positive
  uint16_t delta_y; //mouse y-displacement: upwards is positive
  bool x_ov;        //mouse x-displacement overflow
  bool y_ov;        //mouse y-displacement overflow
};
*/ 

typedef enum {INIT, DRAW_UP, VERT,DRAW_DOWN, COMP} state_t;

int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

int mouse_cmd_aux(uint8_t cmd);

int mouse_cmd(uint8_t cmd);

int set_stream();

int set_remote();

bool isNegX(uint8_t byte);

bool isNegY(uint8_t byte);

bool parseByte(uint8_t byte);

int mouse_poll();

void mouse_process_event(struct mouse_ev * event, uint8_t len, uint8_t tol);

void printState(state_t s);

#define MOUSE_IRQ		12

#define MOUSE_CMD 		0xD4
#define DIS_DATA_R		0xF5
#define EN_DATA_R		0xF4
#define SET_STREAM		0xEA
#define SET_REMOTE		0xF0
#define READ_DATA    	0xEB
#define ACK				0xFA
#define NACK			0xFE
#define ERROR			0xFC

#endif /*_MOUSE_H*/
