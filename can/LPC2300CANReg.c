/****************************************Copyright (c)**************************************************
**                                    ������Զ�������޹�˾
**                                     ��ҵͨ��������ҵ��
**                                http://www.embedcontrol.com
**-------------------------------------�ļ���Ϣ--------------------------------------------------------
**	��   ��  	��:	LPC2300CANReg.C
** 	��  ǰ  ��  ��:	v2.0
** 	��		    ��:	2007��4��26��
**	��		    ��:	
**-------------------------------------��ʷ����--------------------------------------------------------
**	��   ��  	��:	LPC2300CANReg.C
** 	��  		��:	v1.0
** 	��		    ��:	2007��4��26��
**	��    ��    ��:	������
**	��		    ��:	ȫ�°汾��LPCϵ��CAN����������������,CAN�������Ĵ�������
**-------------------------------------��ʷ�޸�--------------------------------------------------------
**	��   ��  	��:	
** 	��  		��:	
** 	��		    ��:	
**	��   ��     ��:	
**	��		    ��:	
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
**   ���� CANMOD �Ĵ���:
1.This register controls the basic operating mode of the CAN Controller. 
2.Bits not listed read as 0 and should be written as 0.
*********************************************************************************************************************
*/
typedef	union		_canmod_
{
	unsigned long DWord;
	struct	{
		unsigned int	RM 		:1;		//����RMλ(��λλ)
		unsigned int	LOM 	:1;		//����LOMλ(ֻ��ģʽλ)

		unsigned int	STM		:1;		//����STMλ "0"���͵���Ϣ���뱻Ӧ��ű��Ͽ�
		//"1"�Բ���ģʽ,CANCMR��SRRλһ��ʹ�ã����Դﵽ�Է����յ�Ч��

		unsigned int	TPM 	:1;		//����TPMλ "0"3�����ͻ����������ȼ��ɸ��Ե�CAN ID����
		//"1"3�����ͻ����������ȼ��ɸ��Ե�Tx���ȼ������

		unsigned int	SM 		:1;		//����SMλ(˯��ģʽλ)

		unsigned int	RPM 	:1;		//����RPMλ "0"��������������λ��Rx��Tx��Ϊ�͵�ƽ
		//"1"��ת����ģʽ--��������������λ��Rx��Ϊ�ߵ�ƽ

		unsigned int	RSV1 	:1;		//����λ
		unsigned int	TM 		:1;		//����TMλ(����ģʽ)
		unsigned int	RSV24	:24;	//����λ
	}Bits;
}REG_CANMOD;
/*
**********************************************************************************************************************
**   ���� CANBTR �Ĵ���:
1.This register controls how various CAN timings are derived from the VPB clock.
2.It can be read at any time, but can only be written if the RM bit in CANmod is 1.
**********************************************************************************************************************
*/
typedef	union		_canbtr_
{
	unsigned long	DWord;				//�ֲ�������
	struct	
	{
		unsigned int	BRP 	:10;	//Ԥ��Ƶλ��϶���
		unsigned int	RSV4	:4;		//����λ
		unsigned int	SJW 	:2;		//ͬ����ת���
		unsigned int	TSEG1 	:4;		//ʱ���1
		unsigned int	TSEG2 	:3;		//ʱ���2
		unsigned int	SAM 	:1;		//����ģʽλ
		unsigned int	RSV8	:8;		//����
	}Bits;
}REG_CANBTR;
/*
**********************************************************************************************************************
**   ���� CANCMR �Ĵ���:
1.Writing to this write-only register initiates an action. 
2.Bits not listed should be written as 0. Reading this register yields zeroes.
**********************************************************************************************************************
*/
typedef	union		_cancmr_
{
	unsigned long	DWord;
	struct	
	{
		unsigned int	TR 		:1;		//����TRλ(��������)
		unsigned int	AT 		:1;		//����ATλ(������ֹ)
		unsigned int	RRB 	:1;		//����RRBλ(���ջ������ͷ�)
		unsigned int	CDO 	:1;		//����������λ
		unsigned int	SRR 	:1;		//����SRRλ(�Խ�������)
		unsigned int	STB1 	:1;		//����SEND TxBUF1λ(ѡ��BUF1����)
		unsigned int	STB2 	:1;		//����SEND TxBUF2λ(ѡ��BUF2����)
		unsigned int	STB3 	:1;		//����SEND TxBUF3λ(ѡ��BUF3����)
		unsigned int	RSV24	:24;	//����λ
	}Bits;
}REG_CANCMR;
/*
**********************************************************************************************************************
**   ���� CANRFS �Ĵ���:
1.This register defines the characteristics of the current received message. 
2.It is read-only in normal operation, but can be written for testing purposes if the RM bit in CANMOD is 1.
**********************************************************************************************************************
*/
typedef	union		_canrfs_
{
	unsigned long		DWord;				//�ֲ�������
	struct	
	{
		unsigned int	IDIN 	:10;	//ID����ֵ
		unsigned int	BP 		:1;		//BP
		unsigned int	RSV5 	:5;		//����
		unsigned int	DLC 	:4;		//���ݳ���
		unsigned int	RSV10	:10;	//����
		unsigned int	RTR 	:1;		//Զ��֡������֡ʶ��λ
		unsigned int	FF 		:1;		//��չ֡����׼֡ʶ��λ
	}Bits;
}REG_CANRFS;
/*
******************************************************************
	CAN�������ṹ�嶨��
******************************************************************
*/
typedef struct _CANCONTROLLOR
{
	unsigned char	m_Channel;		//CANͨ��
	//unsigned char	m_bIsLOM;		//ֻ��ģʽ��־
	//unsigned char	m_bSLPM;		//˯��ģʽ��־
	unsigned long	m_SysClk;		//ϵͳʱ��
	unsigned char	m_Status;		//״̬
	
	/*INT8U	m_EWL;			//������ֵ
	INT32U	m_CAN_Baud;		//CAN������ֵ
	INT32U	m_RegBaseAddr;	//CAN����������ʼ��ַ
	REG_CANRxBUF *m_RxBufAddr;	//���ջ������׵�ַָ��
	INT16U	m_iCAN_RxBufLen;//���ջ�������������	
	INT16U	m_iRdPoint;		//���ջ����������
	INT16U	m_iWrPiont;		//���ջ�����д���
	INT16U	m_iDataCout;	//���ջ�������Ч������Ŀ
	BOOL	m_bIsFull;		//���ջ���������־
	BOOL	m_IsSendOK;		//���ͳɹ���־
	void	*pSendSem;		//�����ź���
	void	*pRecvSem;		//�����ź���
	void	*pRcvMutexSem;	//���ն�ռ�ź���
	void	*pSndMutexSem;	//���Ͷ�ռ�ź���*/
}CANCONTROLLOR;
