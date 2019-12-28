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
    char temperature;//温度
    double pres;//气压(Pa)
    double humidity;//相对湿度(%)
    uint16_t wind_direction;//风向
    double  wind_speed;//风速
    double intensity;//雷达降水强度
    double visibility;//能见度
    char skycon[20];//天气现象
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

