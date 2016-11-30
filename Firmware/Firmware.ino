/*
 * Project Origin: Fablab Karlsruhe e.V.
 * Maintainer: David Bauer
 */

 #define VERSION "0.1"


#include "setup.h"            // project folder

#include "pinning.h"          // project folder
#include "adressen.h"         // project folder
#include "defines.h"          // project folder
#include "structs.h"          // project folder

#define ino_file
	#include "globals.h"        // Globale Variablen.
#undef ino_file 

#if HW == POWER_HUB
 	#include "power_hub.h"
#endif

#include <SPI.h>              // Arduino Lib
#include <Timer.h>            // http://playground.arduino.cc/Code/Timer

Timer t;

#ifdef ONEWIRE
	#include <OneWire.h>        // https://github.com/PaulStoffregen/OneWire
#endif

#ifdef CAN_BUS
	#include <mcp_can.h>        // https://github.com/coryjfowler/MCP_CAN_lib
	MCP_CAN can0(CAN_CS);
#endif

#ifdef RITTAL_LEISTEN
	#include "rittal.h" 
	Rittal Rittal0(RS485_RE); 
#endif

#ifdef ADDON_LCD_LED
	#include <MCP23S17.h>       // https://github.com/MajenkoLibraries/MCP23S17
	#include <Adafruit_GFX.h>   // https://github.com/adafruit/Adafruit-GFX-Library
	#include <TFT_ILI9163C.h>   // https://github.com/sumotoy/TFT_ILI9163C
	TFT_ILI9163C tft = TFT_ILI9163C(TFT_CS, TFT_RS);
	MCP23S17 statusLeds(&SPI, EXPANDER_CS, MCP23S17_STATUS_ADR);
#endif


void setup(){
	int i;
	// start serial port
	Serial.begin(9600);
	SerPrintLn("Booting LabNet Node");
	SerPrint("HW: ");
	#if HW == POWER_HUB
		SerPrintLn("POWER HUB");
	#else
		SerPrintLn("BASIS");
	#endif
	SerPrint("NODEID: ");
	SerPrint("Firmware Version: ");
	SerPrintLn(VERSION);
	SerPrintLn("Project Origin: Fablab Karlsruhe e.V.");
	SerPrintLn("Maintainer: David Bauer");

	#if HW == POWER_HUB
		power_hub_init();
		t.every(10, power_hub_task);
	#endif
  
	#ifdef RITTAL_LEISTEN
		Rittal0.init();
		t.every(10, rittalTask);
		t.every(5000, rittalTest);
	#endif

	#ifdef CAN_BUS
		SerPrintLn("init CAN Bus, baudrate: 125k");
		can0.begin(MCP_STDEXT,CAN_125KBPS, MCP_16MHZ);
		can0.setMode(MCP_NORMAL);
		t.every(1000, sendValues1000);
		t.every(5000, sendValues5000);
	#endif

	#ifdef ADDON_LCD_LED
		SerPrintLn("Addon LCD LED V1.0");
		statusLeds.begin();
		for(i=0;i<16;i++){
			statusLeds.pinMode(i,OUTPUT);
		}
		lcdHelloWorld();
	#endif
}

void loop() {
	t.update();
}

void SerPrintLn(const char str[]){
	#ifdef SERIAL_OUTPUT
		Serial.println(str);
	#endif
}

void SerPrint(const char str[]){
	#ifdef SERIAL_OUTPUT
		Serial.print(str);
	#endif
}

#ifdef CAN_BUS
	unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

	uint32_t buildAdr(uint8_t a, uint32_t b, uint32_t c){
		return 0x00000000 | ((uint32_t)(a & 0x1F)) << 24 | ((uint32_t)(b & 0x0FFF)) << 12 | ((uint32_t)(c & 0x0FFF));
	}

	void sendValues1000(){
		// send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
		can0.sendMsgBuf(buildAdr(CT_REGISTER,0x000001,REG_ID), 1, 8, stmp);
	}

	void sendValues5000(){
		// send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
		can0.sendMsgBuf(buildAdr(CT_REGISTER,0x000001,REG_ID), 1, 8, stmp);
	}
#endif


#ifdef ADDON_LCD_LED

	void printLcd(){
		// als Statemaschine sollte das LCD geupdatet werden, sonst wird das delay zu hoch.
		// 
		// 		 HEIZUNG			MENUE			WHATEVER
		//
		//	|---------------| |---------------| |---------------| 
		//	| fortlaufender	| |> Setup 		  | |               |
		// 	| Output des    | |  Reset  	  | |               |
		//  | Nodes		    | |  Values		  | |         	    | 
		//	|_______________| |_______________| |               |
		//	|T:23.1 F:closed| |T:23.1 F:closed| |               |
		//	|---------------| |---------------| |---------------|
		// 
		// 
		static uint8_t state = 0;
		switch(state){
			state 0: // startup
				break;
			default:
				state = 0;
				break;
		}
	}

	 void lcdHelloWorld(){
		tft.setCursor(0, 0);
		tft.println("Hello World!"); 
	}

	void setStatusLedsToTemperature(float in, float min, float max){
		// temperatur auf 8 LEDs skalieren. 
		// zb von 18-26grad. größere bereiche würde quasi keine veränderung auf der skala bedeuten.
		float b, a = (max-min)/8;
		while(int i=0;i<8;i++){
			if(in>a){
				statusLeds.digitalWrite(i,1);
			}else{
				statusLeds.digitalWrite(i,0);
			}
		}
	}

  	void statusLedsTest(){
		static uint16_t s = 0xFFFF;
		if(s == 0xFFFF)
			s = 0x0000;
		else
			s = 0xFFFF;
		statusLeds.writePort(s);
	}
#endif

#ifdef RITTAL_LEISTEN
  void rittalTest(){
	static uint8_t s = 0;
	if(s){
	  Rittal0.set(1,0x3F);
	  s=0;
	}else{
	  Rittal0.set(1,0x00);
	  s=1;
	}
  }

  void rittalTask(){
	Rittal0.task();
  }
#endif

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}