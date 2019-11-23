#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
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

void system_Init()
{
    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);	 	//串口初始化为115200
    LED_Init();
    LCD_Init();
    RTC_Init();
    W25QXX_Init();				//初始化W25Q128
	//tp_dev.init();				//初始化触摸屏
	usart3_init(115200);		//初始化串口3 
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
    font_init();			//检查字库是否OK      
}


int main()
{
    unsigned char t = 0;
    system_Init();
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
