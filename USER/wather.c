#include "include.h"

#define WEATHER_SERVERIP 	"api.caiyunapp.com"
#define WEATHER_PORTNUM 	"80"
//https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/realtime.json?lang=en_US
//https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/weather?lang=en_US

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
//	u8 ipbuf[16]; 	//IP����
	p=mymalloc(SRAMIN,40);							//����40�ֽ��ڴ�
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",WEATHER_SERVERIP,WEATHER_PORTNUM);    //����Ŀ��TCP������
	res = ESP_8266_send_cmd(p,"OK",200);//���ӵ�Ŀ��TCP������
	if(res==1)
	{
		myfree(SRAMIN,p);
		return 1;
	}
	delay_ms(300);
	ESP_8266_send_cmd("AT+CIPMODE=1","OK",100);      //����ģʽΪ��͸��	
//	ESP_8266_get_wanip(ipbuf);//��ȡWAN IP

	USART3_RX_STA=0;
	ESP_8266_send_cmd("AT+CIPSEND","OK",100);         //��ʼ͸��
	printf("start trans...\r\n");
	u3_printf("GET https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/realtime.json?lang=en_US\n\n");	
	delay_ms(20);//��ʱ20ms���ص���ָ��ͳɹ���״̬
//	ESP_8266_at_response(1);
	USART3_RX_STA=0;	//���㴮��3����
	delay_ms(1000);
//  delay_ms(500);
//	ESP_8266_at_response(0);
//	if(USART3_RX_STA&0X8000)		//��ʱ�ٴνӵ�һ�����ݣ�Ϊ����������
//	{ 
//		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
//	}
    memcpy(wather_data,USART3_RX_BUF,700);
    printf("%s\r\n",wather_data);
	ESP_8266_quit_trans();//�˳�͸��
	ESP_8266_send_cmd("AT+CIPCLOSE","OK",50);         //�ر�����
    
	myfree(SRAMIN,p);
	return 0;
}
uint8_t Analyse_Show_Wather_Data(void)
{
    cJSON* wather_pocket = cJSON_Parse((char*)wather_data);
    cJSON *wather_result = cJSON_GetObjectItem(wather_pocket,"result"); 
    char* current_wather;
    int temperature;
    int  UTC_Time;
    if(wather_pocket==NULL){printf("json pack into cjson error...\r\n");return 1;}
    else printf("json pack into cjson success...\r\n");
    //cJSON_Print(wather);
    current_wather = cJSON_GetObjectItem(wather_result,"skycon")->valuestring;
    temperature    = cJSON_GetObjectItem(wather_result,"temperature")->valueint;
    UTC_Time = cJSON_GetObjectItem(wather_pocket,"server_time")->valueint;
    printf("current_wather:%s\r\ntemperature:%d\r\n",current_wather,temperature);
    printf("UTC_Time:%d",UTC_Time);
    cJSON_Delete(wather_pocket);
    return 0;
}
