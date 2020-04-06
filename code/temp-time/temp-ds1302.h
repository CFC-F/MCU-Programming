#ifndef __TEMP_H_
#define __TEMP_H_
#endif
#ifndef __DS1302_H_
#define __DS1302_H_
#endif

//---ͷ�ļ�---//
#include <reg51.h>
#include <intrins.h>

//---����ؼ���---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

#define dec(dat) ((dat>>4)*10+(dat&0x0f)) 		// BCD��תʮ����

//--����ʹ�õ�IO��--//
sbit IO = P1^4;					// DS1302 �������������
sbit SCLK = P1^5;				// DS1302 ʱ���ź���
sbit RST = P1^6;				// DS1302 ��λ�ź���
sbit DS = P1^7;					// DS18B20 ���ݶ˿�
sbit DAT = P3^0;				// 74HC164 ��������˿�
sbit CLK = P3^1;				// 74HC164 ʱ������˿�
sbit DP = P3^7;					// ʱ��ָ��
sbit DP1 = P3^5;				// �¶�С����
sbit KEY = P3^2;				// ����INT0

//---����ȫ�ֺ���---//
void DS1302Write(uchar addr, uchar dat);
uchar DS1302Read(uchar addr);
void DS1302Init();
void DS1302ReadTime();
void Delay1ms(uint);
uchar DS18b20Init();
void DS18b20WriteByte(uchar com);
uchar DS18b20ReadByte();
void  DS18b20ChangTemp();
void  DS18b20ReadTempCom();
int DS18b20ReadTemp();
void SendByte_74HC164(uchar byte);
void delay(unsigned int mstime);
void Delay50ms();
uint tmp();
bit tmpreadbit(void);
void Timer0();


//---����ȫ�ֱ���--//

extern uchar code write_addr[];	// DS1302д��ַ
extern uchar code read_addr[];		// DS1302����ַ

// ��������ܶ�ѡ(����С����0~9����)
extern uchar code table1[];

// �洢˳�������ʱ��������,�洢��ʽ����BCD��
extern uchar time[7];  //��ʼ����ʱ��Ϊ2019��10��4��18:55:00

extern uint flag;					// ʱ��С������˸����
extern uint temp;					// �¶ȱ���
extern uint DISP_FLAG;			    // ������ʾ��־λ
extern uchar num;