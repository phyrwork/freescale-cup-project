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

/* List of attributes and their #includes */
#define TFTP_LINESCAN0 0x01; //All currently placeholder attributes...
#define TFTP_LINESCAN1 0x02; //...codes are likely going to change.
#define TFTP_W_FL      0x03;
#define TFTP_W_FR      0x04;
#define TFTP_W_RL      0x05;
#define TFTP_W_RR      0x06;
#define TFTP_I_RL      0x07;
#define TFTP_I_RR      0x08;
#define TFTP_T_RL      0x09;
#define TFTP_T_RR      0x0A;
#define TFTP_S_RL      0x0B;
#define TFTP_S_RR      0x0C;

int8_t TftpSend(uint8_t key, void* value, size_t size);

#endif
