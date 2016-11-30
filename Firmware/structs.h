#include <arduino.h>

#ifndef STRUCTS_H_   /* Include guard */
#define STRUCTS_H_

union rittal_u{
	unsigned char d[8];
	struct rittal_s{
		uint8_t min; // min Ampere, default 0
		uint8_t max; // max Ampere, default 16
		uint8_t d1;  // Dose 1
		uint8_t d2;  // Dose 2
		uint8_t d3;  // Dose 3
		uint8_t d4;  // Dose 4
		uint8_t d5;  // Dose 5
		uint8_t d6;  // Dose 6
	} s;
};

#endif // STRUCTS_H_  