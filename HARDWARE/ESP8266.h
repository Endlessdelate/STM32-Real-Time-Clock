#ifndef _ESP8266_H_
#define _ESP8266_H_

//#include 
#include "usart.h"
#include "usart3.h"
#include "delay.h"
#include "malloc.h"
#include "stdlib.h"
#include "text.h"
#include "string.h"
void ESP_8266_AT_response(u8 mode);
u8* ESP_8266_check_cmd(u8 *str);
u8 ESP_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 ESP_8266_send_data(u8 *data,u8 *ack,u16 waittime);
u8 ESP_8266_quit_trans(void);
u8 ESP_8266_consta_check(void);
void ESP_8266_get_wanip(u8* ipbuf);
u8 ESP_8266_wifista_config(void);




#endif
