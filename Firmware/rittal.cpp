#include "rittal.h"            // project folder
#include "Arduino.h"
#include "SoftwareSerial.h"
//SoftwareSerial debug(SOFTSERIAL_RX, SOFTSERIAL_TX); // RX, TX

#define COMMAND_REQ 'I'
#define COMMAND_DATA 'J'

#define STATE_INIT 0
#define STATE_INIT_START 1
#define STATE_INIT_REQ 2
#define STATE_INIT_READ 3
#define STATE_INIT_RETRY 4
#define STATE_INIT_AUSWERTEN 4
#define STATE_WAIT 10
#define STATE_RETRY_REQ 11
#define STATE_RETRY_COMMAND 12
#define STATE_SEND_COMMAND 20
#define STATE_SEND_REQ 21
#define STATE_READ_REQ 30
#define STATE_READ_COMMAND 31

#define WAIT_LOOPS 10
#define ANSWER_LENGHT 50

#define UPDATE_EVERY 1000 // in millis, 1000 = 1s
#define NEW_AVAIL_CHECK 50000 // aktuell nicht benutzt. stattdessen werden einfach alle jede UPDATE_EVERY abgefragt.

#define MAX_TRYS 3

uint8_t trys=0;
uint8_t init_next = 0;
uint8_t id = 0; // state merker, nächste ID für abfrage
int state = 0;
uint8_t count = 0, answer[ANSWER_LENGHT];
uint32_t loops = 0;
uint8_t canbuf[8];

#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
	SoftwareSerial debug(SOFTSERIAL_RX, SOFTSERIAL_TX); // RX, TX
#endif


/* NOTES NOTES NOTES NOTES NOTES NOTES NOTES 

* millis() überlauf wird nicht überwacht. nach spätestens 50 tage sollte Hardware rebootet werden oder millis() überlauf abchecken.
* nur ID 1-4 sind valid
*

State Maschine komplexer als gedacht. 
25.10.17 - großer umbau gestartet - ist nicht mehr debugbar

NOTES NOTES NOTES NOTES NOTES NOTES NOTES */

Rittal::Rittal(){
	
}

void Rittal::sendReq(uint8_t id){
	// I0148
	digitalWrite(RS485_RE,1); // 1
	Serial.write(0x02);
	Serial.write("I0");
	Serial.write(0x30+id);
	Serial.write("48");
	Serial.write(0x03);
	Serial.flush();
	digitalWrite(RS485_RE,0); // 0 0
}

uint8_t debugStr [40];
uint8_t debugChecksum = 0x00;

void Rittal::sendData(rittal_s r){
	int i;
	// \02 start byte
	// J 0 befehl 
	// 01 1 id
	// aaaaaaaaaa 3 name
	// 00 14 switch state
	// 00000 00000 00000 0000 16 NC
	// F 35 upper alarm
	// 000 36 NC
	// 0 39 lower alarm
	// 3D 40 checksum
	// \03 end byte
	uint8_t str [] = "J00aaaaaaaaaa000000000000000000000F0000\0";
	//				  J03ABCDEFGHIJ0:2000000000000007£00F0000
	str[2] += r.id;
	for(i=0;i<10;i++){
		str[3+i] = r.name[i];
	}
	int a = 0;
	if(r.d1==true)a += 0x01;
	if(r.d2==true)a += 0x02;
	if(r.d3==true)a += 0x04;
	if(r.d4==true)a += 0x08;
	if(a<=9)str[14] += a;
	else{
		str[14] = a-10+'A';
	}
	if(r.d5==true)str[13] += 0x01;
	if(r.d6==true)str[13] += 0x02;
	str[34] = r.max;
	str[38] = r.min;
	uint8_t checksum = 0x00;
	i=0;
	while(str[i]){
		checksum = checksum ^ str[i];
		i++;
	}
	digitalWrite(RS485_RE,1);

	Serial.write(0x02);
	i=0;
	while(str[i]){
		Serial.write(str[i]);
		i++;
	}
	if(checksum <= 0x0F){
		Serial.print('0');
	}
	Serial.print(checksum, HEX);
	Serial.write(0x03);
	Serial.flush();
	digitalWrite(RS485_RE,0);
	for(int i=0;i<40;i++){
		debugStr[i] = str[i];
	}
	debugChecksum = checksum;
}

void Rittal::saveReqAnswer(){
	// i01N15AAAAAAAAAA00F3010000010100000F000020
	// i01N15ActivePSM 01D7010000010100000F000065
	//for(int j=0;j<10;j++){
	//	if(answer[7+j] == 0x00){
	//		break
	//	}
	//	leiste[id-1].name[j] = answer[7+j];
	//}
	/*leiste[id-1].d1 = answer[32] & 0x01;
	leiste[id-1].d2 = answer[32] & 0x02 >> 1;
	leiste[id-1].d3 = answer[32] & 0x04 >> 2;
	leiste[id-1].d4 = answer[32] & 0x08 >> 4;
	leiste[id-1].d5 = answer[31] & 0x01;
	leiste[id-1].d6 = answer[31] & 0x02 >> 1;*/
	leiste[id-1].min = answer[34];
	leiste[id-1].max = answer[36];
	leiste[id-1].current = (answer[23]-0x30)*1000+(answer[24]-0x30)*100+(answer[25]-0x30)*10+(answer[26]-0x30)*1;
	//leiste[id-1].lastReq = millis()-1;
}

void Rittal::retrySwitch(){
	if(state == STATE_READ_COMMAND){
		state = STATE_RETRY_COMMAND; // try again
	}else{
		state = STATE_RETRY_REQ; // try again
	}
}


void Rittal::sendDebug(char arr[]){
	#ifdef SERIAL_DEBUG
	debug.print(state);
	debug.print("-");
	debug.print(answer[0]);
	debug.print(answer[1]);
	debug.print(answer[2]);
	debug.print(answer[3]);
	debug.print(answer[4]);
	debug.print(answer[5]);
	//debug.print("-");
	//debug.print(id);
	//debug.print("-");
	//debug.print(trys);
	debug.print("-");
	debug.print(loops);
	//debug.print("-");
	//debug.print(count);
	//debug.print("-");
	//debug.print(leiste[id-1].changed);
	//debug.print("-");
	//debug.print(leiste[id-1].avail);
	debug.print("-");
	debug.print(arr);
	//debug.print("-");
	//for(int i=0;i<20;i++){
	//	debug.print((char)debugStr[i]);
	//}
	//debug.print("-");
	//debug.print((char)debugChecksum,HEX);
	//debug.print("-");
	//debug.print(leiste[id-1].lastReq + UPDATE_EVERY);
	//debug.print("-");
	//debug.print(millis());
	
	debug.println();
	//canbuf[0] = id+0x30;
	//canbuf[1] = trys+0x30;
	//canbuf[2] = debugStr[13]; // Leisten Status
	//canbuf[3] = debugStr[14]; // Leisten Status
	//canbuf[4] = debugStr[34]; //min Ampere
	//canbuf[5] = debugStr[38]; //max Ampere
	//canbuf[6] = "_";
	//canbuf[7] = "_";
	//can0.sendMsgBuf(buildAdr(TT_ANNOUNCE,ANNOUNCE_DEBUG1+id-1), 1, 8, canbuf);
	#endif
}

void Rittal::sendRittalAnnounce(uint8_t rid){
	canbuf[0] = (uint8_t)(leiste[rid-1].current & 0xFF00)>>8;
	canbuf[1] = (uint8_t)(leiste[rid-1].current & 0xFF);
	canbuf[2] = leiste[rid-1].d1;
	canbuf[3] = leiste[rid-1].d2;
	canbuf[4] = leiste[rid-1].d3;
	canbuf[5] = leiste[rid-1].d4;
	canbuf[6] = leiste[rid-1].d5;
	canbuf[7] = leiste[rid-1].d6;
	//debug.print(canbuf[0]);
	//debug.print(canbuf[1]);
	//debug.print(canbuf[2]);
	//debug.print(canbuf[3]);
	//debug.print(canbuf[4]);
	//debug.print(canbuf[5]);
	//debug.print(canbuf[6]);
	//debug.print(canbuf[7]);
	//debug.print("-");
	//debug.print(buildAdr(TT_ANNOUNCE,ANNOUNCE_RITTAL1+rid-1));
	//debug.println();
	can0.sendMsgBuf(buildAdr(TT_ANNOUNCE,ANNOUNCE_RITTAL1+rid-1), 1, 8, canbuf);
}


void Rittal::task(){
	//sendDebug("");
	switch(state){
		case STATE_INIT: 			// =========================================================================    0
			// alle merker Initilialisieren
			id = 0;
			for(int i = 0; i<ANSWER_LENGHT; i++)answer[i] = 0x00; //clear answer
			state = STATE_INIT_START;
			break;
		case STATE_INIT_START: 		// =========================================================================    1
			if (id >= 4){
				state = STATE_WAIT;
				id=0;
			}else{
				state = STATE_INIT_REQ;
				for(int i = 0; i<ANSWER_LENGHT; i++)answer[i] = 0x00; //clear answer
				id++;
				trys=0;
				loops=0;
				count=0;
			}
			break;
		case STATE_INIT_REQ: 		// =========================================================================   2
			sendReq(id);
			while(Serial.available()){Serial.read();}
			state = STATE_INIT_READ;
			leiste[id-1].lastReq = millis();
			break;
		case STATE_INIT_READ: 		// =========================================================================   3
			if(Serial.available() > 0){
				//sendDebug("Serial.available() > 0");
				loops=0; // loops zurücksetzen damit jedes zeichen WAIT_LOOPS zeit hat. 
				answer[count] = Serial.read();
				sendDebug(answer[count]);
				if(answer[0] != 0x02){
					//sendDebug("answer[0] != 0x02");
					//state = STATE_INIT_RETRY; // try again
					count=0;
				}
				if(answer[count] == 0x03){ // 0x03 == endbyte
					//sendDebug("answer[count] == 0x03");
					if(answer[3]==id+0x30){
						saveReqAnswer();
						state = STATE_INIT_START; // auswerten
					}else{
						state = STATE_INIT_RETRY; // auswerten
					}
					
				}
				count++; // zählt die empfangenen Zeichen
				if(count >= ANSWER_LENGHT){
					//sendDebug("count >= ANSWER_LENGHT");
					state = STATE_INIT_RETRY; // auswerten
				}
			}else{
				loops++;
				if(loops>WAIT_LOOPS){
					//sendDebug("loops>WAIT_LOOPS");
					state=STATE_INIT_RETRY;
				}
			}
			break;
		case STATE_INIT_RETRY: 		// =========================================================================   4
			trys++;
			if(trys>=MAX_TRYS){
				//sendDebug("maxTry");
				state = STATE_INIT_START;
			}else{
				state = STATE_INIT_REQ;
			}
			break;
		case STATE_WAIT: 			// =========================================================================   10
			// wait for changes plugs or timer (request for current)
			// checken ob eine leiste sich verändert hat, dann updaten und wieder status abfragen.
			id=0;
			for(int i = 0; i<4; i++){
					if(leiste[i].changed == true){
						id = leiste[i].id;
						state = STATE_SEND_COMMAND;
						trys=0;
						break;
					}
			}
			if(id==0){ // es hat sich keine leiste verändert, aber wir schaun ob eine leiste schon länger nicht abgerfragt wurde.
				for(int i = 0; i<4; i++ ){
					if(millis() > (leiste[i].lastReq + UPDATE_EVERY)){
						id = leiste[i].id;
						state = STATE_SEND_REQ;
						trys=0;
						break;
					}
				}
			}	
			break;
		case STATE_RETRY_COMMAND:	// =========================================================================    12
			trys++;
			state = STATE_SEND_COMMAND;
			if(trys >= MAX_TRYS){
				leiste[id-1].changed = false;
				leiste[id-1].error = CAN_ERROR_UPDATE;
				leiste[id-1].error_count++;
				state = STATE_WAIT;
				sendDebug("CoFail");
			}
			break;
		case STATE_RETRY_REQ: 		// =========================================================================    11
			trys++;
			state = STATE_SEND_REQ;
			if(trys >= MAX_TRYS){
				state = STATE_WAIT;
				//sendDebug("ReFail");
			}
			break;
		case STATE_SEND_REQ: 		// =========================================================================    21
			count=0;
			loops=0;
			sendReq(id);
			leiste[id-1].lastReq = millis();
			while(Serial.available()){Serial.read();}
			for(int i = 0; i<ANSWER_LENGHT; i++)answer[i] = 0x00; //clear answer
			state = STATE_READ_REQ;
			break;
		case STATE_SEND_COMMAND: 	// =========================================================================    20
			count=0;
			loops=0;
			sendData(leiste[id-1]);
			while(Serial.available()){Serial.read();}
			for(int i = 0; i<ANSWER_LENGHT; i++)answer[i] = 0x00; //clear answer
			state = STATE_READ_COMMAND;
			break;
		case STATE_READ_REQ: 		// =========================================================================    30
		case STATE_READ_COMMAND: 	// =========================================================================    31
			loops++;
			sendDebug("case STATE_READ_COMMAND:");
			if(loops > WAIT_LOOPS || count >= ANSWER_LENGHT){
				//trys++;
				if(loops > WAIT_LOOPS){
					//sendDebug("loops > WAIT_LOOPS");
				}
				if(count >= ANSWER_LENGHT){
					//sendDebug("count >= ANSWER_LENGHT");
				}
				retrySwitch(); // try again
			}else{
				if(Serial.available() > 0){
					//sendDebug("Serial.available() > 0");
					loops=0; // loops zurücksetzen damit jedes zeichen WAIT_LOOPS zeit hat. 
					answer[count] = Serial.read();
					if(answer[0] != 0x02){
						//sendDebug("first byte not 0x02");
						//state = STATE_INIT_RETRY; // try again
						count=0;
					}
					if(answer[count] == 0x03){ // 0x03 == endbyte
						//sendDebug("answer[count] == 0x03");
						if(answer[1] == 'j'){
								leiste[id-1].changed = false;
								state = STATE_WAIT;
								leiste[id-1].error = CAN_NO_ERROR;
								sendDebug("CoSucc");
								sendRittalAnnounce(id);
						}else if(answer[1] == 'i'){
							if(answer[3]==id+0x30){
								saveReqAnswer();
								leiste[id-1].error = CAN_NO_ERROR;
								state = STATE_WAIT; // auswerten
								//sendDebug("ReSucc");
								//sendRittalAnnounce(id);
							}else{
								retrySwitch();
							}
						}else{
							retrySwitch();
						}
					}
					count++; // zählt die empfangenen Zeichen
					if(count >= ANSWER_LENGHT){
						//sendDebug("count >= ANSWER_LENGHT");
						retrySwitch(); // auswerten
					}
					if(answer[0] != 0x02){
						//sendDebug("count >= ANSWER_LENGHT");
						retrySwitch();
					}
				}else{
					//loops++;
					if(loops>WAIT_LOOPS){
						retrySwitch();
					}
				}
			}
			break;
		default:
			break;
	}
}

void Rittal::reset(uint8_t id){
	id = id-1;
	leiste[id].id = id+1;
	leiste[id].min = '0'; // min Ampere, default 0
	leiste[id].max = 'F'; // max Ampere, default 16
	leiste[id].d1 = 0;  // Dose 1
	leiste[id].d2 = 0;  // Dose 2
	leiste[id].d3 = 0;  // Dose 3
	leiste[id].d4 = 0;  // Dose 4
	leiste[id].d5 = 0;  // Dose 5
	leiste[id].d6 = 0;  // Dose 6
	leiste[id].changed = false;  // changed = false
	leiste[id].lastReq = 0;  // wenn lastReq 0 ist, dann wurde die leiste noch nie abgefragt.
	leiste[id].name[0] = 'A'+id;
	leiste[id].name[1] = 'A'+id;	
	leiste[id].name[2] = 'A'+id;	
	leiste[id].name[3] = 'A'+id;	
	leiste[id].name[4] = 'A'+id;	
	leiste[id].name[5] = 'A'+id;	
	leiste[id].name[6] = 'A'+id;	
	leiste[id].name[7] = 'A'+id;	
	leiste[id].name[8] = 'A'+id;	
	leiste[id].name[9] = 'A'+id;
	leiste[id].error_count = 0;
}

void Rittal::resetAll(){
	for(int i = 1; i<5; i++){
		reset(i);
	}
}

void Rittal::init(){
	#ifdef SERIAL_DEBUG
		debug.begin(57600);
  		debug.println("power_hub up");
  	#endif
	pinMode(RS485_RE, OUTPUT);
	init_next = 0;
	resetAll();
}

void Rittal::setSocket(uint8_t id, uint8_t socket, uint8_t value){
	if(id != 0 && id <= 4 && socket != 0 && socket <=6){
		id = id-1;
		switch(socket){
			case 1: leiste[id].d1 = value;break;
			case 2: leiste[id].d2 = value;break;
			case 3: leiste[id].d3 = value;break;
			case 4: leiste[id].d4 = value;break;
			case 5: leiste[id].d5 = value;break;
			case 6: leiste[id].d6 = value;break;
		}
		leiste[id].changed = true;
	}
}

void Rittal::setMax(uint8_t id, uint8_t value){
	leiste[id-1].changed = true;
}

void Rittal::setMin(uint8_t id, uint8_t value){
	leiste[id-1].changed = true;
}

/* unused 
void Rittal::setAvail(uint8_t id, uint8_t value){
	if(leiste[id-1].avail != value){
		leiste[id-1].avail = value;
		if(value == true){ // wir resetten alle dosen.  
			leiste[id-1].d1 = 0;  // Dose 1
			leiste[id-1].d2 = 0;  // Dose 2
			leiste[id-1].d3 = 0;  // Dose 3
			leiste[id-1].d4 = 0;  // Dose 4
			leiste[id-1].d5 = 0;  // Dose 5
			leiste[id-1].d6 = 0;  // Dose 6
		}
	}
} */