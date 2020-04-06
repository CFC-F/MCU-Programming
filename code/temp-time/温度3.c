#include "reg51.h"
#include "temp-ds1302.h"

uint flag = 0;					// ʱ��С������˸����
uint temp;						// �¶ȱ���
uint DISP_FLAG = 1;			    // ������ʾ��־λ

/*********DS18B20��ʼ�������***********************/
uchar DS18b20Init(void)					// send reset and initialization command
{
  uint i;
  DS = 0;										// �Ƚ��˿�����
  i=120;										// ά�ֵ͵�ƽ״̬480us~960us
  while(i>0)i--;
  DS = 1;										// Ȼ���ͷ�����(����������),��DS18B20������Ӧ,���Ὣ��15us~60us����������	 // �ȴ�DS18B20��Ӧ
	  i = 0;
		while(DS)									// ��DS�ߵ�ƽʱ�ȴ�
		{
			i++;
			if(i > 30000)						// �ȴ�ʱ�����5ms
			{
				return 0;						//˵����Ӧʧ��
			}
		}
		return 1;
}


/****************DS18B20�����ݺ���*********************/

uchar DS18b20ReadByte(void)
{
	uint i;
	uchar j,dat;
    uchar byte = 0;
	for(j=0;j<8;j++)				// ѭ�� 8 ��,����tmpreadbit����,�� 8bit ����
    {
	    DS = 0;										// �Ƚ��˿�����
		_nop_();									// ��ʱ 2us ,Ҫ�����ٱ���1us
		DS = 1;										// �ٽ��˿�����
		i=8;while(i>0)i--;				            // �ȴ�DS�����ȶ�,Ҫ���������ʱ15us����
		dat = DS;									// ���ݴ���
		i=15;while(i>0)i--;							// �ȴ����ݲ����������,Ҫ�󲻵���60us				
		byte = (byte>>1) | (dat<<7);	     		    // Ч���� j �ȶ��������,�ŵ���dat�ĵ�λ,��8��������1byte. 
    }
  return(byte);		
}


//bit tmpreadbit(void)				// read a bit data
//{
//	uint i;
//	bit dat;									// ����λ���� (dat = 0 or 1)
//	DS = 0;										// �Ƚ��˿�����
//	_nop_();									// ��ʱ 2us ,Ҫ�����ٱ���1us
//	DS = 1;										// �ٽ��˿�����
//	i=8;while(i>0)i--;				// �ȴ�DS�����ȶ�,Ҫ���������ʱ15us����
//	dat = DS;									// ���ݴ���
//	i=15;while(i>0)i--;				// �ȴ����ݲ����������,Ҫ�󲻵���60us
//	return(dat);
//}
//
//uchar DS18b20ReadByte(void)					// read a byte date
//{
//	uchar i,j,dat;
//  dat = 0;									// ��ʼ�����ݱ���Ϊ 0
//  for(i=1;i<=8;i++)					// ѭ�� 8 ��,����tmpreadbit����,�� 8bit ����
//  {
//    j = tmpreadbit();				// �����������ݴ浽 j
//    dat = (j<<7)|(dat>>1);	// Ч���� j �ȶ��������,�ŵ���dat�ĵ�λ,��8��������1byte.
//  }
//  return(dat);							// ���ض������ֽ�
//}

/***************DS18B20д�����ݺ���*******************/
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
      i=8;while(i>0)i--;;			// ��ʱҪ��15~60us��
      DS=1;
      i=15;while(i>0)i--;			// Ҫ�󲻵���60us
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
//		DS = 0;	     	  //ÿд��һλ����֮ǰ�Ȱ���������1us
//		i++;
//		DS = dat & 0x01;  //Ȼ��д��һ�����ݣ������λ��ʼ
//		i=6;
//		while(i--); //��ʱ68us������ʱ������60us
//		DS = 1;	//Ȼ���ͷ����ߣ�����1us�����߻ָ�ʱ����ܽ���д��ڶ�����ֵ
//		dat >>= 1;
//	}
}

/************DS18B20�¶�ת����ʼ������********************/
void  DS18b20ChangTemp(void)					// DS18B20 begin change
{
  while(DS18b20Init() == 0)
	{					
		SendByte_74HC164(0);
		P1 &= 0xfB;
	}
  delay(1);
  DS18b20WriteByte(0xCC);					// ���� ROM ����
  DS18b20WriteByte(0x44);					// ����һ���¶�ת��
}

void DS18b20ReadTempCom(void)										// get the temperature
{

  while(DS18b20Init() == 0)
	{
		SendByte_74HC164(0);
		P1 &= 0xfB;
	}
  delay(1);
  DS18b20WriteByte(0xCC);					// ���� ROM ����
  DS18b20WriteByte(0xBE);					// ���ݴ�Ĵ���
}

/************DS18B20�¶�ת������********************/
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
  	
	if(temp< 0)				//���¶�ֵΪ����
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

