/**
 * @file main.cpp
 * @author Raquel Teixeira (raquelct97@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2019-12-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <Arduino.h>
#include "Wiegand34.h"
#include "SimpleProtocol.h"

#define DEVICE_ID 0xC5

Wiegand34 wg;
SimpleProtocol sp;

uint16_t msgCount = 0;
unsigned char * buff;

void setup() {
  	// put your setup code here, to run once:
  	sp.begin(115200);  
	wg.begin(18,19); // pinD0, pinD1
}

void loop() {
	// put your main code here, to run repeatedly:
	if(wg.available()) {
		msgCount++;
		buff = wg.getCodeInBuffer();
		sp.send(DEVICE_ID, ((msgCount >> 8) & 0xFF), (msgCount & 0xFF), buff);
	}
}