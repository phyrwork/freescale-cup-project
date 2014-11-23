/* GPIO.h
 * =================================
 * GPIO assistant for Kinetis KL25
 * ---------------------------------
 * Assists easy peripheral setup.
 */

#ifndef KL25_GPIO
#define KL25_GPIO

#include "io/IO.h"

typedef enum { GPIO_IN = 0, GPIO_OUT = 1 } GPIO_Dir;

typedef IO_Pin GPIO_Pin;

typedef struct {
	IO_Config io;
	GPIO_Dir  dir;
} GPIO_Config;

extern GPIO_MemMapPtr GPIO[];

GPIO_Pin GPIO_InitPin(GPIO_Config* config);
void     GPIO_SetDir(GPIO_Pin* gpio, GPIO_Dir dir);
void     GPIO_Set(GPIO_Pin* gpio);
void     GPIO_Clear(GPIO_Pin* gpio);
uint8_t  GPIO_Read(GPIO_Pin* gpio);
void     GPIO_Write(GPIO_Pin* gpio, uint8_t val);

#endif
