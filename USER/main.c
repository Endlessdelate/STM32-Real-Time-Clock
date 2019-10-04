#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "RTC.h"
#include "usart.h"

int main()
{
    unsigned char t = 0;
    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);	 	//串口初始化为115200
    LED_Init();
    LCD_Init();
    RTC_Init();
    POINT_COLOR = BLACK;
    LCD_ShowString(60,130,200,32,24,"    -  -  ");
    LCD_ShowString(60,190,200,32,24,"  :  :  ");
    while(1)
    {
        if(t!=calendar.sec)
		{
			t=calendar.sec;
			LCD_ShowNum(60,130,calendar.year,4,24);									  
			LCD_ShowNum(120,130,calendar.month,2,24);									  
			LCD_ShowNum(156,130,calendar.date,2,24);	 
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(60,160,200,16,24,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(60,160,200,16,24,"Monday   ");
					break;
				case 2:
					LCD_ShowString(60,160,200,16,24,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(60,160,200,16,24,"Wednesday");
					break;
				case 4:
					LCD_ShowString(60,160,200,16,24,"Thursday ");
					break;
				case 5:
					LCD_ShowString(60,160,200,16,24,"Friday   ");
					break;
				case 6:
					LCD_ShowString(60,160,200,16,24,"Saturday ");
					break;  
			}
			LCD_ShowNum(60,190,calendar.hour,2,24);									  
			LCD_ShowNum(96,190,calendar.min,2,24);									  
			LCD_ShowNum(132,190,calendar.sec,2,24);
			LED0=!LED0;
		}	
		delay_ms(10);						
    }
}
