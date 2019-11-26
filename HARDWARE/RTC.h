#ifndef _RTC_H_
#define _RTC_H_

//时间结构体
typedef struct
{
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned short int year;
    unsigned char month;
    unsigned char date;
    unsigned char week;
}_calendar_obj;

extern _calendar_obj calendar;
extern const unsigned char mon_table[12];//平年的月份日期表

unsigned char RTC_Init(void);
unsigned char Is_Leap_Year(unsigned short int year);
unsigned char RTC_Set(unsigned int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec);
unsigned char RTC_Alarm_Set(unsigned int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec);
unsigned char RTC_Get(void);
unsigned char RTC_Get_Week(unsigned short int year,unsigned char month,unsigned char day);
void Set_RTC_with_UTC_time(unsigned int UTC_time);


#endif
