#include "LPC23xx.h"
#include "../common/type.h"
#include "../common/irq.h"
#include "../uart/uart.h"
#include "../common/target.h"
#include "../can/LPC2300CAN.h"
#include "../common/key/choiceKey.h"

#define KEY(i)  ((IOPIN1&1<<i)>>i)	
#define key1    ((IOPIN1&1<<24)>>24)
#define key2    ((IOPIN1&1<<25)>>25)
#define key3    ((IOPIN1&1<<26)>>26)
#define key4    ((IOPIN1&1<<27)>>27)

extern unsigned char dataAB[8];
extern unsigned long dataRID;

extern volatile DWORD UART0Count;
extern volatile BYTE UART0Buffer[BUFSIZE];
extern volatile DWORD UART1Count;
extern volatile BYTE UART1Buffer[BUFSIZE];

BOOL checkCANmessage(void);

int main(void)
{
	unsigned char j, j1, j2;

	unsigned char data11[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	j1 = j2 =0;

	KeyInit();

	CAN_Init(0,0x140002);						        //channel:0 Baud speed:1M
    // CAN_Init(1,0x140002);						    //channel:1 Baud speed:1M

	// UARTInit(0, 9600);	                            /* baud rate setting */
    UARTInit(1, 9600);	                                /* baud rate setting */
	while (1)
	{	
		/* key in - begin */
		if( KEY(24) == 0) {								 	
	        while(KEY(24) == 0 ) {
				if (checkCANmessage()) {
			        for (j1 = 0; j1 < 8; j1++) {
					    data11[j1] = dataAB[j1];
				    }

	                writedetail(8, 1, dataRID, data11);
	
                    CANSend(0, 2);
			    } // send end
		    } // key24 while end

        } else if ( KEY(25) == 0 ) {

            while (KEY(25) == 0) {
				if (checkCANmessage()) {
			        /* This is new append code 20110222 */
				    for (j2 = 0; j2 < 8; j2++) {
				        UART1Buffer[j2] = dataAB[j2];
				    }
								 
		            preUARTInitRBR(1);
		            UARTSend( 1, (BYTE *)UART1Buffer, 8 );       // channel:1
		            postUARTInitRBR(1); 		
			    } // send end

			} // key25 while end

        } else if (KEY(26) == 0) {  	
    
	        while(KEY(26) == 0 );

        } else if (KEY(27) == 0) {	
    	    UART1Count = 0x0F;
	        while(KEY(27) == 0);
	    }
	    /* key in - end */

	}  /* while end	*/
	
    return (0);
}

BOOL checkCANmessage(void) {
    unsigned int regaddr, mes;
    BOOL recFlag1 = FALSE;
    if (CAN1GSR & 0x01)							// CAN0 接收到信息 
    {
		CANRCVANDSEND(0,3);                     // 参数分别为：（ CAN通道号，选择缓冲区（1、2、3））

		recFlag1 = TRUE;
		while (CAN1GSR & 0x01)					// RBS为1 接收缓冲区满
		{
			mes  = CAN1CMR;
			mes |= (1 << 2);					// 释放接收缓冲区
			CAN1CMR = mes;
		}
			CAN1CMR &= ~(1 << 2);
	}		

	if (CAN1GSR & (0x01 << 6))					// CAN0错误计数器是否超过错误报警限制
	{
		Enter_SWRst(1);
		regaddr = (unsigned long)(&CAN1GSR);		
		mes     = RGE(regaddr);
		mes    &= 0x00ff;												
		RGE(regaddr) = mes;	    				// 总线错误清除处理
		Quit_SWRst(1);
	}
	
	return (recFlag1);		
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
