/* HC05.c
 * =====================
 * Author: Connor Newton
 */

/* Import UART controls; set macros to use them */
#include "io/UART.h"
#include "devices/CrystalClock.h"
#define  TX(msg,size)   UART0_SendRaw( (msg) , (size) )
#define  SET_BAUD(baud) UART0_ConfigureDataRate(CORE_CLOCK/2/1000, (baud) )

/* Import GPIO controls; configure GPIO pins; set macros */
#include "io/GPIO.h"

GPIO_Pin    pwr;
GPIO_Config pwr_conf = { /* .io = */ { /* .io = */ {
    /* .port = */ E,
    /* .pin = */  0 },
    /* .mux = */ MUX_ALT1 },
    /* .dir = */ GPIO_OUT
};

GPIO_Pin    cmd;
GPIO_Config cmd_conf = { /* .io = */ { /* .io = */ {
    /* .port = */ E,
    /* .pin = */  1 },
    /* .mux = */  MUX_ALT1 },
    /* .dir = */  GPIO_OUT
};

#define  PWR_ON  GPIO_Set(&pwr)
#define  PWR_OFF GPIO_Clear(&pwr)
#define  SET_KEY GPIO_Set(&cmd)
#define  CLR_KEY GPIO_Clear(&cmd)
//

/* HC05 configuration */
#include "config.h"
#define  HC05_CMD_BAUD 38400

/* Initialisation from struct like this a bit more involved than usual */
// HC05_Config config = {
//     /* .baud = */    BLUETOOTH_SERIAL_BAUD,
//     /* .stop = */    1,
//     /* .parity = */ 0
// }

/* Macro definitions to facilitate concatenation
   of string literals with #defines */
#define S(x)   #x           
#define Str(x) S(x)

int8_t HC05_Init(/*HC05_Config* config*/)
{
    /* Configure IO */
    pwr = GPIO_InitPin(&pwr_conf);
    cmd = GPIO_InitPin(&cmd_conf);
    SET_BAUD(HC05_CMD_BAUD); // Set serial symbol rate

    /* Power down HC05 */
    PWR_OFF;

    /* Assert KEY and power up again */
    SET_KEY;
    PWR_ON;
    
    /* Do configuration */
    uint8_t msg[] = "AT+UART=" Str(BLUETOOTH_SERIAL_BAUD) ",1,0\r\n"; //Baud,stop bits,parity bits
    TX(msg, sizeof msg);
    
    /* Unassert KEY */
    CLR_KEY;

    return 0; //Configuration complete, resume;
}
