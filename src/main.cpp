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
#include "Wiegand.h"
#include "SimpleProtocol.h"

#define DEVICE_ID 0xC5

const int pinD0 = 18;
const int pinD1 = 19;
const int ledG = 26;
const int ledR = 27;

Wiegand wg;
SimpleProtocol sp;

uint16_t msgCount = 0;
unsigned char * buff;

void setup() {
  	// put your setup code here, to run once:
  	sp.begin(115200);  
	wg.begin(pinD0, pinD1); // pinD0, pinD1
	pinMode(ledG, OUTPUT);
	pinMode(ledR, OUTPUT);
}

void loop() {
	// put your main code here, to run repeatedly:
	if(wg.available()) {
		msgCount++;
		buff = wg.getCodeInBuffer();
		sp.send(DEVICE_ID, ((msgCount >> 8) & 0xFF), (msgCount & 0xFF), buff);
		digitalWrite(ledR, LOW);
		digitalWrite(ledG, HIGH);
		delay(500);
	}
	else {
		digitalWrite(ledR, HIGH);
		digitalWrite(ledG, LOW);
	}
	
}