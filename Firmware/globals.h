#include <arduino.h>

#ifdef ino_file
	#define EXTERN 
	MCP_CAN can0(CAN_CS);
#else
	#define EXTERN extern
	#include "mcp_can.h" 
	extern MCP_CAN can0;
#endif

EXTERN float temperatur[16]; 	// 1wire sensor onboard = 0 
EXTERN int nodeStatus; 			// 1=ok, 2=warn, 3=error
EXTERN float voltages[16]; 		// powerhub 0 = 24v
EXTERN uint8_t fuse[8];
EXTERN uint8_t fuseStatus[8]; 	// 1=ok,0=undef,2=kaputt
EXTERN uint8_t blockSerial; 	// rs485 und HR20 würden sich stören. 

EXTERN String serialRX;

// CAN Telegramm Structs/Unions

EXTERN rittal_s rittal[4];

EXTERN can_message_s can_messages[CAN_BUF_SIZE];

EXTERN uint32_t buildAdr(uint8_t a, uint32_t c);
