/*
 * Project Origin: Fablab Karlsruhe e.V.
 * Maintainer: David Bauer
 */

#define VERSION "0.2"

#include <EEPROM.h>

#include "setup.h"            // project folder

#include "pinning.h"          // project folder
#include "adressen.h"         // project folder
#include "defines.h"          // project folder
#include "structs.h"          // project folder
#include "SoftwareSerial.h"
#ifdef CAN_BUS
	#include "mcp_can.h"        // https://github.com/coryjfowler/MCP_CAN_lib
	long unsigned int canRxId;
	unsigned char canLen = 0;
	unsigned char canRxBuf[8];
	char canMsgString[128]; 
#endif

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

#ifdef RITTAL_LEISTEN
	#include "rittal.h" 
	Rittal rittal0;
#endif

#ifdef ADDON_LCD_LED
	#include <MCP23S17.h>       // https://github.com/MajenkoLibraries/MCP23S17
	#include <Adafruit_GFX.h>   // https://github.com/adafruit/Adafruit-GFX-Library
	#include <TFT_ILI9163C.h>   // https://github.com/sumotoy/TFT_ILI9163C
	TFT_ILI9163C tft = TFT_ILI9163C(TFT_CS, TFT_RS);
	MCP23S17 statusLeds(&SPI, EXPANDER_CS, MCP23S17_STATUS_ADR);
#endif

//#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
	SoftwareSerial debug(SOFTSERIAL_RX, SOFTSERIAL_TX); // RX, TX
#endif

void saveEeprom(){

}

void readEeprom(){

}


void setup(){
	int i;
	// start serial port
	Serial.begin(19200);
	SerPrintLn("Booting LabNet Node");
	SerPrint("HW: ");
	#if HW == POWER_HUB
		SerPrintLn("POWER HUB");
	#else
		SerPrintLn("BASIS");
	#endif
	SerPrint("NODEID: 0x");
    Serial.println(NODEID, HEX);
	SerPrint("Firmware Version: ");
	SerPrintLn(VERSION);
	SerPrintLn("Project Origin: Fablab Karlsruhe e.V.");
	SerPrintLn("Maintainer: David Bauer");

	#if HW == POWER_HUB
		power_hub_init();
		t.every(500, power_hub_task);
	#endif

	#ifdef CAN_BUS
		SerPrintLn("init CAN Bus - Baudrate: 125k, 16MHz Quarz, Extendet");
		can0.begin(MCP_STDEXT,CAN_125KBPS, MCP_16MHZ);
		can0.setMode(MCP_NORMAL);
		t.every(1000, can1000);
		t.every(5000, can5000);
		t.every(100000, canRittalStatus); //1m
		t.every(1000, canSensors); //1s
		#ifdef RITTAL_LEISTEN
			t.every(1000, canRittal);
		#endif
		can0.sendMsgBuf(buildAdr(TT_ANNOUNCE,ANNOUNCE_STARTUP), 1, 8, (byte*)"Im here!");
	#endif

	#ifdef RITTAL_LEISTEN
		rittal0.init();
		rittal0.resetAll();
		t.every(20, rittalTask);
		t.every(5000, rittalTest);
	#endif

	#ifdef ADDON_LCD_LED
		tft.begin();
		statusLeds.begin();
		delay(100);
		
		SerPrintLn("Addon LCD LED V1.0");
		delay(100);
		for(i=0;i<16;i++){
			statusLeds.pinMode(i,OUTPUT);
		}
		t.every(1000, lcdHelloWorld);
		t.every(500, statusLedsTest);
	#endif

	for(i=0;i<16;i++){
		temperatur[i] = 99999.9;
	}
	for(i=0;i<16;i++){
		voltages[i] = 99999.9;
	}
	#ifdef SERIAL_DEBUG
		debug.begin(57600);
	#endif
}

void loop() {
	t.update();
	canRcv();
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
	uint8_t stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	uint8_t buf[8];

	// CT ID, Register/Sensor
	uint32_t buildAdr(uint8_t a, uint32_t c){
		return 0x00000000 | ((uint32_t)(a & 0x1F)) << 24 | ((uint32_t)(NODEID & 0x0FFF)) << 12 | ((uint32_t)(c & 0x0FFF));
	}

	void canRcv(){
		if(can0.checkReceive() == CAN_MSGAVAIL){
			can0.readMsgBuf(&canRxId, &canLen, canRxBuf);
			#ifdef RITTAL_LEISTEN

				// CAN RITTAL 1-4
				if((canRxId & 0x1FFFFFFF) >= buildAdr(TT_EVENT_LOCAL,EVENT_LOCAL_RITTAL1) && (canRxId & 0x1FFFFFFF) <= buildAdr(TT_EVENT_LOCAL,EVENT_LOCAL_RITTAL4)){
					for(byte i = 0; i<6; i++){
						if(canRxBuf[2+i] != 0x02){
							if(canRxBuf[2+i]==1){
								rittal0.setSocket((canRxId & 0x0F),i+1,true);
							}else if(canRxBuf[2+i]==0){
								rittal0.setSocket((canRxId & 0x0F),i+1,false);
							}
						}
					}
					//debug.println("setSocket");
				}
				if((canRxId & 0x1FFFFFFF) == buildAdr(TT_EVENT_GLOBAL,EVENT_GlOBAL_LABSTATE)){
					// EVENT_GlOBAL_LABSTATE
					// first byte = status as uint8_T
					// next 7 bytes ASCII for debug
					// status byte
					// 0x00 = LAB now Closed - event at the moment of turning switch off
					// 0x01 = LAB now Opened - event at the moment of turning switch on
					// 0x02 = LAB is Closed - event every x seconds for new nodes
					// 0x03 = LAB is Opened - event every x seconds for new nodes
					if(canRxBuf[0] <= 0x01){
						for(byte i = 0; i<4; i++){
							for(byte j = 0; j<6; j++){
								if(canRxBuf[0] == 0x00){
									if((0x0F && rittal[i].config) == 0x03 || (0x0F && rittal[i].config) == 0x06 || (0x0F && rittal[i].config) == 0x07 ){ // ON   AT LAB OFF - SAME AT LAB ON
										rittal0.setSocket(i,j,true);
									}
									
									if((0x0F && rittal[i].config) == 0x04 || (0x0F && rittal[i].config) == 0x05 || (0x0F && rittal[i].config) == 0x08 ){ // ON   AT LAB OFF - SAME AT LAB ON
										rittal0.setSocket(i,j,false);
									}
								}
								if(canRxBuf[0] == 0x01){
									if((0x0F && rittal[i].config) == 0x03 || (0x0F && rittal[i].config) == 0x05 || (0x0F && rittal[i].config) == 0x07 ){ // ON   AT LAB OFF - SAME AT LAB ON
										rittal0.setSocket(i,j,false);
									}
									if((0x0F && rittal[i].config) == 0x01 || (0x0F && rittal[i].config) == 0x06 || (0x0F && rittal[i].config) == 0x08 ){ // ON   AT LAB OFF - SAME AT LAB ON
										rittal0.setSocket(i,j,true);
									}
								}
							}
						}
					}
				}
				// CAN RITTAL CONFIG 1-4
				if((canRxId & 0x1FFFFFFF) >= buildAdr(TT_EVENT_LOCAL,EVENT_LOCAL_RITTAL_CONFIG1) && (canRxId & 0x1FFFFFFF) <= buildAdr(TT_EVENT_LOCAL,EVENT_LOCAL_RITTAL_CONFIG4)){
					for(byte i = 0; i<6; i++){
						// low nibble is for labstatus changed
						// 0 = SAME AT LAB OFF - SAME AT LAB ON
						// 1 = SAME AT LAB OFF - ON   AT LAB ON
						// 2 = SAME AT LAB OFF - OFF  AT LAB ON
						// 3 = ON   AT LAB OFF - SAME AT LAB ON
						// 4 = OFF  AT LAB OFF - SAME AT LAB ON
						// 5 = OFF  AT LAB OFF - OFF  AT LAB ON
						// 6 = ON   AT LAB OFF - ON   AT LAB ON
						// 7 = ON 	AT LAB OFF - OFF  AT LAB ON
						// 8 = OFF 	AT LAB OFF - ON   AT LAB ON

						// high nibble is for default
						// 0 = DEFAULT OFF AND NOT CHANGEABLE
						// 1 = DEFAULT ON  AND NOT CHANGEABLE
						// 2 = DEFAULT OFF AND 	   CHANGEABLE
						// 3 = DEFAULT ON  AND 	   CHANGEABLE	
						rittal[i].config = canRxBuf[i];	
					}
				}
			#endif
				// TT_EEPROM_WR
				if((canRxId & 0x1FFFFFFF) >= buildAdr(TT_EEPROM_WR,0x00000) && (canRxId & 0x1FFFFFFF) <= buildAdr(TT_EEPROM_WR,0xFFFFFF) ){
					for(int i =0; i<canLen;i++){
						EEPROM.write((canRxId & 0xFFFFFF)+i,canRxBuf[i]);
					}
				}
				// TT_EEPROM_WR
				if((canRxId & 0x1FFFFFFF) >= buildAdr(TT_EEPROM_REQ,0x00000) && (canRxId & 0x1FFFFFFF) <= buildAdr(TT_EEPROM_WR,0xFFFFFF) ){
					for(int i =0; i<canLen;i++){
						buf[i] = EEPROM.read((canRxId & 0xFFFFFF)+i);

					}
					can0.sendMsgBuf(buildAdr(TT_EEPROM_REPLY,canRxId & 0xFFFFFF), 1, canLen, buf);
				}
			
					//sprintf(canMsgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (canRxId & 0x1FFFFFFF), canLen);
					//debug.print(canMsgString);
					//for(byte i = 0; i<canLen; i++){
					//	sprintf(canMsgString, " 0x%.2X", canRxBuf[i]);
					//	debug.print(canMsgString);
					//}
		}
	}

	uint8_t floatToBuf(float t){
		if(t != 99999.9){
			uint32_t ui;
			long l = *(long*) &t;
			buf[0] = l & 0x00FF;
    		buf[1] = (l >> 8) & 0x00FF;
    		buf[2] = (l >> 16) & 0x00FF;
    		buf[3] = l >> 24;
    		buf[4] = '+';
    		if(t < 0.0){
    			buf[4] = '-';
    			t = t*-1;
    		}
    		ui = (uint32_t)t;
    		if(ui>=999){
    			buf[5] = '9';buf[6] = '9';buf[7] = '9';
    		}else{
    			buf[5] = '0'+(uint8_t)ui/100;
    			buf[6] = '0'+(uint8_t)(ui-(ui/100)*100)/10;
    			buf[7] = '0'+(uint8_t)ui-(ui/100)*100-((ui-(ui/100)*100)/10)*10;
			}
		}
	}

	void canSensors(){
		can0.sendMsgBuf(buildAdr(TT_ANNOUNCE,ANNOUNCE_FUSES), 1, 8, fuseStatus);
		/*for(int i=0;i<16;i++){
			if(floatToBuf(temperatur[i]))can0.sendMsgBuf(buildAdr(TT_ANNOUNCE,ANNOUNCE_TEMPERATUR1+i), 1, 8, buf);
			if(floatToBuf(voltages[i]))can0.sendMsgBuf(buildAdr(TT_ANNOUNCE,ANNOUNCE_VOLTAGE1+i), 1, 8, buf);
		}	*/
	}

	void canRittalStatus(){ // sendet alle 1m
		//can0.sendMsgBuf(buildAdr(TT_EVENT_LOCAL,ANNOUNCE_PING), 1, 8, (byte*)"Im alive");
	}

	void can1000(){ // sendet alle 1000ms
		// send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
		//can0.sendMsgBuf(buildAdr(TT_REGISTER,ANNOUNCE_PING), 1, 8, "Im alive");
	}

	void can5000(){ // sendet alle 5000ms
		// send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
		can0.sendMsgBuf(buildAdr(TT_ANNOUNCE,ANNOUNCE_PING), 1, 8, (byte*)"Im alive");
	}

	#ifdef RITTAL_LEISTEN
	void canRittal(){
		for(int i=0;i<4;i++){
			//can0.sendMsgBuf(buildAdr(TT_ANNOUNCE,ANNOUNCE_RITTAL1+i), 1, 8, rittal[i].d);
		}
	}
	#endif
#endif

#ifdef RITTAL_LEISTEN
  void rittalTest(){
	static uint8_t s = 0;
	/* if(s){
	  rittal0.set(1,0x3F);
	  s=0;
	}else{
	  Rittal0.set(1,0x00);
	  s=1;
	} */
  }

  void rittalTask(){
	rittal0.task();
  }

  
#endif


#ifdef ADDON_LCD_LED

	/*void printLcd(){
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
	}*/

	 void lcdHelloWorld(){
	 	static uint32_t a = 10;
	 	a++;
		if(a >10){
			a=0;
			tft.setCursor(0, 0);
			tft.fillScreen(WHITE);
			tft.setTextColor(BLACK, WHITE);
		} 
		tft.println("Hello World!"); 
	}

	/*void setStatusLedsToTemperature(float in, float min, float max){
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
	}*/

  	void statusLedsTest(){
		static uint16_t s = 0xFFFF;
		if(s == 0xFFFF){
			s = 0x0000;
		}
		else{
			s = 0xFFFF;
		}
		statusLeds.writePort(s);
		
	}
#endif



void serialEvent() {
  /*while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }*/
}
