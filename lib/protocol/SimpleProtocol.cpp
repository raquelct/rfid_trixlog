/**
 * @file SimpleProtocol.cpp
 * @author Raquel Teixeira (raquelct97@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2019-12-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "SimpleProtocol.h"

unsigned char msk[2];

/**
 * @brief Start the Serial with a specified baudrate
 * 
 * @param baudrate - Baudrate of Serial communication 
 */
void SimpleProtocol::begin(int baudrate) {
    Serial.begin(baudrate);
}

/**
 * @brief Assemble the package and send over the Serial
 * Package - Start byte, Device ID, Message Counter, Card Data, Checksum, Stop byte
 * 
 * @param id - Device ID
 * @param cHigh - Most significant of message counter
 * @param cLow - Less significant of message counter
 * @param data - Card data
 */
void SimpleProtocol::send(unsigned char id, unsigned char cHigh, unsigned char cLow, unsigned char * data) {
    unsigned char tmpBuffer[7] = {id, cHigh, cLow, data[0], data[1], data[2], data[3]};
    unsigned char sendBuffer[14] = {0};
    int index = 0;

    for(int i = 0; i < 7; i++) {
        if(tmpBuffer[i] == START || tmpBuffer[i] == STOP || tmpBuffer[i] == 10){    // vheck if the value is a reserved character
           mask(tmpBuffer[i]);  // if so mask the value
           sendBuffer[index] = msk[1];  // put the masked value in a array to be sent
           sendBuffer[index+1] = msk[0];    
           index += 2;
        } 
        else {  //if not don't mask the value
            sendBuffer[index] = tmpBuffer[i];   // just put in the arry
            index++;
        }
    } 

    // Send the package through the serial in the order of the protocol
    Serial.write(START);
    for(int i = 0; i < index; i++) {
        Serial.write(sendBuffer[i]);
    }
    unsigned char cksum = checksum(sendBuffer, index);  // calculate checksum of the data sent
    if (cksum == START || cksum == STOP || cksum == 10) {   // check to see if checksum is a reserved character
        mask(cksum);    // if so mask the value and send
        Serial.write(msk[1]);
        Serial.write(msk[0]);
    }
    else {  // if not just send
        Serial.write(cksum);
    }
    Serial.write(STOP);
}

/**
 * @brief Calculate checksum of a buffer by adding all values in it
 * 
 * @param buffer - Buffer to calculate checksum
 * @param size - Size of buffer
 * @return unsigned char - Only the less significant part of the sum
 */
unsigned char SimpleProtocol::checksum(unsigned char * buffer, int size) {
    int sum = 0;
    for(int i = 0; i < size; i++) {
        sum += buffer[i];
    }
    return (sum & 0xFF);    // return only one byte with the less signifcant part
}

/**
 * @brief Mask a value that contains a reserved character
 * 
 * @param value - The value to mask
 * @return unsigned char* - An array with the masked value 
 */
void SimpleProtocol::mask(unsigned char value) {
    msk[1] = 10;            // Put 10 in front
    msk[0] = value + 20;    // sum the value + 20
}
