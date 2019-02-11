/*
  DS2482 library for Arduino
  Copyright (C) 2009-2010 Paeae Technologies

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __DS2482_H__
#define __DS2482_H__

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

//Uncomment the following line to enable software I2C
//You will need to have the SoftwareWire library installed
#ifndef ARDUINO_ARCH_ESP8266
#include <SoftwareWire.h> //SoftwareWire by Testato. Installed from library manager.
#endif

// you can exclude onewire_search by defining that to 0
#ifndef ONEWIRE_SEARCH
#define ONEWIRE_SEARCH 1
#endif

// You can exclude CRC checks altogether by defining this to 0
#ifndef ONEWIRE_CRC
#define ONEWIRE_CRC 1
#endif

#define NO_WIRE 0
#define HARD_WIRE 1
#define SOFT_WIRE 2

#define DS2482_DEVICE_RESET			0xF0

 #define DS2482_STATUS_BUSY	(1<<0)
 #define DS2482_STATUS_PPD	(1<<1)
 #define DS2482_STATUS_SD	(1<<2)
 #define DS2482_STATUS_LL	(1<<3)
 #define DS2482_STATUS_RST	(1<<4)
 #define DS2482_STATUS_SBR	(1<<5)
 #define DS2482_STATUS_TSB	(1<<6)
 #define DS2482_STATUS_DIR	(1<<7)

#define DS2482_SET_POINTER		0xE1
 #define DS2482_STATUS_REG		0xF0
 #define DS2482_DATA_REG		0xE1
 #define DS2482_CHANNEL_REG		0xD2
 #define DS2482_CONFIG_REG		0xC3

#define DS2482_WRITE_CONFIG		0xD2

 #define DS2482_CONFIG_APU	(1<<0)
 #define DS2482_CONFIG_PPM  (1<<1)
 #define DS2482_CONFIG_SPU	(1<<2)
 #define DS2482_CONFIG_WS	(1<<3)

#define DS2482_SET_POINTER			0xE1
 #define DS2482_STATUS_REG			0xF0
 #define DS2482_DATA_REG			0xE1
 #define DS2482_CHANNEL_REG			0xD2
 #define DS2482_CONFIG_REG			0xC3
 
#define DS2482_ONE_WIRE_RESET		0xB4
#define DS2482_ONE_WIRE_WRITE_BYTE	0xA5
#define DS2482_ONE_WIRE_READ_BYTE	0x96
#define DS2482_ONE_WIRE_SINGLE_BIT	0x87
#define DS2482_ONE_WIRE_TRIPLET		0x78

#define ONE_WIRE_READ_ROM		0x33
#define ONE_WIRE_MATCH_ROM		0x55
#define ONE_WIRE_SKIP_ROM		0xCC
#define ONE_WIRE_SEARCH_ROM		0xF0
#define ONE_WIRE_ALARM_SEARCH	0xEC

#define DS2482_SELECT_CHANNEL	0xC3
 #define DS2482_WRITE_CHANNEL_0	0xF0
 #define DS2482_WRITE_CHANNEL_1	0xE1
 #define DS2482_WRITE_CHANNEL_2	0xD2
 #define DS2482_WRITE_CHANNEL_3	0xC3
 #define DS2482_WRITE_CHANNEL_4	0xB4
 #define DS2482_WRITE_CHANNEL_5	0xA5
 #define DS2482_WRITE_CHANNEL_6	0x96
 #define DS2482_WRITE_CHANNEL_7	0x87

 #define DS2482_READ_CHANNEL_0	0xB8
 #define DS2482_READ_CHANNEL_1	0xB1
 #define DS2482_READ_CHANNEL_2	0xAA
 #define DS2482_READ_CHANNEL_3	0xA3
 #define DS2482_READ_CHANNEL_4	0x9C
 #define DS2482_READ_CHANNEL_5	0x95
 #define DS2482_READ_CHANNEL_6	0x8E
 #define DS2482_READ_CHANNEL_7	0x87

class DS2482
{
public:
	//Address is 0-3
	DS2482();
    DS2482(TwoWire &wirePort); //Called when user provides Wire port
    DS2482(TwoWire &wirePort, uint8_t address); //Called when user provides Wire port
	#ifdef SoftwareWire_h
	DS2482(SoftwareWire &wirePort); //Called when user provides a softwareWire port
	DS2482(SoftwareWire &wirePort, uint8_t address); //Called when user provides a softwareWire port
	#endif

	bool configure(uint8_t config);
	void reset();
	void setI2CAddress(uint8_t i2caddress); //Set the address the library should use to communicate.
	uint8_t isPresent();					// return true/false if the chip responds
	//DS2482-800 only
	bool setChannel(uint8_t channel);
	
	bool wireReset(); // return true if presence pulse is detected
	uint8_t wireReadStatus(bool setPtr=false);
	
	void wireWriteByte(uint8_t b);
	uint8_t wireReadByte();
	
	void wireWriteBit(uint8_t bit);
	uint8_t wireReadBit();
    // Issue a 1-Wire rom select command, you do the reset first.
    void wireSelect( uint8_t rom[8]);
	// Issue skip rom
	void wireSkip();
	
	uint8_t hasTimeout() { return mTimeout; }
#if ONEWIRE_SEARCH
    // Clear the search state so that if will start from the beginning again.
    void wireResetSearch();

    // Look for the next device. Returns 1 if a new address has been
    // returned. A zero might mean that the bus is shorted, there are
    // no devices, or you have already retrieved all of them.  It
    // might be a good idea to check the CRC to make sure you didn't
    // get garbage.  The order is deterministic. You will always get
    // the same devices in the same order.
    uint8_t wireSearch(uint8_t *newAddr);
#endif
#if ONEWIRE_CRC
    // Compute a Dallas Semiconductor 8 bit CRC, these are used in the
    // ROM and scratchpad registers.
    static uint8_t crc8( uint8_t *addr, uint8_t len);
#endif

private:
	
	uint8_t mAddress;
	uint8_t mTimeout;
	uint8_t readByte();
	void setReadPtr(uint8_t readPtr);
	
	uint8_t busyWait(bool setReadPtr=false); //blocks until
	void begin();
	uint8_t end();

    uint8_t _wireType = HARD_WIRE; //Default to Wire.h
    TwoWire *_hardPort = NO_WIRE; //The generic connection to user's chosen I2C hardware
    
	#ifdef SoftwareWire_h
	SoftwareWire *_softPort = NO_WIRE; //Or, the generic connection to software wire port
	#endif
	
#if ONEWIRE_SEARCH
	uint8_t searchAddress[8];
	uint8_t searchLastDisrepancy;
	uint8_t searchExhausted;
#endif
	
};



#endif