#include "include.h"

#define WEATHER_SERVERIP 	"api.caiyunapp.com"
#define WEATHER_PORTNUM 	"80"
#define WEATHER_SERVERURL "GET https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.274063,35.193959/realtime.json?lang=en_US&unit=metric:v2\r\n"
                             //https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/realtime.json?lang=en_US
                             //https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/113.254881,35.214507/weather?lang=en_US
                             //                             河南理工大学：   113.274063,35.193959
unsigned int  UTC_Time;



u8 wather_data[700];


void Reflash_Wather(void)
{
    Wather weather;
    LCD_ShowString(0,300,22*8,16,16,"Reflash Weather.......");
    get_current_weather();
    Analyse_Wather_Data(&weather);
    LCD_Show_wather(&weather);
    Show_Str(0,300,lcddev.width,lcddev.height,"Done...气象数据来自@彩云天气",16,0);
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
	u3_printf(WEATHER_SERVERURL);
    delay_ms(20);//延时20ms返回的是指令发送成功的状态
//	ESP_8266_at_response(1);
	USART3_RX_STA=0;	//清零串口3数据
	delay_ms(1000);

    memcpy(wather_data,USART3_RX_BUF,700);
    printf("%s\r\n",wather_data);
	ESP_8266_quit_trans();//退出透传
	ESP_8266_send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接
    
	myfree(SRAMIN,p);
	return 0;
}
uint8_t Analyse_Wather_Data(Wather *weather)
{
    static unsigned char count = 0;
    cJSON* wather_pocket;
    cJSON* wather_result; 
	cJSON* weather_wind;
    cJSON* weather_precipitation;
    cJSON* weather_precipitation_local;
	
    wather_pocket = cJSON_Parse((char*)wather_data);
    if(wather_pocket==NULL){printf("json pack into cjson error...\r\n");return 1;}
    else printf("json pack into cjson success...\r\n");
    if(strcmp(cJSON_GetObjectItem(wather_pocket,"status")->valuestring,"failed")==0)
    {
        printf("Error:%s",cJSON_GetObjectItem(wather_pocket,"error")->valuestring);
        return 1;
    }
    else
    {
        if(count==0)
        {
			UTC_Time =  cJSON_GetObjectItem(wather_pocket,"server_time")->valueint;
			UTC_Time += cJSON_GetObjectItem(wather_pocket,"tzshift")->valueint;
			printf("UTC_Time:%d\r\n",UTC_Time);
            Set_RTC_with_UTC_time(UTC_Time+3);
            count++;
        }
        else if(count==2)count = 0;
        else count++;
		wather_result = cJSON_GetObjectItem(wather_pocket,"result");
        strcpy(weather->skycon,cJSON_GetObjectItem(wather_result,"skycon")->valuestring);
        weather->temperature    = cJSON_GetObjectItem(wather_result,"temperature")->valueint;
        weather->humidity       = cJSON_GetObjectItem(wather_result,"humidity")->valuedouble;
        weather->visibility     = cJSON_GetObjectItem(wather_result,"visibility")->valuedouble;
        weather->aqi            = cJSON_GetObjectItem(wather_result,"aqi")->valueint;
        weather->pm25           = cJSON_GetObjectItem(wather_result,"pm25")->valueint;
        weather->pm10           = cJSON_GetObjectItem(wather_result,"pm10")->valueint;
        weather->so2            = cJSON_GetObjectItem(wather_result,"so2")->valueint;
        weather->no2            = cJSON_GetObjectItem(wather_result,"no2")->valueint;
        weather->o3             = cJSON_GetObjectItem(wather_result,"o3")->valueint;
        weather->co             = cJSON_GetObjectItem(wather_result,"co")->valueint;
        
        weather_wind            = cJSON_GetObjectItem(wather_result,"wind");
        weather->wind_direction = cJSON_GetObjectItem(weather_wind,"direction")->valuedouble;
        weather->wind_speed     = cJSON_GetObjectItem(weather_wind,"speed")->valuedouble;
        
        weather_precipitation = cJSON_GetObjectItem(wather_result,"precipitation");
        weather_precipitation_local = cJSON_GetObjectItem(weather_precipitation,"local");
        weather->intensity = cJSON_GetObjectItem(weather_precipitation_local,"intensity")->valuedouble;
        
        
        //printf("current_wather:%s\r\ntemperature:%d\r\n",weather->skycon,weather->temperature);
        //printf("%d\r\n",weather->aqi);
    }
    cJSON_Delete(weather_wind);
    cJSON_Delete(weather_precipitation_local);
    cJSON_Delete(weather_precipitation);
    cJSON_Delete(wather_result);
    cJSON_Delete(wather_pocket);//释放内存有问题？？？
    my_mem_init(SRAMIN);//暴力释放内存
    return 0;
}
void LCD_Show_wather(Wather const *weather)
{
    u8 res;
    u8 string[30];
	file = mymalloc(SRAMIN,sizeof(FIL));
	res=f_open(file,(const TCHAR*)APP_ASCII_5427,FA_READ);//打开文件
	if(res==FR_OK)
	{
		asc2_5427 = mymalloc(SRAMIN,file->fsize);
		if(asc2_5427 != NULL)
		{
			res = f_read(file,asc2_5427,file->fsize,&br);
		}
		f_close(file);
	}
    sprintf((char*)string,"%d°",weather->temperature);
    Show_Str(10,50,lcddev.width,lcddev.height,string,24,0);
    
//    sprintf((char*)string,"%s",weather->skycon);
//    Show_Str(5,96,lcddev.width,lcddev.height,string,16,0);
    
    switch(weather->skycon[0])
    {
        case 'C':if(weather->skycon[2]=='E')Show_Str(5,96,lcddev.width,lcddev.height,"晴",16,0);
                 else                       Show_Str(5,96,lcddev.width,lcddev.height,"阴",16,0);
                 break;
        case 'P':Show_Str(5,96,lcddev.width,lcddev.height,"多云",16,0);break;
        case 'W':Show_Str(5,96,lcddev.width,lcddev.height,"大风",16,0);break;
        case 'H':Show_Str(5,96,lcddev.width,lcddev.height,"雾霾",16,0);break;
        case 'R':Show_Str(21,96,lcddev.width,lcddev.height,"雨",16,0);break;
        case 'S':Show_Str(21,96,lcddev.width,lcddev.height,"雪",16,0);break;
    }
    //Show_Str(13,96,lcddev.width,lcddev.height,"雪",16,0);break;
    sprintf((char*)string,"相对湿度：%.1f%%",weather->humidity*100);
    Show_Str(5,122,lcddev.width,lcddev.height,string,16,0);
    
    sprintf((char*)string,"风向：%d°，风速：%.2f Km/h",weather->wind_direction,weather->wind_speed);
    Show_Str(5,138,lcddev.width,lcddev.height,string,16,0);
    if(weather->intensity!=0)
    {
    sprintf((char*)string,"降水：%.3f mm/h",weather->intensity);
    Show_Str(5,154,lcddev.width,lcddev.height,string,16,0);
        if((weather->intensity>=0.03)&&(weather->intensity<=2.5))
            Show_Str(5,96,lcddev.width,lcddev.height,"小",16,0);
        else if((weather->intensity>2.5)&&(weather->intensity<=8))
            Show_Str(5,96,lcddev.width,lcddev.height,"中",16,0);
        else if((weather->intensity>8)&&(weather->intensity<=15))
            Show_Str(5,96,lcddev.width,lcddev.height,"大",16,0);
        else if(weather->intensity>15)
            Show_Str(5,96,lcddev.width,lcddev.height,"暴",16,0);
    }
    sprintf((char*)string,"能见度：%.2f Km",weather->visibility);
    Show_Str(5,170,lcddev.width,lcddev.height,string,16,0);
    
    sprintf((char*)string,"空气污染指数AQI：%d",(int)weather->aqi);
    Show_Str(10,202,lcddev.width,lcddev.height,string,16,0);
    
    sprintf((char*)string,"PM2.5:%d",weather->pm25);
    Show_Str(20,218,lcddev.width,lcddev.height,string,16,0);
    sprintf((char*)string,"PM10:%d",weather->pm10);
    Show_Str(140,218,lcddev.width,lcddev.height,string,16,0);
    
    sprintf((char*)string,"SO2 : %d" ,weather->so2);
    Show_Str(20,234,lcddev.width,lcddev.height,string,16,0);
    sprintf((char*)string,"NO2 :%d",weather->no2);
    Show_Str(140,234,lcddev.width,lcddev.height,string,16,0);
    
    sprintf((char*)string,"O3  : %d",weather->o3);
    Show_Str(20,250,lcddev.width,lcddev.height,string,16,0);
    sprintf((char*)string,"CO : %d",weather->co);
    Show_Str(140,250,lcddev.width,lcddev.height,string,16,0);
}
