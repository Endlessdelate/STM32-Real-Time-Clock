#ifndef _INCLUDE_H_
#define _INCLUDE_H_
#include <string.h>

#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "exit.h"
#include "KEY.H"
#include "lcd.h"      
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"			
#include "usart3.h"
#include "cJSON.h" 
#include "rtc.h"
#include "ESP8266.h"
#include "timer.h"
#include "string.h"
extern unsigned int  UTC_Time;

typedef struct
{
    char temperature;//�¶�
    double pres;//��ѹ(Pa)
    double humidity;//���ʪ��(%)
    uint16_t wind_direction;//����
    double  wind_speed;//����
    double intensity;//�״ｵˮǿ��
    double visibility;//�ܼ���
    char skycon[20];//��������
    uint16_t aqi;
    uint16_t pm25;
    uint16_t pm10;
    uint16_t o3;
    uint16_t so2;
    uint16_t no2;
    uint16_t co;
}Wather;
uint8_t get_current_weather(void);
uint8_t Analyse_Wather_Data(Wather *weather);
void LCD_Show_wather(Wather const *weather);
void Reflash_Wather(void);


void Show_RTC_time(void);

#endif

