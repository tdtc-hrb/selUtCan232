/****************************************Copyright (c)**************************************************
**                                    ¹ãÖİÖÂÔ¶µç×ÓÓĞÏŞ¹«Ë¾
**                                     ¹¤ÒµÍ¨ĞÅÍøÂçÊÂÒµ²¿
**                                http://www.embedcontrol.com
**-------------------------------------ÎÄ¼şĞÅÏ¢--------------------------------------------------------
**	ÎÄ   ¼ş  	Ãû:	LPC2300CAN.c
** 	µ±  Ç°  °æ  ±¾:	v1.0
** 	ÈÕ		    ÆÚ:	2007Äê5ÔÂ06ÈÕ
**	Ãè		    Êö:	
**-------------------------------------ÀúÊ·´´½¨--------------------------------------------------------
**	ÎÄ   ¼ş  	Ãû:	LPC2300CAN.c
** 	°æ  		±¾:	v1.0
** 	ÈÕ		    ÆÚ:	2007Äê4ÔÂ27ÈÕ
**	´´    ½¨    ÈË:	ëøĞÀĞÀ
**	Ãè		    Êö:	LPC2300¿ª·¢°åÊµÑé½Ì³ÌÊµÀı´úÂë
**-------------------------------------ÀúÊ·ĞŞ¸Ä--------------------------------------------------------
**	ÎÄ   ¼ş  	Ãû:	
** 	°æ  		±¾:	
** 	ÈÕ		    ÆÚ:	
**	ĞŞ   ¸Ä     ÈË:	
**	Ãè		    Êö:	
********************************************************************************************************/
#include "../can/LPC2300CAN.h"
#include "lpc23xx.h"
#include "../common/type.h"
static struct _MessageDetail MessageDetailT,MessageDetailR;

unsigned char dataAB[8];
unsigned long dataRID;

/*
*********************************************************************************************************
**º¯ÊıÔ­ĞÍ		:  	unsigned char Enter_SWRst(unsigned char Ch);
**²ÎÊıËµÃ÷		:  	Ch:CAN¿ØÖÆÆ÷ºÅ£¬0±íÊ¾µÚÒ»Â·CAN
**·µ»ØÖµ		:	0£ºÊ§°Ü
**				:	1: ³É¹¦	
**Ëµ	Ã÷		:	CAN¿ØÖÆÆ÷½øÈëÈí¼ş¸´Î»Ä£Ê½
*********************************************************************************************************/
unsigned char Enter_SWRst(unsigned char Ch)
{
	unsigned long regaddr;
	REG_CANMOD regmod;
	regaddr = (unsigned long)(&CAN1MOD)+Ch*CANOFFSET;			
	regmod.DWord = RGE(regaddr);		//¶ÁÈ¡CAN1MOD¼Ä´æÆ÷
	regmod.Bits.RM = 1;					//RMÎ»ÖÃ¡°1¡±
	RGE(regaddr) = regmod.DWord;		//»ØĞ´CAN1MOD¼Ä´æÆ÷
	regmod.DWord = RGE(regaddr);		//ÑéÖ¤Ğ´ÈëÖµ	
	return (0 != regmod.Bits.RM)? 1:0; 
}
/*
*********************************************************************************************************
**º¯ÊıÔ­ĞÍ		:  	unsigned char Quit_SWRst(unsigned char Ch);
**²ÎÊıËµÃ÷		:  	Ch:CAN¿ØÖÆÆ÷ºÅ£¬0±íÊ¾µÚÒ»Â·CAN
**·µ»ØÖµ		:	0£ºÊ§°Ü
**				:	1: ³É¹¦	
**Ëµ	Ã÷		:	CAN¿ØÖÆÆ÷ÍË³öÈí¼ş¸´Î»Ä£Ê½
*********************************************************************************************************/
unsigned char Quit_SWRst(unsigned char Ch)
{
	unsigned long regaddr;
	REG_CANMOD regmod;
	regaddr = (unsigned long)(&CAN1MOD)+Ch*CANOFFSET;			
	regmod.DWord = RGE(regaddr);		//¶ÁÈ¡CAN1MOD¼Ä´æÆ÷
	regmod.Bits.RM = 0;					//RMÎ»ÖÃ¡°0¡±
	RGE(regaddr) = regmod.DWord;		//»ØĞ´CAN1MOD¼Ä´æÆ÷
	regmod.DWord = RGE(regaddr);		//ÑéÖ¤Ğ´ÈëÖµ	
	return (0 != regmod.Bits.RM)? 0:1; 
}
/*
*********************************************************************************************************
**º¯ÊıÔ­ĞÍ		:  	unsigned char CAN_Init(unsigned char Ch, unsigned long Baud);
**²ÎÊıËµÃ÷		:  	Ch:CAN¿ØÖÆÆ÷ºÅ£¬0±íÊ¾µÚÒ»Â·CAN
**				:	Baud:CAN²¨ÌØÂÊÖµ
**·µ»ØÖµ		:	ÎŞ
**Ëµ	Ã÷		:	CAN¿ØÖÆÆ÷ÍË³öÈí¼ş¸´Î»Ä£Ê½
*********************************************************************************************************/
void CAN_Init(unsigned char Ch, unsigned long Baud)
{
	unsigned long regaddr;
	PCONP |= 0x01L<<13;				//´ò¿ªCAN¿ØÖÆÆ÷µçÔ´
	switch(Ch)						//ÅäÖÃCAN¿ØÖÆÆ÷Òı½Å
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
	CAN_AFMR &= ~0x07;	//ÉèÖÃÎªÅÔÂ·Ä£Ê½
	CAN_AFMR|=(1<<1);
	Quit_SWRst(Ch);
}
/*
*********************************************************************************************************
**º¯ÊıÔ­ĞÍ		:  	unsigned char CANRCVANDSEND(unsigned char Ch, unsigned char BUFNum);
**²ÎÊıËµÃ÷		:  	Ch:CAN¿ØÖÆÆ÷ºÅ£¬0±íÊ¾µÚÒ»Â·CAN
**				:	BUFNum Ñ¡Ôñ»º³åÇø
**·µ»ØÖµ		:	ÎŞ
**Ëµ	Ã÷		:	CAN½«ÊÕµ½µÄĞÅÏ¢ÔÙ·¢ËÍ³öÈ¥ 
*********************************************************************************************************/
unsigned char CANRCVANDSEND(unsigned char Ch, unsigned char BUFNum)
{
	unsigned long mes, CAN32reg;
	unsigned long regaddr;
	unsigned char i=5;
	unsigned char i2 = 0;
	BUFNum-=1;		// ¼ÆËãµØÖ··½±ã¡£
	
	
	regaddr = (unsigned long)(&CAN1RFS)+Ch*CANOFFSET;				//´¦ÀíDLC¡¢RTR¡¢FF
	mes = RGE(regaddr);
	mes &= 0Xffff0000;
	regaddr = (unsigned long)(&CAN1TFI1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	
	
	regaddr = (unsigned long)(&CAN1RID)+Ch*CANOFFSET;				//´¦ÀíID
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TID1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	dataRID = mes;
	
	regaddr = (unsigned long)(&CAN1RDA)+Ch*CANOFFSET;				//´¦Àí Êı¾İA
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TDA1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	// MessageDetailR.DATAA = mes;
	for (i2 = 0; i2 < 4; i2++) {
		dataAB[i2] = mes >> 8 * i2;
	} 

	regaddr = (unsigned long)(&CAN1RDB)+Ch*CANOFFSET;				//´¦Àí Êı¾İB
	mes = RGE(regaddr);
	regaddr = (unsigned long)(&CAN1TDB1)+Ch*CANOFFSET+0X10*BUFNum;
	RGE(regaddr)=mes;
	// MessageDetailR.DATAB = mes;
	for (i2 = 0; i2 < 4; i2++) {
		dataAB[4 + i2] = mes >> 8 * i2;
	} 
		
	regaddr = (unsigned long)(&CAN1GSR)+Ch*CANOFFSET;				//²é ½ÓÊÕ»º³åÇø×´Ì¬
	mes = RGE(regaddr);
	while(mes&(1<<0))												// RBSÎª1 ½ÓÊÕ»º³åÇøÂú
	{
		regaddr = (unsigned long)(&CAN1CMR)+Ch*CANOFFSET;				
		mes=RGE(regaddr);
		mes |= (1<<2);												//ÊÍ·Å½ÓÊÕ»º³åÇø
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
**º¯ÊıÔ­ĞÍ		:  	unsigned char writedetail(unsigned char LEN,unsigned char FF, unsigned int ID,unsigned char *data);
**²ÎÊıËµÃ÷		:  	LEN:Êı¾İ³¤¶È
**				:	*data ·¢ËÍÊı¾İËùÔÚµÄÊı×é 
				:   
**·µ»ØÖµ		:	²Ù×÷³É¹¦·µ»Ø 1 Ê§°Ü·µ»Ø 0
**Ëµ	Ã÷		:	 ÓÃ»§ÌîĞ´·¢ËÍ Ö¡ĞÅÏ¢
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
		MessageDetailT.DATAA=0;			//ÏÈÇåÁã
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
**º¯ÊıÔ­ĞÍ		:  	unsigned char CANSend(unsigned char Ch, unsigned char BUFNum);
**²ÎÊıËµÃ÷		:  	Ch:CAN¿ØÖÆÆ÷ºÅ£¬0±íÊ¾µÚÒ»Â·CAN
**				:	BUFNum Ñ¡Ôñ»º³åÇø
				:   MessageDetailT ±¨ÎÄµÄÌØÕ÷ĞÅÏ¢¼°Êı¾İ
**·µ»ØÖµ		:	ÎŞ
**Ëµ	Ã÷		:	CAN¿ØÖÆÆ÷ÍË³öÈí¼ş¸´Î»Ä£Ê½
*********************************************************************************************************/
unsigned char CANSend(unsigned char Ch, unsigned char BUFNum)
{
	unsigned long CAN32reg;
	unsigned long regaddr;
	unsigned char FFflag;
	
	BUFNum-=1;		// ¼ÆËãµØÖ··½±ã¡£
	
	
	regaddr = (unsigned long)(&CAN1TFI1)+Ch*CANOFFSET+0X10*BUFNum;	// 3»º³åÇø¼äµØÖ·²î 0x10
	CAN32reg = RGE(regaddr);
	CAN32reg &= ~((0x0fL<<16) | (0x01L<<30) | (0x80000000));        //Çå DLC,RTR.FFÎ»¡
	CAN32reg |= ((unsigned long)MessageDetailT.LEN<<16) | ((unsigned long)MessageDetailT.FF<<31);
	RGE(regaddr)=CAN32reg;
	
	FFflag = MessageDetailT.FF;
	
	regaddr = (unsigned long)(&CAN1TID1)+Ch*CANOFFSET+0X10*BUFNum;	//Ğ´Ö¡ID
	CAN32reg = RGE(regaddr);
	if(FFflag)														//FFÎª1£¬IDÎª29Î»
	{
		CAN32reg &=0x70000000;
		CAN32reg |= (MessageDetailT.CANID & 0x1fffffff);
	}
	else
	{
		CAN32reg &= 0xfffff800;										//FFÎª0 £¬IDÎª11Î»
		CAN32reg |= (MessageDetailT.CANID & 0x000007ff);
	}
	RGE(regaddr)=CAN32reg;
	
	regaddr = (unsigned long)(&CAN1TDA1)+Ch*CANOFFSET+0X10*BUFNum;	// Ğ´Ö¡Êı¾İA
	RGE(regaddr) = MessageDetailT.DATAA;
	
	regaddr = (unsigned long)(&CAN1TDB1)+Ch*CANOFFSET+0X10*BUFNum;	// Ğ´Ö¡Êı¾İB
	RGE(regaddr) = MessageDetailT.DATAB;
	
	regaddr = (unsigned long)(&CAN1CMR)+Ch*CANOFFSET;				//Ğ´¿ØÖÆ¼Ä´æÆ÷£¬·¢ËÍ
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
	
	
	regaddr = (unsigned long)(&CAN1GSR)+Ch*CANOFFSET;				//²éÑ¯·¢ËÍ×´Ì¬
	CAN32reg = RGE(regaddr);
	/*/if(CAN32reg&(1<<5))											//µÈ´ıËùÓĞÇëÇóµÄ·¢ËÍÒÑ¾­Íê³É
	{
		;
	}	*/	
	if(CAN32reg&(1<<3))			//ËùÓĞµÄÇëÇóÒÑ³É¹¦Íê³É
	{
		
		return(1);				//·¢ËÍ³É¹¦·µ»Ø 1
	}	
	else { return (0);}			//·¢ËÍÊ§°Ü·µ»Ø 0 
	
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
