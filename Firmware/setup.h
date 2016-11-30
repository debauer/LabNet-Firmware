
// Nichts ändern!
#define POWER_HUB 1
#define BASIS 2
#define MCP23S17_STATUS_ADR 0

// AB HIER ÄNDERN
#define NODEID 0x01  		// ganze ID besteht aus HW + NODEID 
#define HW BASIS

// uncomment if not needed
// #define RITTAL_LEISTEN
#define CAN_BUS
#define ONEWIRE
#define SERIAL_OUTPUT 		// konflikte mit HR20 ??

/*
 * Vorhandene Addons auf der Basis und evtl Einstellungen
 */
#if HW == BASIS
	#define ADDON_LCD_LED 	// Auch für HR20
	#define ADDON_MOTOR
	#define ADDON_RGBW
	// define ADDON_XYZ
#endif
