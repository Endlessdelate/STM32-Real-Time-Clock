#include "RTC.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"	  


_calendar_obj calendar;//ʱ��ṹ��


/****************************************************
* ��������  RTC_NVIC_Config
* ����������RTC�ж�����
* �����������
* ����������
* ����ֵ  ����
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
* ��������  RTC_Init
* ������������ʼ��ʵʱʱ��
* �����������
* ����������ʼ���Ƿ�ɹ�
* ����ֵ  ��0 �ɹ� / ���� ʧ��
*****************************************************/
unsigned char RTC_Init(void)
{
    unsigned char temp = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);//ʹ��PWR��BKP����ʱ��
    PWR_BackupAccessCmd(ENABLE);//ʹ�ܺ󱸼Ĵ������� 
    if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050)//��ָ���ĺ󱸼Ĵ����ж�������:�ж��Ƿ��Ѿ�д��ʱ����Ϣ
    {
        BKP_DeInit();//��λ��������
        RCC_LSEConfig(RCC_LSE_ON);//�����ⲿ���پ���(LSE),ʹ��������پ���
        while((RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET)&&(temp<250))//���ָ����RCC��־λ�������,�ȴ����پ������
        {
            temp++;
            delay_ms(10);
        }
        if(temp > 250)return 1;
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);//ʹ��RTCʱ��  
        RTC_WaitForLastTask();
        RTC_WaitForSynchro();
        RTC_ITConfig(RTC_IT_SEC,ENABLE);//RTC���ж�ʹ��
        RTC_WaitForLastTask();
        RTC_EnterConfigMode();//��������ģʽ
        RTC_SetPrescaler(32768);//RTCʱ�ӷ�Ƶϵ��
        RTC_WaitForLastTask();
        RTC_Set(2018,7,16,9,16,30);
        RTC_ExitConfigMode();//�˳�����ģʽ
        BKP_WriteBackupRegister(BKP_DR1,0x5050);
    }
    else
    {
        RTC_WaitForSynchro();
        RTC_ITConfig(RTC_IT_SEC,ENABLE);//RTC���ж�ʹ��
        RTC_WaitForLastTask();
    }
    RTC_NVIC_Config();
    RTC_Get();
    return 0;
}    

/****************************************************
* ��������  RTC_IRQHandler
* ����������RTC�жϷ�����
* �����������
* ����������
* ����ֵ  ����
*****************************************************/
void RTC_IRQHandler(void)
{
    if(RTC_GetFlagStatus(RTC_IT_SEC) == SET)//�����ж�
        RTC_Get();
    if(RTC_GetFlagStatus(RTC_IT_ALR) == SET)//�����ж�
    {
        RTC_ClearITPendingBit(RTC_IT_ALR);
        RTC_Get();
        printf("Alarm Time:%d-%d-%d  %d:%d:%d\n",calendar.year,calendar.month,calendar.date,calendar.hour,calendar.min,calendar.sec);    
    }
    RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
    RTC_WaitForLastTask();

}

/****************************************************
* ��������  Is_Leap_Year
* �����������ж��Ƿ�������
* ���������Year ��
* ��������������Ƿ�Ϊ����
* ����ֵ  ��1 �� / 0 ����
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


const unsigned char table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
const unsigned char mon_table[12] ={31,28,31,30,31,30,31,31,30,31,30,31};//ƽ����·����ڱ�

/****************************************************
* ��������  RTC_Set
* ��������������ʱ�ӣ��������ʱ��ת��Ϊ��
* ���������������ʱ����
* �������������Ƿ�ɹ�
* ����ֵ  ��0 �ɹ� / ���� ʧ��
* ��ע    ����1970��1��1��Ϊ��׼ 1970~2099��Ϊ�Ϸ����
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
    seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;	 //����������
	seccount+=sec;//�������Ӽ���ȥ
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(seccount);
    RTC_WaitForLastTask();
    
    return 0;
}

/****************************************************
* ��������  RTC_Alarm_Set
* �����������������ӣ��������ʱ��ת��Ϊ��
* ���������������ʱ����
* �������������Ƿ�ɹ�
* ����ֵ  ��0 �ɹ� / ���� ʧ��
* ��ע    ����1970��1��1��Ϊ��׼ 1970~2099��Ϊ�Ϸ����
*****************************************************/
unsigned char RTC_Alarm_Set(unsigned int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
    return 1;
}



/****************************************************
* ��������  RTC_Get
* �����������õ���ǰʱ��
* �����������
* ���������Ƿ�ɹ�
* ����ֵ  ��0 �ɹ� / ����ʧ��
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
            if(Is_Leap_Year(calendar.year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++; 
        }
        calendar.month=temp1+1;	//�õ��·�
		calendar.date=temp+1;  	//�õ����� 
    }
    temp = timecount % 86400;
    calendar.hour=temp/3600;     	//Сʱ
    calendar.min=(temp%3600)/60; 	//����	
    calendar.sec=(temp%3600)%60; 	//����
    calendar.week=RTC_Get_Week(calendar.year,calendar.month,calendar.date);//��ȡ����   
    return 0;
}

/****************************************************
* ��������  RTC_Get_Week
* ����������������������ڼ�
* ���������������
* ���������������ڼ�
* ����ֵ  �����ں�
*****************************************************/
unsigned char RTC_Get_Week(unsigned short int year,unsigned char month,unsigned char day)
{
    u16 temp2;
	u8 yearH,yearL;
	
	yearH = year / 100;	
    yearL = year % 100; // ���Ϊ21����,�������100 
	if (yearH>19)
        yearL += 100;// ����������ֻ��1900��֮���
	temp2 = yearL+yearL/4;
	temp2 = temp2%7; 
	temp2 = temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)
        temp2--;

	return(temp2%7);
}





