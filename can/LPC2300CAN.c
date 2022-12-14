/****************************************Copyright (c)**************************************************
**                                    广州致远电子有限公司
**                                     工业通信网络事业部
**                                http://www.embedcontrol.com
**-------------------------------------文件信息--------------------------------------------------------
**	文   件  	名:	LPC2300CAN.c
** 	当  前  版  本:	v1.0
** 	日		    期:	2007年5月06日
**	描		    述:	
**-------------------------------------历史创建--------------------------------------------------------
**	文   件  	名:	LPC2300CAN.c
** 	版  		本:	v1.0
** 	日		    期:	2007年4月27日
**	创    建    人:	滕欣欣
**	描		    述:	LPC2300开发板实验教程实例代码
**-------------------------------------历史修改--------------------------------------------------------
**	文   件  	名:	
** 	版  		本:	
** 	日		    期:	
**	修   改     人:	
**	描		    述:	
********************************************************************************************************/
#include "../can/LPC2300CAN.h"
#include "lpc23xx.h"
#include "../common/type.h"
static struct _MessageDetail MessageDetailT,MessageDetailR;

unsigned char dataAB[8];
unsigned long dataRID;

/*
*********************************************************************************************************
**函数原型		:  	unsigned char Enter_SWRst(unsigned char Ch);
**参数说明		:  	Ch:CAN控制器号，0表示第一路CAN
**返回值		:	0：失败
**				:	1: 成功	
**说	明		:	CAN控制器进入软件复位模式
*********************************************************************************************************/
unsigned char Enter_SWRst(unsigned char Ch)
{
	unsigned long regaddr;
	REG_CANMOD regmod;
	regaddr = (unsigned long)(&CAN1MOD)+Ch*CANOFFSET;			
	regmod.DWord = RGE(regaddr);		//读取CAN1MOD寄存器
	regmod.Bits.RM = 1;					//RM位置“1”
	RGE(regaddr) = regmod.DWord;		//回写CAN1MOD寄存器
	regmod.DWord = RGE(regaddr);		//验证写入值	
	return (0 != regmod.Bits.RM)? 1:0; 
}
/*
*********************************************************************************************************
**函数原型		:  	unsigned char Quit_SWRst(unsigned char Ch);
**参数说明		:  	Ch:CAN控制器号，0表示第一路CAN
**返回值		:	0：失败
**				:	1: 成功	
**说	明		:	CAN控制器退出软件复位模式
*********************************************************************************************************/
unsigned char Quit_SWRst(unsigned char Ch)
{
	unsigned long regaddr;
	REG_CANMOD regmod;
	regaddr = (unsigned long)(&CAN1MOD)+Ch*CANOFFSET;			
	regmod.DWord = RGE(regaddr);		//读取CAN1MOD寄存器
	regmod.Bits.RM = 0;					//RM位置“0”
	RGE(regaddr) = regmod.DWord;		//回写CAN1MOD寄存器
	regmod.DWord = RGE(regaddr);		//验证写入值	
	return (0 != regmod.Bits.RM)? 0:1; 
}
/*
*********************************************************************************************************
**函数原型		:  	unsigned char CAN_Init(unsigned char Ch, unsigned long Baud);
**参数说明		:  	Ch:CAN控制器号，0表示第一路CAN
**				:	Baud:CAN波特率值
**返回值		:	无
**说	明		:	CAN控制器退出软件复位模式
*********************************************************************************************************/
void CAN_Init(unsigned char Ch, unsigned long Baud)
{
	unsigned long regaddr;
	PCONP |= 0x01L<<13;				//打开CAN控制器电源
	switch(Ch)						//配置CAN控制器引脚
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
	CAN_AFMR &= ~0x07;	//设置为旁路模式
	CAN_AFMR|=(1<<1);
	Quit_SWRst(Ch);
}
/*
*********************************************************************************************************
**函数原型		:  	unsigned char CANRCVANDSEND(unsigned char Ch, unsigned char BUFNum);
**参数说明		:  	Ch:CAN控制器号，0表示第一路CAN
**				:	BUFNum 选择缓冲区
**返回值		:	无
**说	明		:	CAN将收到的信息再发送出去 
*********************************************************************************************************/
unsigned char CANRCVANDSEND(unsigned char Ch, unsigned char BUFNum)
{
	unsigned long mes, CAN32reg;
	unsigned long regaddr;
	unsigned char i=5;
	unsigned char i2 = 0;
	BUFNum-=1;		// 计算地址方便。
	
	
	regaddr = (unsigned long)(&CAN1RFS)+Ch*CANOFFSET;				//处理DLC、RTR、FF
	mes = RGE(regaddr);
	mes &= 0Xffff0000;
	regaddr = (unsigned long)(&CAN1TFI1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	
	
	regaddr = (unsigned long)(&CAN1RID)+Ch*CANOFFSET;				//处理ID
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TID1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	dataRID = mes;
	
	regaddr = (unsigned long)(&CAN1RDA)+Ch*CANOFFSET;				//处理 数据A
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TDA1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	// MessageDetailR.DATAA = mes;
	for (i2 = 0; i2 < 4; i2++) {
		dataAB[i2] = mes >> 8 * i2;
	} 

	regaddr = (unsigned long)(&CAN1RDB)+Ch*CANOFFSET;				//处理 数据B
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TDB1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	// MessageDetailR.DATAB = mes;
	for (i2 = 0; i2 < 4; i2++) {
		dataAB[4 + i2] = mes >> 8 * i2;
	} 
		
	regaddr = (unsigned long)(&CAN1GSR)+Ch*CANOFFSET;				//查 接收缓冲区状态
	mes = RGE(regaddr);
	while(mes&(1<<0))												// RBS为1 接收缓冲区满
	{
		regaddr = (unsigned long)(&CAN1CMR)+Ch*CANOFFSET;				
		mes=RGE(regaddr);
		mes |= (1<<2);												//释放接收缓冲区
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
**函数原型		:  	unsigned char writedetail(unsigned char LEN,unsigned char FF, unsigned int ID,unsigned char *data);
**参数说明		:  	LEN:数据长度
**				:	*data 发送数据所在的数组 
				:   
**返回值		:	操作成功返回 1 失败返回 0
**说	明		:	 用户填写发送 帧信息
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
		MessageDetailT.DATAA=0;			//先清零
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
**函数原型		:  	unsigned char CANSend(unsigned char Ch, unsigned char BUFNum);
**参数说明		:  	Ch:CAN控制器号，0表示第一路CAN
**				:	BUFNum 选择缓冲区
				:   MessageDetailT 报文的特征信息及数据
**返回值		:	无
**说	明		:	CAN控制器退出软件复位模式
*********************************************************************************************************/
unsigned char CANSend(unsigned char Ch, unsigned char BUFNum)
{
	unsigned long CAN32reg;
	unsigned long regaddr;
	unsigned char FFflag;
	
	BUFNum-=1;		// 计算地址方便。
	
	
	regaddr = (unsigned long)(&CAN1TFI1)+Ch*CANOFFSET+0X10*BUFNum;	// 3缓冲区间地址差 0x10
	CAN32reg = RGE(regaddr);
	CAN32reg &= ~((0x0fL<<16) | (0x01L<<30) | (0x80000000));        //清 DLC,RTR.FF位?
	CAN32reg |= ((unsigned long)MessageDetailT.LEN<<16) | ((unsigned long)MessageDetailT.FF<<31);
	RGE(regaddr)=CAN32reg;
	
	FFflag = MessageDetailT.FF;
	
	regaddr = (unsigned long)(&CAN1TID1)+Ch*CANOFFSET+0X10*BUFNum;	//写帧ID
	CAN32reg = RGE(regaddr);
	if(FFflag)														//FF为1，ID为29位
	{
		CAN32reg &=0x70000000;
		CAN32reg |= (MessageDetailT.CANID & 0x1fffffff);
	}
	else
	{
		CAN32reg &= 0xfffff800;										//FF为0 ，ID为11位
		CAN32reg |= (MessageDetailT.CANID & 0x000007ff);
	}
	RGE(regaddr)=CAN32reg;
	
	regaddr = (unsigned long)(&CAN1TDA1)+Ch*CANOFFSET+0X10*BUFNum;	// 写帧数据A
	RGE(regaddr) = MessageDetailT.DATAA;
	
	regaddr = (unsigned long)(&CAN1TDB1)+Ch*CANOFFSET+0X10*BUFNum;	// 写帧数据B
	RGE(regaddr) = MessageDetailT.DATAB;
	
	regaddr = (unsigned long)(&CAN1CMR)+Ch*CANOFFSET;				//写控制寄存器，发送
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
	
	
	regaddr = (unsigned long)(&CAN1GSR)+Ch*CANOFFSET;				//查询发送状态
	CAN32reg = RGE(regaddr);
	/*/if(CAN32reg&(1<<5))											//等待所有请求的发送已经完成
	{
		;
	}	*/	
	if(CAN32reg&(1<<3))			//所有的请求已成功完成
	{
		
		return(1);				//发送成功返回 1
	}	
	else { return (0);}			//发送失败返回 0 
	
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
