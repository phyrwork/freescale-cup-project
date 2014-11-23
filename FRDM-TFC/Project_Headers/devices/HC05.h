/* HC05.h
 * =====================
 * Author: Connor Newton
 */

#ifndef TFC_HC05_
#define TFC_HC05_

#include "io/Bluetooth.h"

void HC05_Init();
int8_t HC05_Config(BT_Config* config);

#endif //TFC_HC05_