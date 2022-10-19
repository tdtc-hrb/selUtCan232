/****************************************Copyright (c)**************************************************
**                                    广州致远电子有限公司
**                                     工业通信网络事业部
**                                http://www.embedcontrol.com
**-------------------------------------文件信息--------------------------------------------------------
**	文   件  	名:	LPC2300CANReg.C
** 	当  前  版  本:	v2.0
** 	日		    期:	2007年4月26日
**	描		    述:	
**-------------------------------------历史创建--------------------------------------------------------
**	文   件  	名:	LPC2300CANReg.C
** 	版  		本:	v1.0
** 	日		    期:	2007年4月26日
**	创    建    人:	滕欣欣
**	描		    述:	全新版本的LPC系列CAN控制器驱动函数库,CAN控制器寄存器定义
**-------------------------------------历史修改--------------------------------------------------------
**	文   件  	名:	
** 	版  		本:	
** 	日		    期:	
**	修   改     人:	
**	描		    述:	
********************************************************************************************************/
#define LPC2300PCONP	(*((volatile unsigned long*)0xE01FC0C4))
#define LPC2300PINSEL0	(*((volatile unsigned long*)0xE002C000))
#define LPC2300PINSEL1	(*((volatile unsigned long*)0xE002C004))
#define LPC2300PINSEL4	(*((volatile unsigned long*)0xE002C010))

#define LPC2300_CAN_CTRLRAG_BASEADDR	0xE0044000
#define LPC2300_CANCTRLOR_ADDR_OFFSET	0x4000
#define LPC3200REG(CH, REGOFFSET)	(*((volatile unsigned long*)(LPC2300_CAN_CTRLRAG_BASEADDR + LPC2300_CANCTRLOR_ADDR_OFFSET*CH + REGOFFSET*4)))

enum LPC2300CANCTRLOR_REGOFFSET {MOD=0,CMR,GSR,ICR,IER,BTR,EWL,SR,RFS,RID,RDA,RDB,TFI1,TID1,TDA1,TDB1,TFI2,TID2,TDA2,TDB2,TFI3,TID3,TDA3,TDB3};

/*
*********************************************************************************************************************
**   定义 CANMOD 寄存器:
1.This register controls the basic operating mode of the CAN Controller. 
2.Bits not listed read as 0 and should be written as 0.
*********************************************************************************************************************
*/
typedef	union		_canmod_
{
	unsigned long DWord;
	struct	{
		unsigned int	RM 		:1;		//定义RM位(复位位)
		unsigned int	LOM 	:1;		//定义LOM位(只听模式位)

		unsigned int	STM		:1;		//定义STM位 "0"发送的信息必须被应答才被认可
		//"1"自测试模式,CANCMR的SRR位一起使用，可以达到自发自收的效果

		unsigned int	TPM 	:1;		//定义TPM位 "0"3个发送缓冲区的优先级由各自的CAN ID决定
		//"1"3个发送缓冲区的优先级由各自的Tx优先级域决定

		unsigned int	SM 		:1;		//定义SM位(睡眠模式位)

		unsigned int	RPM 	:1;		//定义RPM位 "0"如果传输的是显性位，Rx和Tx脚为低电平
		//"1"翻转极性模式--如果传输的是显性位，Rx脚为高电平

		unsigned int	RSV1 	:1;		//保留位
		unsigned int	TM 		:1;		//定义TM位(测试模式)
		unsigned int	RSV24	:24;	//保留位
	}Bits;
}REG_CANMOD;
/*
**********************************************************************************************************************
**   定义 CANBTR 寄存器:
1.This register controls how various CAN timings are derived from the VPB clock.
2.It can be read at any time, but can only be written if the RM bit in CANmod is 1.
**********************************************************************************************************************
*/
typedef	union		_canbtr_
{
	unsigned long	DWord;				//字操作定义
	struct	
	{
		unsigned int	BRP 	:10;	//预分频位组合定义
		unsigned int	RSV4	:4;		//保留位
		unsigned int	SJW 	:2;		//同步跳转宽度
		unsigned int	TSEG1 	:4;		//时间段1
		unsigned int	TSEG2 	:3;		//时间段2
		unsigned int	SAM 	:1;		//采样模式位
		unsigned int	RSV8	:8;		//保留
	}Bits;
}REG_CANBTR;
/*
**********************************************************************************************************************
**   定义 CANCMR 寄存器:
1.Writing to this write-only register initiates an action. 
2.Bits not listed should be written as 0. Reading this register yields zeroes.
**********************************************************************************************************************
*/
typedef	union		_cancmr_
{
	unsigned long	DWord;
	struct	
	{
		unsigned int	TR 		:1;		//定义TR位(发送请求)
		unsigned int	AT 		:1;		//定义AT位(发送中止)
		unsigned int	RRB 	:1;		//定义RRB位(接收缓冲区释放)
		unsigned int	CDO 	:1;		//清除数据溢出位
		unsigned int	SRR 	:1;		//定义SRR位(自接收请求)
		unsigned int	STB1 	:1;		//定义SEND TxBUF1位(选择BUF1发送)
		unsigned int	STB2 	:1;		//定义SEND TxBUF2位(选择BUF2发送)
		unsigned int	STB3 	:1;		//定义SEND TxBUF3位(选择BUF3发送)
		unsigned int	RSV24	:24;	//保留位
	}Bits;
}REG_CANCMR;
/*
**********************************************************************************************************************
**   定义 CANRFS 寄存器:
1.This register defines the characteristics of the current received message. 
2.It is read-only in normal operation, but can be written for testing purposes if the RM bit in CANMOD is 1.
**********************************************************************************************************************
*/
typedef	union		_canrfs_
{
	unsigned long		DWord;				//字操作定义
	struct	
	{
		unsigned int	IDIN 	:10;	//ID索引值
		unsigned int	BP 		:1;		//BP
		unsigned int	RSV5 	:5;		//保留
		unsigned int	DLC 	:4;		//数据长度
		unsigned int	RSV10	:10;	//保留
		unsigned int	RTR 	:1;		//远程帧、数据帧识别位
		unsigned int	FF 		:1;		//扩展帧、标准帧识别位
	}Bits;
}REG_CANRFS;
/*
******************************************************************
	CAN控制器结构体定义
******************************************************************
*/
typedef struct _CANCONTROLLOR
{
	unsigned char	m_Channel;		//CAN通道
	//unsigned char	m_bIsLOM;		//只听模式标志
	//unsigned char	m_bSLPM;		//睡眠模式标志
	unsigned long	m_SysClk;		//系统时钟
	unsigned char	m_Status;		//状态
	
	/*INT8U	m_EWL;			//出错警告值
	INT32U	m_CAN_Baud;		//CAN波特率值
	INT32U	m_RegBaseAddr;	//CAN控制器的起始基址
	REG_CANRxBUF *m_RxBufAddr;	//接收缓冲区首地址指针
	INT16U	m_iCAN_RxBufLen;//接收缓冲区数量长度	
	INT16U	m_iRdPoint;		//接收缓冲区读入口
	INT16U	m_iWrPiont;		//接收缓冲区写入口
	INT16U	m_iDataCout;	//接收缓冲区有效数据数目
	BOOL	m_bIsFull;		//接收缓冲区满标志
	BOOL	m_IsSendOK;		//发送成功标志
	void	*pSendSem;		//发送信号量
	void	*pRecvSem;		//接收信号量
	void	*pRcvMutexSem;	//接收独占信号量
	void	*pSndMutexSem;	//发送独占信号量*/
}CANCONTROLLOR;
