/**
 * @file Wiegand34.cpp
 * @author Raquel Teixeira (raquelct97@gmail.com.com)
 * @brief 
 * @version 1.0
 * @date 2019-12-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "Wiegand34.h"

volatile unsigned long Wiegand34::_cardTempHigh = 0;
volatile unsigned long Wiegand34::_cardTemp = 0;
volatile unsigned long Wiegand34::_lastPulseTime = 0;
unsigned long Wiegand34::_code = 0;
volatile int Wiegand34::_bitCount = 0;
unsigned char Wiegand34::_buffer[4];	

/**
 * @brief Read the card code number converted from Wiegand34
 * 
 * @return unsigned long - Code from card
 */
unsigned long Wiegand34::getCode() {
	return _code;
}

/**
 * @brief Read the card code number converted from Wiegand34 in a char buffer
 * 
 * @return unsigned char* - Buffer with code
 */
unsigned char * Wiegand34::getCodeInBuffer() {
	_buffer[0] = ((_code >> 24) & 0xFF);
	_buffer[1] = ((_code >> 16) & 0xFF);
	_buffer[2] = ((_code >> 8) & 0xFF);
	_buffer[3] = (_code & 0xFF);
	return _buffer;
}

/**
 * @brief Check if received any valid Wiegand34 message
 * 
 * @return true - received a new Wiegand34 message
 * @return false - no new valid messages
 */
bool Wiegand34::available() {
	bool ret;
	noInterrupts();
	ret = DoWiegandConversion();
	interrupts();
	return ret;
}

/**
 * @brief Initialize the card reader, setting its pins as inputs and configuring interrupts
 * 
 * @param pinD0 - Pin DATA0(-) of card reader
 * @param pinD1 - Pin DATA1(+) of card reader
 */
void Wiegand34::begin(int pinD0, int pinD1) {
	_lastPulseTime = 0;
	_cardTempHigh = 0;
	_cardTemp = 0;
	_code = 0;
	_bitCount = 0;  
	pinMode(pinD0, INPUT);	// Set D0 pin as input
	pinMode(pinD1, INPUT);	// Set D1 pin as input
	
	attachInterrupt(digitalPinToInterrupt(pinD0), ReadD0, FALLING);  // Hardware interrupt - high to low pulse
	attachInterrupt(digitalPinToInterrupt(pinD1), ReadD1, FALLING);  // Hardware interrupt - high to low pulse
}

/**
 * @brief Interrupt callback to Read DATA0 pin and store is value
 * 
 */
IRAM_ATTR void Wiegand34::ReadD0 () {
	_bitCount++;				// Increament bit count for Interrupt connected to D0
	if (_bitCount>31) {			// If bit count more than 31, process high bits
		_cardTempHigh |= ((0x80000000 & _cardTemp)>>31);	//	shift value to high bits
		_cardTempHigh <<= 1;
		_cardTemp <<= 1;
	}
	else {
		_cardTemp <<= 1;		// D0 represent binary 0, so just left shift card data
	}
	_lastPulseTime = millis();	// Keep track of last wiegand bit received
}

/**
 * @brief Interrupt callback to Read DATA1 pin and store is value
 * 
 */
IRAM_ATTR void Wiegand34::ReadD1() {
	_bitCount ++;				// Increment bit count for Interrupt connected to D1
	if (_bitCount>31) {			// If bit count more than 31, process high bits
		_cardTempHigh |= ((0x80000000 & _cardTemp)>>31);	// shift value to high bits
		_cardTempHigh <<= 1;
		_cardTemp |= 1;
		_cardTemp <<= 1;
	}
	else {
		_cardTemp |= 1;			// D1 represent binary 1, so OR card data with 1 then
		_cardTemp <<= 1;		// left shift card data
	}
	_lastPulseTime = millis();	// Keep track of last wiegand bit received
}

/**
 * @brief Convert the 34 bits Wiegand in a 32 bits value removing the parity bits
 * 
 * @param unsigned long *codehigh - Most significant of the code containg 3 bits useful
 * @param unsigned long *codelow - Less significant of the code containg 31 bits useful
 * @return unsigned long - 32 bits code
 */
unsigned long Wiegand34::GetCardId (volatile unsigned long *codehigh, volatile unsigned long *codelow) {
		*codehigh = *codehigh & 0x03;				// only need the 2 LSB of the codehigh
		*codehigh <<= 30;							// shift 2 LSB to MSB		
		*codelow >>= 1;								// remove the first parity
		return *codehigh | *codelow;
}

/**
 * @brief Calculate parity of a value by adding n bits
 * 
 * @param v - Value to calculate parity
 * @param n - Number of bits to sum
 * @param type - Parity Type - True = Even, False = Odd
 * @return true - Type = true: number of bits 1 is even, Type = false: number of bits 1 is odd
 * @return false - Type = true: number of bits 1 is odd, Type = false: number of bits 1 is even
 */
bool Wiegand34::parityCalc(unsigned long v, int n, bool type){
	int count=0;
	for (int i = 0; i <= n; i++) {
		// sum every bit to calculate parity if even(0) if odd(1)
		count += v & 1; 
    	v >>= 1;
	}
	
	if (type)
		return (count % 2);	
	else
		return !(count % 2);
}

/**
 * @brief Check if the recieved Wiegand 34 message it's valid after a timout
 * 
 * @return true - The recieved message it's 34 bits and the parity is correct
 * @return false - invalid message or noise
 */
bool Wiegand34::DoWiegandConversion () {
	unsigned long sysTick = millis();
	
	if ((sysTick - _lastPulseTime) > 25) {	// if no more signal coming through after 25ms
		if (_bitCount==34) {	// bitCount for Wiegand 34
			_cardTemp >>= 1;	// shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			
			// in wiegand 34 the first(Odd) an the last(Even) bit are for parity
			bool rightParity = _cardTemp & 0x01;
			bool leftParity = _cardTempHigh>>2 & 0x01;
			Serial.println(_cardTemp);
			Serial.println(_cardTempHigh); 
			// use only the 32 bits useful data for calculate parity
			bool rpar = parityCalc(_cardTemp>>1 & 0xFFFF, (_bitCount-2)/2, true); // calculate parity for the firt half of bits(16) eliminating the first bit
			bool lpar = parityCalc((_cardTempHigh<<14) | (_cardTemp>>17), (_bitCount-2)/2, false); // calculate parity for the rest 16 bits

            if (leftParity == lpar && rightParity == rpar) {	// parity is correct
				_code = GetCardId (&_cardTempHigh, &_cardTemp);	// get the card code
			} 
			else {	// parity error
				_bitCount = 0;
				_cardTemp = 0;
				_cardTempHigh = 0;
				return false;
			}
			_bitCount = 0;
			_cardTemp = 0;
			_cardTempHigh = 0;
			return true;
		}
		else {
			// time over 25 ms and bitCount !=34 , must be noise or nothing then.
			_lastPulseTime = sysTick;
			_bitCount = 0;			
			_cardTemp = 0;
			_cardTempHigh = 0;
			return false;
		}	
	}
	else
		return false;
}