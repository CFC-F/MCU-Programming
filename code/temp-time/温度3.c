#include "reg51.h"
#include "temp-ds1302.h"

uint flag = 0;					// 时间小数点闪烁计数
uint temp;						// 温度变量
uint DISP_FLAG = 1;			    // 功能显示标志位

/*********DS18B20初始化命令函数***********************/
uchar DS18b20Init(void)					// send reset and initialization command
{
  uint i;
  DS = 0;										// 先将端口拉低
  i=120;										// 维持低电平状态480us~960us
  while(i>0)i--;
  DS = 1;										// 然后释放总线(将总线拉高),若DS18B20做出反应,将会将在15us~60us后将总线拉低	 // 等待DS18B20响应
	  i = 0;
		while(DS)									// 在DS高电平时等待
		{
			i++;
			if(i > 30000)						// 等待时间大于5ms
			{
				return 0;						//说明响应失败
			}
		}
		return 1;
}


/****************DS18B20读数据函数*********************/

uchar DS18b20ReadByte(void)
{
	uint i;
	uchar j,dat;
    uchar byte = 0;
	for(j=0;j<8;j++)				// 循环 8 次,调用tmpreadbit函数,读 8bit 数据
    {
	    DS = 0;										// 先将端口拉低
		_nop_();									// 延时 2us ,要求至少保持1us
		DS = 1;										// 再将端口拉高
		i=8;while(i>0)i--;				            // 等待DS数据稳定,要求的至少延时15us以上
		dat = DS;									// 数据传输
		i=15;while(i>0)i--;							// 等待数据采样周期完成,要求不低于60us				
		byte = (byte>>1) | (dat<<7);	     		    // 效果是 j 先读入的数据,放到了dat的低位,读8次正好是1byte. 
    }
  return(byte);		
}


//bit tmpreadbit(void)				// read a bit data
//{
//	uint i;
//	bit dat;									// 定义位数据 (dat = 0 or 1)
//	DS = 0;										// 先将端口拉低
//	_nop_();									// 延时 2us ,要求至少保持1us
//	DS = 1;										// 再将端口拉高
//	i=8;while(i>0)i--;				// 等待DS数据稳定,要求的至少延时15us以上
//	dat = DS;									// 数据传输
//	i=15;while(i>0)i--;				// 等待数据采样周期完成,要求不低于60us
//	return(dat);
//}
//
//uchar DS18b20ReadByte(void)					// read a byte date
//{
//	uchar i,j,dat;
//  dat = 0;									// 初始化数据变量为 0
//  for(i=1;i<=8;i++)					// 循环 8 次,调用tmpreadbit函数,读 8bit 数据
//  {
//    j = tmpreadbit();				// 读出的数据暂存到 j
//    dat = (j<<7)|(dat>>1);	// 效果是 j 先读入的数据,放到了dat的低位,读8次正好是1byte.
//  }
//  return(dat);							// 返回读到的字节
//}

/***************DS18B20写入数据函数*******************/
void DS18b20WriteByte(uchar dat)   //write a byte to ds18b20
{
  uint i;
  uchar j;
  bit testb;
  for(j=0;j<8;j++)
  {
    testb = dat&0x01;
    dat = dat>>1;
    if(testb)     						// write 1
    {
      DS=0;
      i=8;while(i>0)i--;;			// 延时要求15~60us内
      DS=1;
      i=15;while(i>0)i--;			// 要求不低于60us
    }
    else
    {
      DS = 0;       					// write 0
      i=15;while(i>0)i--;
      DS = 1;
      i++;i++;
    }
  }
//	uint i, j;
//
//	for(j=0; j<8; j++)
//	{
//		DS = 0;	     	  //每写入一位数据之前先把总线拉低1us
//		i++;
//		DS = dat & 0x01;  //然后写入一个数据，从最低位开始
//		i=6;
//		while(i--); //延时68us，持续时间最少60us
//		DS = 1;	//然后释放总线，至少1us给总线恢复时间才能接着写入第二个数值
//		dat >>= 1;
//	}
}

/************DS18B20温度转换初始化函数********************/
void  DS18b20ChangTemp(void)					// DS18B20 begin change
{
  while(DS18b20Init() == 0)
	{					
		SendByte_74HC164(0);
		P1 &= 0xfB;
	}
  delay(1);
  DS18b20WriteByte(0xCC);					// 跳过 ROM 操作
  DS18b20WriteByte(0x44);					// 启动一次温度转换
}

void DS18b20ReadTempCom(void)										// get the temperature
{

  while(DS18b20Init() == 0)
	{
		SendByte_74HC164(0);
		P1 &= 0xfB;
	}
  delay(1);
  DS18b20WriteByte(0xCC);					// 跳过 ROM 操作
  DS18b20WriteByte(0xBE);					// 读暂存寄存器
}

/************DS18B20温度转换函数********************/
uint tmp()
{  
  uint temp = 0;
  float tp=0;
  uchar high,low;
  DS18b20ChangTemp();
  DS18b20ReadTempCom();
  low=DS18b20ReadByte();
  high=DS18b20ReadByte();
  temp=high;
  temp<<=8;										// two byte compose a int variable
  temp|=low;
  	
	if(temp< 0)				//当温度值为负数
	{ 
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
	}
	else
	{			
		tp=temp;
		temp=temp*0.0625*100+0.5;	
	}
	return temp;
}

