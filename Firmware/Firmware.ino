/*
 * Project Origin: Fablab Karlsruhe e.V.
 * Maintainer: David Bauer
 */


#include "setup.h"            // project folder

#include "pinning.h"          // project folder
#include "adressen.h"         // project folder
#include "misc.h"             // project folder

#define ino_file
	#include "globals.h"        // Globale Variablen.
#undef ino_file 

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
	TFT_ILI9163C tft = TFT_ILI9163C(TFT_CS, 14);
	MCP23S17 statusLeds(&SPI, EXPANDER_CS, 0);
#endif


void setup() {
	int i;
	// start serial port
	Serial.begin(9600);
	#ifdef SERIAL_OUTPUT
		Serial.println("Booting LabNet Node");
		Serial.print("HW: ");
		Serial.println(HW);
		Serial.print("NODE: ");
		Serial.println(NODE);
		Serial.print("NODEID: ");
		Serial.println(NODEID);
	#endif
  
	#if RITTAL_LEISTEN
		Rittal0.init();
		t.every(10, rittalTask);
		t.every(5000, rittalTest);
	#endif

	#if CAN_BUS
		// init can bus, baudrate: 125k
		can0.begin(MCP_STDEXT,CAN_125KBPS, MCP_16MHZ);
		can0.setMode(MCP_NORMAL);
		t.every(1000, sendValues1000);
		t.every(5000, sendValues5000);
	#endif

	#if ADDON_LCD_LED
		Serial.print("Addon LCD LED V1.0");
		statusLeds.begin();
		for(i=0;i<16;i++){
			statusLeds.pinMode(i,OUTPUT);
		}
		lcdHelloWorld();
	#endif

  
}

void loop() {
	t.update();
	readVoltages();
}

void readVoltages(){
	voltage24 = analogRead(V24) * (5.0 / 1023.0/1.5*11.5);
}

#if CAN_BUS
	unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

	void sendValues1000(){
		// send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
		can0.sendMsgBuf(CT_ANNCOUNCE+0x01, 1, 8, stmp);
	}

	void sendValues5000(){
		// send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
		can0.sendMsgBuf(CT_ANNCOUNCE+0x01, 1, 8, stmp);
	}
#endif


#if ADDON_LCD_LED
	 void lcdHelloWorld(){
		tft.setCursor(0, 0);
		tft.println("Hello World!"); 
	}

  	void statusLedsTest(){
		static uint16_t s = 0xFFFF;
		if(s== 0xFFFF)
			s=0x0000;
		else
			s=0xFFFF;
		statusLeds.writePort(s);
	}
#endif

#if RITTAL_LEISTEN
  void rittalTest(){
	static uint8_t s =0;
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
