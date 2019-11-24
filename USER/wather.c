#include "include.h"

#define WEATHER_SERVERIP 	"api.caiyunapp.com"
#define WEATHER_PORTNUM 	"80"
//https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/realtime.json?lang=en_US
//https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/weather?lang=en_US

u8 wather_data[700];
uint8_t get_current_weather(void);

void Reflash_Wather()
{
    get_current_weather();
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
	u3_printf("GET https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/realtime.json?lang=en_US\n\n");	
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
