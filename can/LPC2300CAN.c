/****************************************Copyright (c)**************************************************
**                                    ������Զ�������޹�˾
**                                     ��ҵͨ��������ҵ��
**                                http://www.embedcontrol.com
**-------------------------------------�ļ���Ϣ--------------------------------------------------------
**	��   ��  	��:	LPC2300CAN.c
** 	��  ǰ  ��  ��:	v1.0
** 	��		    ��:	2007��5��06��
**	��		    ��:	
**-------------------------------------��ʷ����--------------------------------------------------------
**	��   ��  	��:	LPC2300CAN.c
** 	��  		��:	v1.0
** 	��		    ��:	2007��4��27��
**	��    ��    ��:	������
**	��		    ��:	LPC2300������ʵ��̳�ʵ������
**-------------------------------------��ʷ�޸�--------------------------------------------------------
**	��   ��  	��:	
** 	��  		��:	
** 	��		    ��:	
**	��   ��     ��:	
**	��		    ��:	
********************************************************************************************************/
#include "../can/LPC2300CAN.h"
#include "lpc23xx.h"
#include "../common/type.h"
static struct _MessageDetail MessageDetailT,MessageDetailR;

unsigned char dataAB[8];
unsigned long dataRID;

/*
*********************************************************************************************************
**����ԭ��		:  	unsigned char Enter_SWRst(unsigned char Ch);
**����˵��		:  	Ch:CAN�������ţ�0��ʾ��һ·CAN
**����ֵ		:	0��ʧ��
**				:	1: �ɹ�	
**˵	��		:	CAN���������������λģʽ
*********************************************************************************************************/
unsigned char Enter_SWRst(unsigned char Ch)
{
	unsigned long regaddr;
	REG_CANMOD regmod;
	regaddr = (unsigned long)(&CAN1MOD)+Ch*CANOFFSET;			
	regmod.DWord = RGE(regaddr);		//��ȡCAN1MOD�Ĵ���
	regmod.Bits.RM = 1;					//RMλ�á�1��
	RGE(regaddr) = regmod.DWord;		//��дCAN1MOD�Ĵ���
	regmod.DWord = RGE(regaddr);		//��֤д��ֵ	
	return (0 != regmod.Bits.RM)? 1:0; 
}
/*
*********************************************************************************************************
**����ԭ��		:  	unsigned char Quit_SWRst(unsigned char Ch);
**����˵��		:  	Ch:CAN�������ţ�0��ʾ��һ·CAN
**����ֵ		:	0��ʧ��
**				:	1: �ɹ�	
**˵	��		:	CAN�������˳������λģʽ
*********************************************************************************************************/
unsigned char Quit_SWRst(unsigned char Ch)
{
	unsigned long regaddr;
	REG_CANMOD regmod;
	regaddr = (unsigned long)(&CAN1MOD)+Ch*CANOFFSET;			
	regmod.DWord = RGE(regaddr);		//��ȡCAN1MOD�Ĵ���
	regmod.Bits.RM = 0;					//RMλ�á�0��
	RGE(regaddr) = regmod.DWord;		//��дCAN1MOD�Ĵ���
	regmod.DWord = RGE(regaddr);		//��֤д��ֵ	
	return (0 != regmod.Bits.RM)? 0:1; 
}
/*
*********************************************************************************************************
**����ԭ��		:  	unsigned char CAN_Init(unsigned char Ch, unsigned long Baud);
**����˵��		:  	Ch:CAN�������ţ�0��ʾ��һ·CAN
**				:	Baud:CAN������ֵ
**����ֵ		:	��
**˵	��		:	CAN�������˳������λģʽ
*********************************************************************************************************/
void CAN_Init(unsigned char Ch, unsigned long Baud)
{
	unsigned long regaddr;
	PCONP |= 0x01L<<13;				//��CAN��������Դ
	switch(Ch)						//����CAN����������
	{
		case 0:
			PINSEL0 &= ~(0x03L<<0);/*RD1*/
			PINSEL0 |= (0x01L<<0);
			PINSEL0 &= ~(0x03L<<2);/*TD1*/
			PINSEL0 |= (0x01L<<2);
			break;
		case 1:
			PINSEL1 &= ~(0x03L<<10);/*RD1*/
			PINSEL1 |= (0x03L<<10);
			PINSEL1 &= ~(0x03L<<12);/*TD1*/
			PINSEL1 |= (0x03L<<12);
			break;
		default:
			break;
	}
	Enter_SWRst(Ch);
	regaddr = (unsigned long)(&CAN1BTR)+Ch*CANOFFSET;
	RGE(regaddr) = Baud;
	CAN_AFMR &= ~0x07;	//����Ϊ��·ģʽ
	CAN_AFMR|=(1<<1);
	Quit_SWRst(Ch);
}
/*
*********************************************************************************************************
**����ԭ��		:  	unsigned char CANRCVANDSEND(unsigned char Ch, unsigned char BUFNum);
**����˵��		:  	Ch:CAN�������ţ�0��ʾ��һ·CAN
**				:	BUFNum ѡ�񻺳���
**����ֵ		:	��
**˵	��		:	CAN���յ�����Ϣ�ٷ��ͳ�ȥ 
*********************************************************************************************************/
unsigned char CANRCVANDSEND(unsigned char Ch, unsigned char BUFNum)
{
	unsigned long mes, CAN32reg;
	unsigned long regaddr;
	unsigned char i=5;
	unsigned char i2 = 0;
	BUFNum-=1;		// �����ַ���㡣
	
	
	regaddr = (unsigned long)(&CAN1RFS)+Ch*CANOFFSET;				//����DLC��RTR��FF
	mes = RGE(regaddr);
	mes &= 0Xffff0000;
	regaddr = (unsigned long)(&CAN1TFI1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	
	
	regaddr = (unsigned long)(&CAN1RID)+Ch*CANOFFSET;				//����ID
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TID1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	dataRID = mes;
	
	regaddr = (unsigned long)(&CAN1RDA)+Ch*CANOFFSET;				//���� ����A
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TDA1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	// MessageDetailR.DATAA = mes;
	for (i2 = 0; i2 < 4; i2++) {
		dataAB[i2] = mes >> 8 * i2;
	} 

	regaddr = (unsigned long)(&CAN1RDB)+Ch*CANOFFSET;				//���� ����B
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TDB1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	// MessageDetailR.DATAB = mes;
	for (i2 = 0; i2 < 4; i2++) {
		dataAB[4 + i2] = mes >> 8 * i2;
	} 
		
	regaddr = (unsigned long)(&CAN1GSR)+Ch*CANOFFSET;				//�� ���ջ�����״̬
	mes = RGE(regaddr);
	while(mes&(1<<0))												// RBSΪ1 ���ջ�������
	{
		regaddr = (unsigned long)(&CAN1CMR)+Ch*CANOFFSET;				
		mes=RGE(regaddr);
		mes |= (1<<2);												//�ͷŽ��ջ�����
		RGE(regaddr)=mes;
		if(!(i--))
		{
			return(0);
		}
	}
	
	// regaddr = (unsigned long)(&CAN1CMR)+Ch*CANOFFSET;				
	// mes=RGE(regaddr);
	// mes &= (~(1<<2));												
	// RGE(regaddr)=mes;
	
	return(1);
}
/*
*********************************************************************************************************
**����ԭ��		:  	unsigned char writedetail(unsigned char LEN,unsigned char FF, unsigned int ID,unsigned char *data);
**����˵��		:  	LEN:���ݳ���
**				:	*data �����������ڵ����� 
				:   
**����ֵ		:	�����ɹ����� 1 ʧ�ܷ��� 0
**˵	��		:	 �û���д���� ֡��Ϣ
*********************************************************************************************************/
 unsigned char writedetail(unsigned char LEN,unsigned char FF, unsigned int ID,unsigned char *data)
{
	unsigned char i;
	if(LEN>8)
	{
		return(0);
	}
	else
	{
		MessageDetailT.LEN=LEN;
		MessageDetailT.FF=FF;
		MessageDetailT.CANID=ID;
		MessageDetailT.DATAA=0;			//������
		MessageDetailT.DATAB=0;
	
		for(i=0;i<LEN;i++)
		{
			if(i<4)
			{
				MessageDetailT.DATAA |= (*(data+i)<<(i*8)); 
			}
			else
			{
				MessageDetailT.DATAB |= (*(data+i)<<((i-4)*8));
			}
		}
	//MessageDetailT.DATAB = ID;	
	return(1);
	}
}
/*
*********************************************************************************************************
**����ԭ��		:  	unsigned char CANSend(unsigned char Ch, unsigned char BUFNum);
**����˵��		:  	Ch:CAN�������ţ�0��ʾ��һ·CAN
**				:	BUFNum ѡ�񻺳���
				:   MessageDetailT ���ĵ�������Ϣ������
**����ֵ		:	��
**˵	��		:	CAN�������˳������λģʽ
*********************************************************************************************************/
unsigned char CANSend(unsigned char Ch, unsigned char BUFNum)
{
	unsigned long CAN32reg;
	unsigned long regaddr;
	unsigned char FFflag;
	
	BUFNum-=1;		// �����ַ���㡣
	
	
	regaddr = (unsigned long)(&CAN1TFI1)+Ch*CANOFFSET+0X10*BUFNum;	// 3���������ַ�� 0x10
	CAN32reg = RGE(regaddr);
	CAN32reg &= ~((0x0fL<<16) | (0x01L<<30) | (0x80000000));        //�� DLC,RTR.FFλ�
	CAN32reg |= ((unsigned long)MessageDetailT.LEN<<16) | ((unsigned long)MessageDetailT.FF<<31);
	RGE(regaddr)=CAN32reg;
	
	FFflag = MessageDetailT.FF;
	
	regaddr = (unsigned long)(&CAN1TID1)+Ch*CANOFFSET+0X10*BUFNum;	//д֡ID
	CAN32reg = RGE(regaddr);
	if(FFflag)														//FFΪ1��IDΪ29λ
	{
		CAN32reg &=0x70000000;
		CAN32reg |= (MessageDetailT.CANID & 0x1fffffff);
	}
	else
	{
		CAN32reg &= 0xfffff800;										//FFΪ0 ��IDΪ11λ
		CAN32reg |= (MessageDetailT.CANID & 0x000007ff);
	}
	RGE(regaddr)=CAN32reg;
	
	regaddr = (unsigned long)(&CAN1TDA1)+Ch*CANOFFSET+0X10*BUFNum;	// д֡����A
	RGE(regaddr) = MessageDetailT.DATAA;
	
	regaddr = (unsigned long)(&CAN1TDB1)+Ch*CANOFFSET+0X10*BUFNum;	// д֡����B
	RGE(regaddr) = MessageDetailT.DATAB;
	
	regaddr = (unsigned long)(&CAN1CMR)+Ch*CANOFFSET;				//д���ƼĴ���������
	CAN32reg = RGE(regaddr);
	CAN32reg &= ~0x03;
	CAN32reg |= 0x03;
	CAN32reg &= ~((1<<5)|(1<<6)|(1<<7));
	switch(BUFNum)
	{
		case 0: CAN32reg |= 01<<5;break;
		case 1: CAN32reg |= 01<<6;break;
		case 2: CAN32reg |= 01<<7;break;
		default: break;
	}
	RGE(regaddr)=CAN32reg;
	
	
	regaddr = (unsigned long)(&CAN1GSR)+Ch*CANOFFSET;				//��ѯ����״̬
	CAN32reg = RGE(regaddr);
	/*/if(CAN32reg&(1<<5))											//�ȴ���������ķ����Ѿ����
	{
		;
	}	*/	
	if(CAN32reg&(1<<3))			//���е������ѳɹ����
	{
		
		return(1);				//���ͳɹ����� 1
	}	
	else { return (0);}			//����ʧ�ܷ��� 0 
	
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
