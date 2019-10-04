#include "RTC.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"	  


_calendar_obj calendar;//时间结构体


/****************************************************
* 函数名：  RTC_NVIC_Config
* 函数描述：RTC中断配置
* 输入参数：无
* 输出结果：无
* 返回值  ：无
*****************************************************/
static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/****************************************************
* 函数名：  RTC_Init
* 函数描述：初始化实时时钟
* 输入参数：无
* 输出结果：初始化是否成功
* 返回值  ：0 成功 / 其他 失败
*****************************************************/
unsigned char RTC_Init(void)
{
    unsigned char temp = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);//使能PWR和BKP外设时钟
    PWR_BackupAccessCmd(ENABLE);//使能后备寄存器访问 
    if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050)//从指定的后备寄存器中读出数据:判断是否已经写过时间信息
    {
        BKP_DeInit();//复位备份区域
        RCC_LSEConfig(RCC_LSE_ON);//设置外部低速晶振(LSE),使用外设低速晶振
        while((RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET)&&(temp<250))//检查指定的RCC标志位设置与否,等待低速晶振就绪
        {
            temp++;
            delay_ms(10);
        }
        if(temp > 250)return 1;
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);//使能RTC时钟  
        RTC_WaitForLastTask();
        RTC_WaitForSynchro();
        RTC_ITConfig(RTC_IT_SEC,ENABLE);//RTC秒中断使能
        RTC_WaitForLastTask();
        RTC_EnterConfigMode();//进入配置模式
        RTC_SetPrescaler(32768);//RTC时钟分频系数
        RTC_WaitForLastTask();
        RTC_Set(2018,7,16,9,16,30);
        RTC_ExitConfigMode();//退出设置模式
        BKP_WriteBackupRegister(BKP_DR1,0x5050);
    }
    else
    {
        RTC_WaitForSynchro();
        RTC_ITConfig(RTC_IT_SEC,ENABLE);//RTC秒中断使能
        RTC_WaitForLastTask();
    }
    RTC_NVIC_Config();
    RTC_Get();
    return 0;
}    

/****************************************************
* 函数名：  RTC_IRQHandler
* 函数描述：RTC中断服务函数
* 输入参数：无
* 输出结果：无
* 返回值  ：无
*****************************************************/
void RTC_IRQHandler(void)
{
    if(RTC_GetFlagStatus(RTC_IT_SEC) == SET)//秒钟中断
        RTC_Get();
    if(RTC_GetFlagStatus(RTC_IT_ALR) == SET)//闹钟中断
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);
        RTC_Get();
        printf("Alarm Time:%d-%d-%d  %d:%d:%d\n",calendar.year,calendar.month,calendar.date,calendar.hour,calendar.min,calendar.sec);    
    }
    RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
    RTC_WaitForLastTask();

}

/****************************************************
* 函数名：  Is_Leap_Year
* 函数描述：判断是否是闰年
* 输入参数：Year 年
* 输出结果：该年份是否为闰年
* 返回值  ：1 是 / 0 不是
*****************************************************/
unsigned char Is_Leap_Year(unsigned short int year)
{
    if(year%4 == 0)
    {
        if(year%100 == 0)
        {
            if(year%400 == 0)
            {
                return 1;
                
            }else return 0;
        }else return 1;
    }else return 0;
}


const unsigned char table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
const unsigned char mon_table[12] ={31,28,31,30,31,30,31,31,30,31,30,31};//平年的月份日期表

/****************************************************
* 函数名：  RTC_Set
* 函数描述：设置时钟，把输入的时间转化为秒
* 输入参数：年月日时分秒
* 输出结果：配置是否成功
* 返回值  ：0 成功 / 其他 失败
* 备注    ：以1970年1月1日为基准 1970~2099年为合法年份
*****************************************************/
unsigned char RTC_Set(unsigned int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
    unsigned short int t;
    unsigned int seccount = 0;
    if(syear<1970||syear>2099)return 1;
    for(t=1970;t<syear;t++)
    {
        if(Is_Leap_Year(t))seccount+=31622400;
        else seccount+=31536000;
    }
    smon-=1;
    for(t=0;t<smon;t++)
    {
        seccount+=(unsigned int)mon_table[t]*86400;
        if(Is_Leap_Year(t)&&t==1)seccount+=86400;
    }
    seccount+=(unsigned int)(sday-1)*86400;
    seccount+=(u32)hour*3600;//小时秒钟数
    seccount+=(u32)min*60;	 //分钟秒钟数
	seccount+=sec;//最后的秒钟加上去
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(seccount);
    RTC_WaitForLastTask();
    
    return 0;
}

/****************************************************
* 函数名：  RTC_Alarm_Set
* 函数描述：设置闹钟，把输入的时间转化为秒
* 输入参数：年月日时分秒
* 输出结果：配置是否成功
* 返回值  ：0 成功 / 其他 失败
* 备注    ：以1970年1月1日为基准 1970~2099年为合法年份
*****************************************************/
unsigned char RTC_Alarm_Set(unsigned int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
    return 1;
}



/****************************************************
* 函数名：  RTC_Get
* 函数描述：得到当前时间
* 输入参数：无
* 输出结果：是否成功
* 返回值  ：0 成功 / 其他失败
*****************************************************/
unsigned char RTC_Get()
{
    static unsigned short int daycnt = 0;
    unsigned int timecount = 0;
    unsigned int temp = 0;
    unsigned int temp1 = 0;
    timecount = RTC_GetCounter();
    temp = timecount / 86400;
    if(daycnt != temp)
    {
        daycnt = temp;
        temp1 = 1970;
        while(temp>=365)
        {
            if(Is_Leap_Year(temp1))
            {
                if(temp>=366)temp-=366;
                else{temp1++;break;}
            }
            else temp-=365;
            temp1++;
        }
        calendar.year = temp1;
        temp1 = 0;
        while(temp>=28)
        {
            if(Is_Leap_Year(calendar.year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++; 
        }
        calendar.month=temp1+1;	//得到月份
		calendar.date=temp+1;  	//得到日期 
    }
    temp = timecount % 86400;
    calendar.hour=temp/3600;     	//小时
    calendar.min=(temp%3600)/60; 	//分钟	
    calendar.sec=(temp%3600)%60; 	//秒钟
    calendar.week=RTC_Get_Week(calendar.year,calendar.month,calendar.date);//获取星期   
    return 0;
}

/****************************************************
* 函数名：  RTC_Get_Week
* 函数描述：获得现在是星期几
* 输入参数：年月日
* 输出结果：当天星期几
* 返回值  ：星期号
*****************************************************/
unsigned char RTC_Get_Week(unsigned short int year,unsigned char month,unsigned char day)
{
    u16 temp2;
	u8 yearH,yearL;
	
	yearH = year / 100;	
    yearL = year % 100; // 如果为21世纪,年份数加100 
	if (yearH>19)
        yearL += 100;// 所过闰年数只算1900年之后的
	temp2 = yearL+yearL/4;
	temp2 = temp2%7; 
	temp2 = temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)
        temp2--;

	return(temp2%7);
}





