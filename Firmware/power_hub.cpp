#include "power_hub.h"            // project folder

void read_fuses();
void readVoltage();

void power_hub_task(){
	read_fuses();	// ~40µs
	readVoltage(); 	// ~100µs
}

void power_hub_init(){
	int i;
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
}

void read_fuses(){
	// 5µs pro digitalRead
	int i;
	for(i=0;i<8;i++){
		if(digitalRead(fuse[i])==1){	
			fuseStatus[i] = FUSE_STATUS_OK;
		}else{
			fuseStatus[i] = FUSE_STATUS_KAPUTT;
		}
	}
}

void readVoltage(){
	voltage24 = analogRead(V24) * (5.0 / 1023.0/1.5*11.5);
}