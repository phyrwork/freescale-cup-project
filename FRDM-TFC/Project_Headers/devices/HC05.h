/* HC05.h
 * =====================
 * Author: Connor Newton
 */

#ifndef TFC_HC05_
#define TFC_HC05_

#include "io/Bluetooth.h"

/* Import UART controls; set macros to use them */
#include "io/UART.h"
#define  TX(msg,size)   UART0_SendRaw( (msg) , (size) ); UART0_Process(); while (rbUsed(&TxBuffer) > 0) {}; for (uint8_t i = 0; i < 5*CORE_CLOCK/BLUETOOTH_CMD_BAUD; i++) {}
#define  SET_BAUD(baud) uart0_init(CORE_CLOCK/2/1000, (baud) )

/* Import GPIO controls; set macros to use them */
//#include "io/UART.h"
#include "io/GPIO.h"
#define  PWR_ON
#define  PWR_OFF
#define  SET_KEY GPIO_Set(&btcmd)
#define  CLR_KEY GPIO_Clear(&btcmd)

/* Macro definitions to facilitate concatenation
   of string literals with #defines */
#define S(x) #x           
#define pp_string(x) S(x)

int8_t HC05_Init(BT_Config)
{
    /* Power down HC05 */
    PWR_OFF;

    /* Assert KEY and power up again */
    SET_KEY;
    PWR_ON;
    
    /* Do configuration */

    /////////////////
    // CONFIG HERE //
    /////////////////
    
    /* Unassert KEY */
    CLR_KEY;

    return 0; //Configuration successful, resume;
}

/*
uint8_t hc05setBaud115200_cmd[] = "AT+UART=" pp_string(BLUETOOTH_SERIAL_BAUD) ",1,0\r\n";
uint8_t hc05setBaud115200_res[] = "OK\r\n";
*/

#endif //TFC_HC05_