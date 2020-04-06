#include "reg51.h"
#include "temp-ds1302.h"

uchar code write_addr[]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};	// DS1302写地址
uchar code read_addr[]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};		// DS1302读地址

uchar time[7]={0,0x50,0x08,0x06,0x12,0x05,0x19};

/**************DS1302初始化函数**********/ 
void DS1302Init()
{
   uchar k;
   DS1302Write(0x8e,0x00);  		// 禁止写保护，即允许数据写入
   for(k=0;k<7;k++)							// 写入7个字节的时钟信号：秒分时日月周年
   {
     DS1302Write(write_addr[k],time[k]);
   }
   DS1302Write(0x8e,0x80);  		// 打开写保护,禁止数据写入
}

/***************** 函数名称：DS1302读操作函数********************/ 
uchar DS1302Read(uchar addr)
{
	uchar a, rdata=0;
	RST = 0;											// 拉低RST引脚,终止数据传输
	SCLK = 0;											// 拉低SCLK引脚,清零时钟线
	RST = 1;											// 拉高RST引脚,启动控制逻辑
	//发送控制字节
	for(a=0; a<8; a++)
	{
		SCLK = 0;
		IO = addr & 0x01;
		SCLK = 1;										// 制造一个上升沿,写入地址
		addr >>= 1;
	}
	//读1字节数据
	for(a=0; a<8; a++)
	{
		SCLK = 1;
		rdata >>= 1;
		SCLK = 0;										// 制造一个下降沿,读取数据
		if(IO)
		{														// 如果读到1
			rdata |= 0x80;						// 把最高位置为1,记录到rdata中
		}
	}
	RST=0;												// 拉低RST
	return rdata;
}

/*****************DS1302写操作函数***************************/
void DS1302Write(uchar addr,uchar wdata)
{
	uchar a;
	RST=0;									// 拉低RST引脚,终止数据传输
	SCLK=0;									// 拉低SCLK引脚,清零时钟线
	RST=1;									// 拉高RST引脚,所有数据传输都要拉高RST脚,启动控制逻辑
	//写入控制字节
	for(a=0; a<8; a++)
	{
			IO= addr & 0x01;			// IO引脚准备好要写入的1位数据
			SCLK=1;							// SCLK上升沿,1位数据从IO脚写入,低位先写入
			addr >>=1;						// 数据右移1位
			SCLK=0;							// 拉低SCLK,为下次写入准备,循环8次写入1字节
	}
	//写入数据字节
	for(a=0; a<8; a++)
	{
			IO= wdata & 0x01;
			SCLK=1;
			wdata>>=1;
			SCLK=0;
	}
	RST=0;									// 数据传输完拉低RST
}  

/*************DS1302数据读取函数***********************/ 
void DS1302ReadTime()
{
   uchar n;
   for(n=0;n<7;n++)
    time[n] = DS1302Read(read_addr[n]);   //读取秒分时日月周年
}

