#include "reg51.h"
#include "temp-ds1302.h"

#define uchar unsigned char
#define uint unsigned int

uchar code table1[]={0x81,0xD7,0xC8,0xC2,0x96,0xA2,0xA0,0xC7,0x80,0x82,0xA9};
uchar num=0;
/*****************低精度延时函数*********************/
void delay(unsigned int mstime)
{
	int i,j;
	for(i=mstime; i>=0; i--)
		for(j=114; j>=0; j--);
}


/***************精确延时50ms函数*********************/
void Delay50ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 3;
	j = 26;
	k = 223;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



/********************74HC164发送Byte数据函数**************/
void SendByte_74HC164(uchar byte)
{
	uchar num,c;
	num=table1[byte];
//	for(c=0; c<8; c++)
//	{
//		DAT=num>>7;		
//		CLK=0;			// 制造一个上升沿
//		_nop_();					
//		CLK=1;
//		num=num<<1;	
//	}
	for(c=0; c<8; c++)
	{
		DAT=num&0x01;		// P3^0 --> 0000 000x
		CLK=0;					// 制造一个上升沿
		CLK=1;
		num>>=1;				// 将数据发送到寄存器
	}
}


/**************数码管温度数据显示函数******************/
void Display_Temp(uint temp)				// 显示程序
{
	uchar disp1,disp2,disp3,disp4;
	disp1=temp/1000;							
	disp2=(temp%1000)/100;						
	disp3=(temp%100)/10;						
	disp4=temp%10;
	
	DP = 0;										// 温度小数点开启
   
	// 控制数码管显示温度

	P1 |= 0x0f;		   //消隐
	SendByte_74HC164(disp1);
	P1 &= 0xf7;			//1111 0111
	delay(6);

	P1 |= 0x0f;		  //消隐
	SendByte_74HC164(disp2);
	P1 &= 0xfb;		 //1111 1011
	delay(6);
	
	P1 |= 0x0f;		  //消隐
	SendByte_74HC164(disp3);
	P1 &= 0xfd;		 //1111 1101
	delay(6);

	P1 |= 0x0f;		   //消隐
	SendByte_74HC164(disp4);
	P1 &= 0xfE;		   //1111 1110 
	delay(6);
	
	P1 |= 0x0f;
}


/******************显示时间*******************/
void Display_Time()							// 显示程序
{
	uchar disp0,disp1,disp2,disp3,disp4,disp5,disp6,disp7,disp8,disp9;
	disp0 = (time[0])/16;				
	disp1 = (time[0])%16;				
	disp2 = (time[1])/16;				
	disp3 = (time[1])%16;				
	disp4 = (time[2])/16;				
	disp5 = (time[2])%16;				
	disp6 = (time[3])/16;				
	disp7 = (time[3])%16;				
	disp8 = (time[4])/16;				
	disp9 = (time[4])%16;				
	

//	flag ++;											// 时间点闪烁计数
//	if(flag >= 132)
//	{
//		DP = ~DP;
//		flag = 0;
//	}

   
	// 控制数码管显示时间
	P1 |= 0x0f;
	SendByte_74HC164(disp4);				
	P1 &= 0xf7;
	delay(6);

	P1 |= 0x0f;
	SendByte_74HC164(disp5);				
	P1 &= 0xfB;
	delay(6);

	P1 |= 0x0f;							
	SendByte_74HC164(disp2);				
	P1 &= 0xfd;
	delay(6);
	
	P1 |= 0x0f;
	SendByte_74HC164(disp3);				
	P1 &= 0xfe;
	delay(6);

	P1 |= 0x0f;
}

/*************定时器初始化****************/
void Timer0()
{
	
	TMOD |= 0x01;		 //定时器0   方式1   16位重装
	TH0=0x4c;
	TL0=0x00;
	EA=1;
	ET0=1;
	TR0=1;
}

/**************中断初始化*****************/
void configExtInt0()
{
	IT0 = 1;	//选择为下降沿触发方式(为1代表下降沿触发，为0代表低电平触发)
	EX0 = 1;	//使能外部中断0
	EA = 1;  	//使能总中断
}

/***************外部中断*****************/
void extInt0() interrupt 0
{
	EA = 0;							// 关闭总中断
	Delay50ms();				// 延时消抖
	if(KEY == 0)
	{
		P1 |= 0x0f;				    // 关闭位选,清屏
		DISP_FLAG = ~DISP_FLAG;		// 显示其他功能
	}
	EA = 1;							// 开启总中断
}

/****************主函数*******************/
void main()
{
	uchar a;
	Timer0();
	configExtInt0();									// 配置外部中断0
//	DS1302Init();										// DS1302日期初始化
	while(1)
	{
		if(DISP_FLAG == 1)							// 时间显示
		{
			DS1302ReadTime();						// DS1302读取当前时间

			Display_Time();								// 显示当前时间
		}
		else														// 温度显示
		{
				DS18b20ChangTemp();					// 温度转换
				for(a=40;a>0;a--)						// 延时,保持连续显示
				{
					Display_Temp(tmp());				// 进行温度转换和数值显示
				}
		}
	}
}

void Timert0() interrupt 1
{ 
	TH0=0x4c;
	TL0=0x00;
	num++;			//50ms计数
	if(num==10)		//计数20次 1s
	{
		num=0;
		DP = ~DP;
	}
}
