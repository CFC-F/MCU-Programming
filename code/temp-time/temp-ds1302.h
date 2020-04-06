#ifndef __TEMP_H_
#define __TEMP_H_
#endif
#ifndef __DS1302_H_
#define __DS1302_H_
#endif

//---头文件---//
#include <reg51.h>
#include <intrins.h>

//---定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

#define dec(dat) ((dat>>4)*10+(dat&0x0f)) 		// BCD码转十进制

//--定义使用的IO口--//
sbit IO = P1^4;					// DS1302 数据输入输出线
sbit SCLK = P1^5;				// DS1302 时钟信号线
sbit RST = P1^6;				// DS1302 复位信号线
sbit DS = P1^7;					// DS18B20 数据端口
sbit DAT = P3^0;				// 74HC164 数据输入端口
sbit CLK = P3^1;				// 74HC164 时钟输入端口
sbit DP = P3^7;					// 时间分割点
sbit DP1 = P3^5;				// 温度小数点
sbit KEY = P3^2;				// 按键INT0

//---定义全局函数---//
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


//---加入全局变量--//

extern uchar code write_addr[];	// DS1302写地址
extern uchar code read_addr[];		// DS1302读地址

// 定义数码管段选(不加小数点0~9、℃)
extern uchar code table1[];

// 存储顺序是秒分时日月周年,存储格式是用BCD码
extern uchar time[7];  //初始化的时间为2019年10月4日18:55:00

extern uint flag;					// 时间小数点闪烁计数
extern uint temp;					// 温度变量
extern uint DISP_FLAG;			    // 功能显示标志位
extern uchar num;