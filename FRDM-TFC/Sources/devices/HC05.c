/* HC05.c
 * =====================
 * Author: Connor Newton
 */

/* Import UART controls; set macros to use them */
#include "io/UART.h"
#define  TX(msg,size)   UART0_SendRaw( (msg) , (size) ); UART0_Process(); while (rbUsed(&TxBuffer) > 0) {}; for (uint8_t i = 0; i < 5*CORE_CLOCK/BLUETOOTH_CMD_BAUD; i++) {}
#define  SET_BAUD(baud) uart0_init(CORE_CLOCK/2/1000, (baud) )

/* Import GPIO controls; configure GPIO pins; set macros */
#include "io/GPIO.h"

GPIO_Pin    pwr;
GPIO_Config pwr_conf = {
    /* .io = */ {
        /* .io = */ {
            /* .port = */ E,
            /* .pin = */  0
        },
        /* .mux = */ 2
    },
    /* .dir = */ GPIO_OUT
};

GPIO_Pin    cmd;
GPIO_Config cmd_conf = {
    /* .io = */ {
        /* .io = */ {
            /* .port = */ E,
            /* .pin = */  1
        },
        /* .mux = */ 2
    },
    /* .dir = */ GPIO_OUT
};

#define  PWR_ON  GPIO_Set(&pwr)
#define  PWR_OFF GPIO_Clear(&pwr)
#define  SET_KEY GPIO_Set(&cmd)
#define  CLR_KEY GPIO_Clear(&cmd)
//

/* Macro definitions to facilitate concatenation
   of string literals with #defines */
#define S(x) #x           
#define pp_string(x) S(x)

void HC05_Init() {
    /* Initialise IO */
    pwr = GPIO_InitPin(&pwr_conf);
    cmd = GPIO_InitPin(&cmd_conf);
}

int8_t HC05_Config(BT_Config* config)
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