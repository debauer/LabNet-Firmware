#include <arduino.h>

#ifndef STRUCTS_H_   /* Include guard */
#define STRUCTS_H_

struct rittal_s{
		uint8_t min; // min Ampere, default 0
		uint8_t max; // max Ampere, default 16
		uint8_t d1;  // Dose 1
		uint8_t d2;  // Dose 2
		uint8_t d3;  // Dose 3
		uint8_t d4;  // Dose 4
		uint8_t d5;  // Dose 5
		uint8_t d6;  // Dose 6
		uint8_t id;  // id 1-9
		uint8_t name[10]; // name 
		uint8_t changed;
		unsigned long lastReq;
		uint8_t avail;
		uint8_t announce; // if 1 then announce data via CAN
		uint8_t error;
		unsigned long error_count;
		uint16_t current;
		uint8_t config; // byte from EVENT_LOCAL_RITTAL_CONFIG$
};

struct can_message_s{
	uint32_t id;
	uint8_t ext;
	uint8_t len;
	uint8_t *buf;
};


#endif // STRUCTS_H_  