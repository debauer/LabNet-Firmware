#include <arduino.h>

#ifdef ino_file
	#define EXTERN 
#else
	#define EXTERN extern
#endif


EXTERN float temperatur[8]; 	// 1wire sensor onboard = 0 
EXTERN int nodeStatus = 1; 		// 1=ok, 2=warn, 3=error
EXTERN float voltage24; 		// nur powerhub, 24V Versorgung
EXTERN uint8_t fuse[8];
EXTERN uint8_t fuseStatus[8]; 	// 1=ok,0=undef,2=kaputt
