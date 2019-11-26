#include "include.h"


void Show_RTC_time(void);
void system_Init(void);
void Reflash_Wather(void);

int main()
{
    system_Init();
    while(1)
    {
        Show_RTC_time();
    }
}


void system_Init()
{
    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);	 	    //串口初始化为115200
    LED_Init();
    LCD_Init();
    POINT_COLOR = BLACK;
    RTC_Init();
    W25QXX_Init();				//初始化W25Q128
	//tp_dev.init();			//初始化触摸屏
	usart3_init(115200);		//初始化串口3 
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
    font_init();			    //检查字库是否OK      
    
    LCD_ShowString(0,0,10*8,16,16,"0000-00-00");
    LCD_ShowString(120,0,8*12,24,24,"00:00:00");
    
    ESP_8266_wifista_config();
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    Reflash_Wather();
}

void Show_RTC_time(void)
{
    static unsigned char t= 0;
    static unsigned int count = 0;
    if(t!=calendar.sec)
    {
        t=calendar.sec;
        LCD_ShowxNum(0,0,calendar.year,4,16,0x80);									  
        LCD_ShowxNum(40,0,calendar.month,2,16,0x80);									  
        LCD_ShowxNum(64,0,calendar.date,2,16,0x80);	 
        switch(calendar.week)
        {
            case 0:
                LCD_ShowString(0,16,72,16,16,"Sunday   ");
                break;
            case 1:
                LCD_ShowString(0,16,72,16,16,"Monday   ");
                break;
            case 2:
                LCD_ShowString(0,16,72,16,16,"Tuesday  ");
                break;
            case 3:
                LCD_ShowString(0,16,72,16,16,"Wednesday");
                break;
            case 4:
                LCD_ShowString(0,16,72,16,16,"Thursday ");
                break;
            case 5:
                LCD_ShowString(0,16,72,16,16,"Friday   ");
                break;
            case 6:
                LCD_ShowString(0,16,72,16,16,"Saturday ");
                break;  
        }
        LCD_ShowxNum(120,0,calendar.hour,2,24,0x80);									  
        LCD_ShowxNum(156,0,calendar.min,2,24,0x80);									  
        LCD_ShowxNum(192,0,calendar.sec,2,24,0x80);
        LED0=!LED0;
        count++;
        if((calendar.min==30)&&(calendar.sec==0))
        {
            LED1=!LED1;
            //delay_ms(100);
            Reflash_Wather();
            LED1=!LED1;
        }
    }
}
