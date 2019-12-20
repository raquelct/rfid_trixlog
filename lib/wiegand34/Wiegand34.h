/**
 * @file Wiegand34.h
 * @author Raquel Teixeira (raquelct97@gmail.com.com)
 * @brief 
 * @version 1.0
 * @date 2019-12-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _WIEGAND34_H
#define _WIEGAND34_H

#include "Arduino.h"

class Wiegand34 {
	public:
		void begin(int pinD0, int pinD1);
		bool available();
		unsigned long getCode();
		unsigned char * getCodeInBuffer();
		
	private:
		static void ReadD0();
		static void ReadD1();
		static bool DoWiegandConversion();
		static unsigned long GetCardId(volatile unsigned long *codehigh, volatile unsigned long *codelow);
		static bool parityCalc(unsigned long v, int n, bool type);
		
		static volatile unsigned long _cardTempHigh;
		static volatile unsigned long _cardTemp;
		static volatile unsigned long _lastPulseTime;
		static volatile int	_bitCount;	
		static unsigned long _code;
		static unsigned char _buffer[4];
};

#endif