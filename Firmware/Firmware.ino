/*
 * Project Origin: Fablab Karlsruhe e.V.
 * Maintainer: David Bauer
 */

 #define VERSION "0.1"


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
	TFT_ILI9163C tft = TFT_ILI9163C(TFT_CS, TFT_RS);
	MCP23S17 statusLeds(&SPI, EXPANDER_CS, MCP23S17_STATUS_ADR);
#endif


void setup() {
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
		fuse[0] = FUSE0;
		fuse[1] = FUSE1;
		fuse[2] = FUSE2;
		fuse[3] = FUSE3;
		fuse[4] = FUSE4;
		fuse[5] = FUSE5;
		fuse[6] = FUSE6;
		fuse[7] = FUSE7;
		for(i=0;i<8;i++){
			pinMode(fuse[i], INPUT);
			fuseStatus[i] = FUSE_STATUS_UNDEF;
		}
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
	readVoltages();
}

void SerPrintLn(const char str[]){
	#ifdef SERIAL_OUTPUT
		Serial.println("Addon LCD LED V1.0");
	#endif
}

void SerPrint(const char str[]){
	#ifdef SERIAL_OUTPUT
		Serial.print("Addon LCD LED V1.0");
	#endif
}

void readVoltages(){
	voltage24 = analogRead(V24) * (5.0 / 1023.0/1.5*11.5);
}

#if HW == POWER_HUB
	void readFuses(){
		for(i=0;i<8;i++){
			if(digitalRead(fuse[i])==1){	
				fuseStatus[i] = FUSE_STATUS_OK;
			}else{
				fuseStatus[i] = FUSE_STATUS_KAPUTT;
			}
		}
	}
#endif

#ifdef CAN_BUS
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


#ifdef ADDON_LCD_LED
	 void lcdHelloWorld(){
		tft.setCursor(0, 0);
		tft.println("Hello World!"); 
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
