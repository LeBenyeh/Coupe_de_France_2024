/*
 * parser.h
 *
 *  Created on: Dec 16, 2022
 *      Author: albin
 */

#ifndef INC_PARSER_H_
#define INC_PARSER_H_

#include "stdbool.h"
#include "Arduino.h"

#define MAX_BUFFER_CNT 8
#define MAX_BUFFER_LEN 32

#define SOURCE_SERIAL_1 1
#define SOURCE_SERIAL_USB 2

#define ERR_NO_FREE_BUFFER 1;


typedef enum
{
	EMPTY,
	WRITING,
	FILLED,
	READY,
	BUFF_ERROR
}buffer_state;

typedef struct msg_value
{
	bool present;
	bool has_value;
	int i_val;
	float f_val;
}value;

typedef struct msg_buffer_t
{
	uint8_t source;
	uint32_t origin_date;
	char message[MAX_BUFFER_LEN];
	uint8_t len;
	buffer_state state;
	value val[26];

}msg_buffer;

int16_t l_to_id(char c);
void init_msg_handler();
int write_to_buffer(char * data, uint16_t len, uint8_t source);
void parse_all();
msg_buffer* get_message(int * pos);

#endif /* INC_PARSER_H_ */
