#if HW == BASIS
	#define NODEID 0x100 | HWID
#elif HW == POWER_HUB
	#define NODEID 0xF00 | HWID
#elif HW == BRDIGE 
	#define NODEID 0x000 | HWID
#else // alle andern
	#define NODEID 0xA00 | HWID
#endif


#define TT_MASK           	0x1F000000
#define REG_MASK           	0x00000FFF
#define NODE_MASK           0x00FFF000
#define SENSOR_MASK         0x00000FFF

// TT Telegram Type
#define TT_EVENT_GLOBAL   	0x00
#define TT_EVENT_LOCAL    	0x01
#define TT_REGISTER       	0x03
#define TT_ANNOUNCE      	0x04
#define TT_EEPROM_WR      	0x08
#define TT_EEPROM_REQ     	0x09
#define TT_EEPROM_REPLY   	0x0A
#define TT_SF				0x0B

// TT_EVENT_LOCAL & REG_MASK
// 0x000000 -> 0x00000F for SYSTEM
#define EVENT_GlOBAL_LABSTATE			0x000000 	// lab status from the lab switch
#define EVENT_GlOBAL_RITTAL_UPDATE		0x000001 	// one of the nodes wants all rittal plug states

// 0x000030 -> 0x00003F for RITTAL STRIPS
#define EVENT_LOCAL_RESET   			0x000000
#define EVENT_LOCAL_DEFAULTI			0x000011	
#define EVENT_LOCAL_DEFAULTE			0x000012
#define EVENT_LOCAL_RITTAL_CONFIG 		0x000030
#define EVENT_LOCAL_RITTAL1 			0x000031	// set plugs and current
#define EVENT_LOCAL_RITTAL2 			0x000032	// set plugs and current
#define EVENT_LOCAL_RITTAL3 			0x000033	// set plugs and current
#define EVENT_LOCAL_RITTAL4 			0x000034	// set plugs and current
#define EVENT_LOCAL_RITTAL_CONFIG1 		0x000035	// set plugs and current
#define EVENT_LOCAL_RITTAL_CONFIG2 		0x000036	// set plugs and current
#define EVENT_LOCAL_RITTAL_CONFIG3 		0x000037	// set plugs and current
#define EVENT_LOCAL_RITTAL_CONFIG4 		0x000038	// set plugs and current

// TT_REGISTER & REG_MASK
// 0x000000 -> 0x00000F for SYSTEM
#define REGISTER_ID				0x000000
#define REGISTER_NAME1			0x000001
#define REGISTER_NAME2			0x000002
#define REGISTER_UPTIME			0x000003
#define REGISTER_STATUS  		0x000013
#define REGISTER_TIME			0x000014
#define REGISTER_DATE 			0x000015


// TT_ANNOUNCE & REG_MASK
// 0x000000 -> 0x00000F for SYSTEM
#define ANNOUNCE_STARTUP   			0x000001
#define ANNOUNCE_PING				0x000002
#define ANNOUNCE_RESET   			0x000010

// 0x000020 -> 0x00002F for single values
#define ANNOUNCE_FUSES			0x000020

// 0x000030 -> 0x00003F for RITTAL STRIPS
#define ANNOUNCE_RITTAL_ERRORCOUNT  	0x000030  	// 2 Bytes for the errorcount per strip 11223344
#define ANNOUNCE_RITTAL1 				0x000031	// actual plug status and current
#define ANNOUNCE_RITTAL2 				0x000032	// actual plug status and current
#define ANNOUNCE_RITTAL3 				0x000033	// actual plug status and current
#define ANNOUNCE_RITTAL4 				0x000034	// actual plug status and current
#define ANNOUNCE_RITTAL_DEBUG1 			0x000035	// kurzer status String
#define ANNOUNCE_RITTAL_DEBUG2 			0x000036	// kurzer status String
#define ANNOUNCE_RITTAL_DEBUG3 			0x000037	// kurzer status String
#define ANNOUNCE_RITTAL_DEBUG4 			0x000038	// kurzer status String
#define ANNOUNCE_RITTAL_ERROR1 			0x000039    // Error Codes
#define ANNOUNCE_RITTAL_ERROR2 			0x00003A    // Error Codes
#define ANNOUNCE_RITTAL_ERROR3 			0x00003B    // Error Codes
#define ANNOUNCE_RITTAL_ERROR4 			0x00003C    // Error Codes

#define ANNOUNCE_TEMPERATUR1 		0x000040
#define ANNOUNCE_TEMPERATUR2 		0x000041
#define ANNOUNCE_TEMPERATUR3 		0x000042
#define ANNOUNCE_TEMPERATUR4 		0x000043
#define ANNOUNCE_TEMPERATUR5 		0x000044
#define ANNOUNCE_TEMPERATUR6 		0x000045
#define ANNOUNCE_TEMPERATUR7 		0x000046
#define ANNOUNCE_TEMPERATUR8 		0x000047
#define ANNOUNCE_TEMPERATUR9 		0x000048
#define ANNOUNCE_TEMPERATUR10 		0x000049
#define ANNOUNCE_TEMPERATUR11 		0x00004A
#define ANNOUNCE_TEMPERATUR12 		0x00004B
#define ANNOUNCE_TEMPERATUR13 		0x00004C
#define ANNOUNCE_TEMPERATUR14 		0x00004D
#define ANNOUNCE_TEMPERATUR15 		0x00004E
#define ANNOUNCE_TEMPERATUR16 		0x00004F

#define ANNOUNCE_VOLTAGE1 			0x000050
#define ANNOUNCE_VOLTAGE2 			0x000051
#define ANNOUNCE_VOLTAGE3 			0x000052
#define ANNOUNCE_VOLTAGE4 			0x000053
#define ANNOUNCE_VOLTAGE5 			0x000054
#define ANNOUNCE_VOLTAGE6 			0x000055
#define ANNOUNCE_VOLTAGE7 			0x000056
#define ANNOUNCE_VOLTAGE8 			0x000057
#define ANNOUNCE_VOLTAGE9 			0x000058
#define ANNOUNCE_VOLTAGE10 			0x000059
#define ANNOUNCE_VOLTAGE11 			0x00005A
#define ANNOUNCE_VOLTAGE12 			0x00005B
#define ANNOUNCE_VOLTAGE13 			0x00005C
#define ANNOUNCE_VOLTAGE14 			0x00005D
#define ANNOUNCE_VOLTAGE15 			0x00005E
#define ANNOUNCE_VOLTAGE16 			0x00005F
