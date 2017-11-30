
// Nichts ändern!
#define NO_HW_TYPE 0
#define POWER_HUB 1
#define BASIS 2
#define BRIDGE 3
#define MCP23S17_STATUS_ADR 0

// AB HIER ÄNDERN
#define HWID 0x01  		// NODEID besteht aus HW + HWID // max 0xFF
#define HW POWER_HUB

// uncomment if not needed
#define RITTAL_LEISTEN
#define CAN_BUS
#define ONEWIRE
#define SERIAL_OUTPUT 		// konflikte mit HR20 ??
#define SOFTSERIAL

/*
 * Vorhandene Addons auf der Basis und evtl Einstellungen
 */
#if HW == BASIS
	#define ADDON_LCD_LED 	// Auch für HR20
	// #define ADDON_MOTOR
	// #define ADDON_RGBW
	// define ADDON_XYZ
#endif
