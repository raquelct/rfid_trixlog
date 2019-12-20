/**
 * @file SimpleProtocol.h
 * @author Raquel Teixeira (raquelct97@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2019-12-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _SIMPLEPROTOCOL_H
#define _SIMPLEPROTOCOL_H

#include "Arduino.h"

#define START 0x01
#define STOP 0x04
#define MASK_CHAR 0x0A

class SimpleProtocol {
    public:
        void begin(int baudrate);
        void send(unsigned char id, unsigned char cHigh, unsigned char cLow, unsigned char * data);
    private:
        static unsigned char checksum(unsigned char * buffer, int size);
        static void mask(unsigned char value);
};

#endif