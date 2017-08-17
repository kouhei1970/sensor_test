/***************************************************************/
/*                                                             */
/*      PROJECT NAME :  rxserial_test                          */
/*      FILE         :  hardware_setup.c                       */
/*      DESCRIPTION  :  Hardware Initialization                */
/*      CPU SERIES   :  RX600                                  */
/*      CPU TYPE     :  RX621                                  */
/*                                                             */
/*      This file is generated by e2 studio.                   */
/*                                                             */
/***************************************************************/                                
                                                                           
                                                                           
/************************************************************************/
/*    File Version: V1.00                                               */
/*    Date Generated: 08/07/2013                                        */
/************************************************************************/

#include "iodefine.h"
#include "rxserial.h"

#ifdef __cplusplus
extern "C" {
#endif
extern void HardwareSetup(void);
#ifdef __cplusplus
}
#endif

void HardwareSetup(void)
{
	volatile i;

	//IOPORTD
	PORTD.DDR.BIT.B0 = 1;
	PORTD.DDR.BIT.B1 = 1;

	init_serial();
	init_iic_master();


}