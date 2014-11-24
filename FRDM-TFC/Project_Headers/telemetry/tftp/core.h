/* Tftp.h
 * =========================================
 * Team Freescalextric Telemetry Protocol
 * -----------------------------------------
 * When supported by sufficient link layer
 * or data storage services TFTP facilitates
 * the collection of various data from the
 * FRDM-TFC system.
 */

#ifndef TFTP_PROTOCOL_H_
#define TFTP_PROTOCOL_H_

#include "telemetry/tftp/attributes.h"

int8_t Tftp_Send(uint8_t code, void* value, uint16_t size);
int8_t Tftp_Push(uint8_t code, void* value, uint16_t size);

#endif
