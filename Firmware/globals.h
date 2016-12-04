#include <arduino.h>

#ifdef ino_file
	#define EXTERN 
#else
	#define EXTERN extern
#endif

EXTERN float temperatur[8]; 	// 1wire sensor onboard = 0 
EXTERN int nodeStatus; 			// 1=ok, 2=warn, 3=error
EXTERN float voltage24; 		// nur powerhub, 24V Versorgung
EXTERN uint8_t fuse[8];
EXTERN uint8_t fuseStatus[8]; 	// 1=ok,0=undef,2=kaputt
EXTERN uint8_t blockSerial; 	// rs485 und HR20 würden sich stören. 

EXTERN String serialRX;

// CAN Telegramm Structs/Unions

EXTERN rittal_u rittal[4];