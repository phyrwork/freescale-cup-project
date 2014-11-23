/* GPIO.c
 * =================================
 * GPIO helper for Kinetis KL25
 * ---------------------------------
 * Assists easy peripheral setup.
 */

#include "io/GPIO.h"

GPIO_MemMapPtr GPIO[] = GPIO_BASE_PTRS;

#define PORT gpio->port
#define PIN  gpio->pin

GPIO_Pin GPIO_InitPin(GPIO_Config* config) {
	GPIO_Pin gpio = (GPIO_Pin) IO_InitPin(&(config->io));
	GPIO_SetDir(&gpio, config->dir);
	return gpio;
}

void GPIO_SetDir(GPIO_Pin* gpio, GPIO_Dir dir) {
	/* Clear bit if input, set if output */
	switch(dir) {
		case GPIO_IN:
				GPIO[PORT]->PDDR &= ~(1 << PIN);
			break;
		case GPIO_OUT:
				GPIO[PORT]->PDDR |= (1 << PIN);
			break;
	}
	return;
}

inline void GPIO_Set(GPIO_Pin* gpio) {
	/* Set bit on output register */
	GPIO[PORT]->PSOR = (1 << PIN);
}

inline void GPIO_Clear(GPIO_Pin* gpio) {
	/* Clear bit on output register */
	GPIO[PORT]->PCOR = (1 << PIN);
}

uint8_t GPIO_Read(GPIO_Pin* gpio) {

	uint32_t val; //Temp storage for logic value.

	if(GPIO[PORT]->PDDR & (1 << PIN))
		/* If true then output */
		val = GPIO[PORT]->PDIR & (1 << PIN);
	else
		/* Else input */
		val = GPIO[PORT]->PDOR & (1 << PIN);

	if(val) return 1;
	else    return 0;
}

void GPIO_Write(GPIO_Pin* gpio, uint8_t val) {
	/* Call appropriate function */
	if(val) GPIO_Set(gpio);
	else    GPIO_Clear(gpio);
}
