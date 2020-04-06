#include "reg51.h"
#include "temp-ds1302.h"

uchar code write_addr[]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};	// DS1302д��ַ
uchar code read_addr[]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};		// DS1302����ַ

uchar time[7]={0,0x50,0x08,0x06,0x12,0x05,0x19};

/**************DS1302��ʼ������**********/ 
void DS1302Init()
{
   uchar k;
   DS1302Write(0x8e,0x00);  		// ��ֹд����������������д��
   for(k=0;k<7;k++)							// д��7���ֽڵ�ʱ���źţ����ʱ��������
   {
     DS1302Write(write_addr[k],time[k]);
   }
   DS1302Write(0x8e,0x80);  		// ��д����,��ֹ����д��
}

/***************** �������ƣ�DS1302����������********************/ 
uchar DS1302Read(uchar addr)
{
	uchar a, rdata=0;
	RST = 0;											// ����RST����,��ֹ���ݴ���
	SCLK = 0;											// ����SCLK����,����ʱ����
	RST = 1;											// ����RST����,���������߼�
	//���Ϳ����ֽ�
	for(a=0; a<8; a++)
	{
		SCLK = 0;
		IO = addr & 0x01;
		SCLK = 1;										// ����һ��������,д���ַ
		addr >>= 1;
	}
	//��1�ֽ�����
	for(a=0; a<8; a++)
	{
		SCLK = 1;
		rdata >>= 1;
		SCLK = 0;										// ����һ���½���,��ȡ����
		if(IO)
		{														// �������1
			rdata |= 0x80;						// �����λ��Ϊ1,��¼��rdata��
		}
	}
	RST=0;												// ����RST
	return rdata;
}

/*****************DS1302д��������***************************/
void DS1302Write(uchar addr,uchar wdata)
{
	uchar a;
	RST=0;									// ����RST����,��ֹ���ݴ���
	SCLK=0;									// ����SCLK����,����ʱ����
	RST=1;									// ����RST����,�������ݴ��䶼Ҫ����RST��,���������߼�
	//д������ֽ�
	for(a=0; a<8; a++)
	{
			IO= addr & 0x01;			// IO����׼����Ҫд���1λ����
			SCLK=1;							// SCLK������,1λ���ݴ�IO��д��,��λ��д��
			addr >>=1;						// ��������1λ
			SCLK=0;							// ����SCLK,Ϊ�´�д��׼��,ѭ��8��д��1�ֽ�
	}
	//д�������ֽ�
	for(a=0; a<8; a++)
	{
			IO= wdata & 0x01;
			SCLK=1;
			wdata>>=1;
			SCLK=0;
	}
	RST=0;									// ���ݴ���������RST
}  

/*************DS1302���ݶ�ȡ����***********************/ 
void DS1302ReadTime()
{
   uchar n;
   for(n=0;n<7;n++)
    time[n] = DS1302Read(read_addr[n]);   //��ȡ���ʱ��������
}

