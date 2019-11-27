#include "include.h"

#define WEATHER_SERVERIP 	"api.caiyunapp.com"
#define WEATHER_PORTNUM 	"80"
//https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/realtime.json?lang=en_US
//https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/weather?lang=en_US
//                             河南理工大学：   113.274063,35.193959

unsigned int  UTC_Time;
typedef struct
{
    char temperature;//温度
    float pres;//气压(Pa)
    float humidity;//相对湿度(%)
    uint16_t wind_direction;//风向
    float  wind_speed;//风速
    float intensity;//雷达降水强度
    uint8_t visibility;//能见度
    char *skycon;//天气现象
    uint16_t aqi;
    uint16_t pm25;
    uint16_t pm10;
    uint16_t o3;
    uint16_t so2;
    uint16_t no2;
    uint16_t co;
}Wather;
Wather weather;

u8 wather_data[700];
uint8_t get_current_weather(void);
uint8_t Analyse_Show_Wather_Data(void);

void Reflash_Wather()
{
    get_current_weather();
    Analyse_Show_Wather_Data();
}

uint8_t get_current_weather(void)
{
    u8 *p;
	u8 res;
//	u8 ipbuf[16]; 	//IP缓存
	p=mymalloc(SRAMIN,40);							//申请40字节内存
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",WEATHER_SERVERIP,WEATHER_PORTNUM);    //配置目标TCP服务器
	res = ESP_8266_send_cmd(p,"OK",200);//连接到目标TCP服务器
	if(res==1)
	{
		myfree(SRAMIN,p);
		return 1;
	}
	delay_ms(300);
	ESP_8266_send_cmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	
//	ESP_8266_get_wanip(ipbuf);//获取WAN IP

	USART3_RX_STA=0;
	ESP_8266_send_cmd("AT+CIPSEND","OK",100);         //开始透传
	printf("start trans...\r\n");
    //TIM_Cmd(TIM4,ENABLE);
	u3_printf("GET https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.274063,35.193959/realtime.json?lang=en_US\n\n");	
    //TIM4 -> CNT = 0;
    delay_ms(20);//延时20ms返回的是指令发送成功的状态
//	ESP_8266_at_response(1);
	USART3_RX_STA=0;	//清零串口3数据
	delay_ms(1000);
//  delay_ms(500);
//	ESP_8266_at_response(0);
//	if(USART3_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
//	{ 
//		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
//	}
    memcpy(wather_data,USART3_RX_BUF,700);
    printf("%s\r\n",wather_data);
	ESP_8266_quit_trans();//退出透传
	ESP_8266_send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接
    
	myfree(SRAMIN,p);
	return 0;
}
uint8_t Analyse_Show_Wather_Data(void)
{
    static unsigned char count = 0;
    uint32_t wlyc;
    cJSON* wather_pocket = cJSON_Parse((char*)wather_data);
    cJSON* wather_result; 
    if(wather_pocket==NULL){printf("json pack into cjson error...\r\n");return 1;}
    else printf("json pack into cjson success...\r\n");
    //cJSON_Print(wather);
    if(strcmp(cJSON_GetObjectItem(wather_pocket,"status")->valuestring,"failed")==0)
    {
        printf("Error:%s",cJSON_GetObjectItem(wather_pocket,"error")->valuestring);
    }
    else
    {
        wather_result = cJSON_GetObjectItem(wather_pocket,"result");
        //weather.skycon      = cJSON_GetObjectItem(wather_result,"skycon")->valuestring;
        //weather.temperature = cJSON_GetObjectItem(wather_result,"temperature")->valueint;
        UTC_Time = cJSON_GetObjectItem(wather_pocket,"server_time")->valueint;
        //printf("current_wather:%s\r\ntemperature:%d\r\n",weather.skycon,weather.temperature);
        printf("UTC_Time:%d",UTC_Time);
        if(count==0)
        {
            //wlyc = TIM4->CNT;
            Set_RTC_with_UTC_time(UTC_Time+3);
            TIM_Cmd(TIM4,DISABLE);
            count++;
        }
        else if(count==2)count = 0;
        else count++;
    }
    cJSON_Delete(wather_pocket);
    cJSON_Delete(wather_result);
    return 0;
}
