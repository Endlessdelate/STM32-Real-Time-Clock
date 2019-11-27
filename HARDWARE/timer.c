#include "include.h"

extern vu16 USART3_RX_STA;

//定时器7中断服务程序		    
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART3_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIM7更新中断标志    
		TIM_Cmd(TIM7, DISABLE);  //关闭TIM7 
	}	    
}
 
//通用定时器7中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz 
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7时钟使能    
	
	//定时器TIM7初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
	
	TIM_Cmd(TIM7,ENABLE);//开启定时器7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}
//arr：自动重装值
//psc：时钟预分频系数
//Tout= ((arr+1)*(psc+1 ))/Tclk；
//溢出时间us               输入时钟频率

/*---通用定时器4初始化---*/
void TIM4_Int_Init(unsigned int arr,unsigned int psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//TIM 4时钟使能
    
    TIM_TimeBaseInitStructure.TIM_Period = arr;//设置自动重装载寄存器周期的值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;//设置时钟频率除数的预分频值
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;//设置时间分割
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//定时器向上计数
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//初始化tim4
    
    TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);//允许更新中断
    
//    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//    TIM_Cmd(TIM4,ENABLE);
}

/*---定时器4中断服务函数---*/
//void TIM4_IRQHandler(void)
//{
//    if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)
//    {
//        TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
//        PBout(5) = ~PBout(5);
//        //Show_RTC_time();
//    }
//}
