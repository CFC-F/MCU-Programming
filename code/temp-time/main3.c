#include "reg51.h"
#include "temp-ds1302.h"

#define uchar unsigned char
#define uint unsigned int

uchar code table1[]={0x81,0xD7,0xC8,0xC2,0x96,0xA2,0xA0,0xC7,0x80,0x82,0xA9};
uchar num=0;
/*****************�;�����ʱ����*********************/
void delay(unsigned int mstime)
{
	int i,j;
	for(i=mstime; i>=0; i--)
		for(j=114; j>=0; j--);
}


/***************��ȷ��ʱ50ms����*********************/
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



/********************74HC164����Byte���ݺ���**************/
void SendByte_74HC164(uchar byte)
{
	uchar num,c;
	num=table1[byte];
//	for(c=0; c<8; c++)
//	{
//		DAT=num>>7;		
//		CLK=0;			// ����һ��������
//		_nop_();					
//		CLK=1;
//		num=num<<1;	
//	}
	for(c=0; c<8; c++)
	{
		DAT=num&0x01;		// P3^0 --> 0000 000x
		CLK=0;					// ����һ��������
		CLK=1;
		num>>=1;				// �����ݷ��͵��Ĵ���
	}
}


/**************������¶�������ʾ����******************/
void Display_Temp(uint temp)				// ��ʾ����
{
	uchar disp1,disp2,disp3,disp4;
	disp1=temp/1000;							
	disp2=(temp%1000)/100;						
	disp3=(temp%100)/10;						
	disp4=temp%10;
	
	DP = 0;										// �¶�С���㿪��
   
	// �����������ʾ�¶�

	P1 |= 0x0f;		   //����
	SendByte_74HC164(disp1);
	P1 &= 0xf7;			//1111 0111
	delay(6);

	P1 |= 0x0f;		  //����
	SendByte_74HC164(disp2);
	P1 &= 0xfb;		 //1111 1011
	delay(6);
	
	P1 |= 0x0f;		  //����
	SendByte_74HC164(disp3);
	P1 &= 0xfd;		 //1111 1101
	delay(6);

	P1 |= 0x0f;		   //����
	SendByte_74HC164(disp4);
	P1 &= 0xfE;		   //1111 1110 
	delay(6);
	
	P1 |= 0x0f;
}


/******************��ʾʱ��*******************/
void Display_Time()							// ��ʾ����
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
	

//	flag ++;											// ʱ�����˸����
//	if(flag >= 132)
//	{
//		DP = ~DP;
//		flag = 0;
//	}

   
	// �����������ʾʱ��
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

/*************��ʱ����ʼ��****************/
void Timer0()
{
	
	TMOD |= 0x01;		 //��ʱ��0   ��ʽ1   16λ��װ
	TH0=0x4c;
	TL0=0x00;
	EA=1;
	ET0=1;
	TR0=1;
}

/**************�жϳ�ʼ��*****************/
void configExtInt0()
{
	IT0 = 1;	//ѡ��Ϊ�½��ش�����ʽ(Ϊ1�����½��ش�����Ϊ0����͵�ƽ����)
	EX0 = 1;	//ʹ���ⲿ�ж�0
	EA = 1;  	//ʹ�����ж�
}

/***************�ⲿ�ж�*****************/
void extInt0() interrupt 0
{
	EA = 0;							// �ر����ж�
	Delay50ms();				// ��ʱ����
	if(KEY == 0)
	{
		P1 |= 0x0f;				    // �ر�λѡ,����
		DISP_FLAG = ~DISP_FLAG;		// ��ʾ��������
	}
	EA = 1;							// �������ж�
}

/****************������*******************/
void main()
{
	uchar a;
	Timer0();
	configExtInt0();									// �����ⲿ�ж�0
//	DS1302Init();										// DS1302���ڳ�ʼ��
	while(1)
	{
		if(DISP_FLAG == 1)							// ʱ����ʾ
		{
			DS1302ReadTime();						// DS1302��ȡ��ǰʱ��

			Display_Time();								// ��ʾ��ǰʱ��
		}
		else														// �¶���ʾ
		{
				DS18b20ChangTemp();					// �¶�ת��
				for(a=40;a>0;a--)						// ��ʱ,����������ʾ
				{
					Display_Temp(tmp());				// �����¶�ת������ֵ��ʾ
				}
		}
	}
}

void Timert0() interrupt 1
{ 
	TH0=0x4c;
	TL0=0x00;
	num++;			//50ms����
	if(num==10)		//����20�� 1s
	{
		num=0;
		DP = ~DP;
	}
}
