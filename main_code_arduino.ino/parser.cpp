/*
 * parser.c
 *
 *  Created on: Dec 16, 2022
 *      Author: albin
 */

#include "parser.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint8_t to_read_index;
uint8_t to_write_index;
msg_buffer buffers[MAX_BUFFER_CNT];

int16_t l_to_id(char c)
{
	if('a' <= c && c <= 'z')
	{
		return c - 'a';
	}else if('A' <= c && c <= 'Z'){
		return c - 'A';
	}
	return -1;
}

void init_msg_value(value * val)
{
	val->f_val = 0.0f;
	val->i_val = 0;
	val->has_value = false;
	val->present = false;
}

void init_buffer(msg_buffer * buff)
{
	buff->len = 0;
	buff->origin_date = 0;
	buff->source = 0;
	buff->state = EMPTY;
	memset(buff->message, '\0', MAX_BUFFER_LEN);
	for(int i = 0; i < 26; i++)
	{
		init_msg_value(&(buff->val[i]));
	}
}

void init_msg_handler()
{
	for(int i = 0; i < MAX_BUFFER_CNT; i++)
	{
		init_buffer(&(buffers[i]));
	}
}

int look_for_writeable_buffer(uint8_t source)
{
	uint8_t found = 0;
	uint8_t index = 0;
	while(index < MAX_BUFFER_CNT && !found)
	{
		if(buffers[index].state == EMPTY || buffers[index].state == BUFF_ERROR|| (buffers[index].state == WRITING  && buffers[index].source == source))
		{
			return index;
		}
		index++;
	}
	return -1;
}

int look_for_readable_buffer()
{
	uint8_t found = 0;
	uint8_t index = 0;
	while(index < MAX_BUFFER_CNT && !found)
	{
		if(buffers[index].state == READY)
		{
			return index;
		}
		index++;
	}
	return -1;
}


int write_to_buffer(char * data, uint16_t len, uint8_t source)
{
	uint16_t pos = 0;
	bool can_write = true;

	int buffer_to_write = look_for_writeable_buffer(source);

	if(buffer_to_write == -1)
	{
		return ERR_NO_FREE_BUFFER;
	}
	if(buffers[buffer_to_write].state == EMPTY || buffers[buffer_to_write].state == BUFF_ERROR){
		init_buffer(&buffers[buffer_to_write]);
		buffers[buffer_to_write].state = WRITING;
		buffers[buffer_to_write].source = source;
		buffers[buffer_to_write].origin_date = millis();
	}

	while(pos < len && can_write)
	{
		char c = data[pos];
		if(c == '\r' || c == '\n' || c == ';')
		{
			buffers[buffer_to_write].state = FILLED;

			buffer_to_write = look_for_writeable_buffer(source);

			if(buffer_to_write == -1)
			{
				can_write = false;
			}else if((buffers[buffer_to_write].state == EMPTY || buffers[buffer_to_write].state == BUFF_ERROR) && pos < len - 1){ // if the last character is an end char, we do not initiate a new buffer.
				init_buffer(&buffers[buffer_to_write]);
				buffers[buffer_to_write].state = WRITING;
				buffers[buffer_to_write].source = source;
				buffers[buffer_to_write].origin_date = millis();
			}
		}
		else
		{
			buffers[buffer_to_write].message[buffers[buffer_to_write].len] = c;
			buffers[buffer_to_write].len++;
		}
		pos++;
	}
	return 0;
}

void parse(msg_buffer *buffer)
{
	uint16_t pos = 0;
	char value[MAX_BUFFER_LEN] = {0};
	uint16_t value_len = 0;
	int parameter = -1;
	while(pos < buffer->len)
	{
		char in_c = buffer->message[pos];
		if(in_c == ' ')
		{
			//End of parameter
			if(parameter != -1)
			{
				if(value_len > 0)
				{
					buffer->val[parameter].has_value = true;
					buffer->val[parameter].i_val = atoi(value);
					sscanf(value, "%f", &(buffer->val[parameter].f_val));
					//buffer->val[parameter].f_val = atoff(value);
					value_len = 0;
					memset(value, 0, MAX_BUFFER_LEN);
				}
				parameter = -1;
			}
		}else{
			int l_id = l_to_id(in_c);
			if( l_id != -1 )
			{
				if(parameter == -1)
				{
					parameter = l_id;
					buffer->val[parameter].present = true;
				}else{
					//Two consecutive letters : error
					buffer->state = BUFF_ERROR;
					return;
				}
			}else{
				value[value_len] = in_c;
				value_len++;;
			}
		}
		pos++;
	}
	if(parameter != -1 && value_len > 0)
	{
		buffer->val[parameter].has_value = true;
		buffer->val[parameter].i_val = atoi(value);
		sscanf(value, "%f", &(buffer->val[parameter].f_val));
		//buffer->val[parameter].f_val = atof(value);
	}
	buffer->state = READY;
}

void parse_all()
{
	for(int i = 0; i < MAX_BUFFER_CNT; i++)
	{
		if(buffers[i].state == FILLED){
			parse(&buffers[i]);
		}
	}
}

msg_buffer* get_message(int * pos)
{
	int buff = look_for_readable_buffer();
	*pos = buff;
	return(buff==-1?NULL:&buffers[buff]);
}
