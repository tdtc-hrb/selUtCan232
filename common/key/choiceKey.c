#include <LPC23XX.h>
#include "choiceKey.h"

void KeyInit(void)
{
	PINSEL3  = (PINSEL3&0xF0FFFFFF)|0x00000000;	    
	PINSEL10 = 0x00000000;		                            //½ûÖ¹ETM
	IODIR1   = (IODIR1&0xF0FFFFFF)|0x00000000;	    
}
