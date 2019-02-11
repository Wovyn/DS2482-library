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

  You should have readd a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

	crc code is from OneWire library

	-Updates:
		* fixed wireReadByte busyWait (thanks Mike Jackson)
		* Modified search function (thanks Gary Fariss)

*/

#include "DS2482.h"


//Begin comm with DS2482 over I2C
DS2482::DS2482()
{
	mAddress = 0x18; 	//Default, I2C address is 0x18
	_hardPort = &Wire;	//Default to Wire port
	_wireType = HARD_WIRE;
	Wire.begin();
	
}

//Begin comm with DS2482 over I2C
DS2482::DS2482(TwoWire &wirePort)
{
	mAddress = 0x18; 	//Default, I2C address is 0x18
	_hardPort = &wirePort;
	_wireType = HARD_WIRE;

}

//Begin comm with DS2482 over I2C
DS2482::DS2482(TwoWire &wirePort, uint8_t address)
{
	mAddress = 0x18 | address; 	//Default, I2C address is 0x18
	_hardPort = &wirePort;
	_wireType = HARD_WIRE;

}

//Begin comm with DS2482 over software I2C
#ifdef SoftwareWire_h
DS2482::DS2482(SoftwareWire& wirePort)
{
	mAddress = 0x18; 	//Default, I2C address is 0x18
	_softPort = &wirePort;
	_wireType = SOFT_WIRE;

}

DS2482::DS2482(SoftwareWire& wirePort, uint8_t address)
{
	mAddress = 0x18 | address; 	//Default, I2C address is 0x18
	_softPort = &wirePort;
	_wireType = SOFT_WIRE;

}
#endif

void DS2482::begin()
{
	switch(_wireType)
	{
		case(HARD_WIRE):
			_hardPort->beginTransmission(mAddress);
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			_softPort->beginTransmission(mAddress);
		#endif
			break;
	}
}

uint8_t DS2482::end()
{
	switch(_wireType)
	{
		case(HARD_WIRE):
			return _hardPort->endTransmission();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			return _softPort->endTransmission();
		#endif
			break;
	}
}

// Simply starts and ends an Wire transmission
// If no devices are present, this returns false
uint8_t DS2482::isPresent()
{
	begin();
	return !end() ? true : false;
}

void DS2482::setReadPtr(uint8_t readPtr)
{
	switch(_wireType)
	{
		case(HARD_WIRE):
			begin();
			_hardPort->write(DS2482_SET_POINTER);  // changed from 'send' to 'write' according http://blog.makezine.com/2011/12/01/arduino-1-0-is-out-heres-what-you-need-to-know/'
			_hardPort->write(readPtr);
			end();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			begin();
			_softPort->write(DS2482_SET_POINTER);  // changed from 'send' to 'write' according http://blog.makezine.com/2011/12/01/arduino-1-0-is-out-heres-what-you-need-to-know/'
			_softPort->write(readPtr);
			end();
		#endif
			break;
	}
}

uint8_t DS2482::readByte()
{
	switch(_wireType)
	{
		case(HARD_WIRE):
			_hardPort->requestFrom(mAddress,(uint8_t)1);
			return _hardPort->read();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			_softPort->requestFrom(mAddress,(uint8_t)1);
			return _softPort->read();
		#endif
			break;
	}
}

uint8_t DS2482::wireReadStatus(bool setPtr)
{
	if (setPtr)
		setReadPtr(DS2482_STATUS_REG);

	return readByte();
}

uint8_t DS2482::busyWait(bool setReadPtr)
{
	uint8_t status;
	int loopCount = 1000;
	while((status = wireReadStatus(setReadPtr)) & DS2482_STATUS_BUSY)
	{
		if (--loopCount <= 0)
		{
			mTimeout = 1;
			break;
		}
		delayMicroseconds(20);
	}
	return status;
}

//----------interface
void DS2482::reset()
{
	mTimeout = 0;
	switch(_wireType)
	{
		case(HARD_WIRE):
			begin();
			_hardPort->write(DS2482_DEVICE_RESET);
			end();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
		begin();
		_softPort->write(DS2482_DEVICE_RESET);
		end();
		#endif
			break;
	}
}

bool DS2482::configure(uint8_t config)
{
	busyWait(true);
	switch(_wireType)
	{
		case(HARD_WIRE):
			begin();
			_hardPort->write(DS2482_WRITE_CONFIG);
			_hardPort->write(config | (~config)<<4);
			end();
			
			return readByte() == config;
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			begin();
			_softPort->write(DS2482_WRITE_CONFIG);
			_softPort->write(config | (~config)<<4);
			end();
			
			return readByte() == config;
		#endif
			break;
	}
}

//Set the global setting for the I2C address we want to communicate with
//Default is 0x18
void DS2482::setI2CAddress(uint8_t address)
{
	mAddress = address; //Set the I2C address for this device
}

bool DS2482::setChannel(uint8_t channel)
{
	uint8_t ch, ch_read;

	switch (channel)
	{
		case 0:
		default:
			ch = DS2482_WRITE_CHANNEL_0;
			ch_read = DS2482_READ_CHANNEL_0;
			break;
		case 1:
			ch = DS2482_WRITE_CHANNEL_1;
			ch_read = DS2482_READ_CHANNEL_1;
			break;
		case 2:
			ch = DS2482_WRITE_CHANNEL_2;
			ch_read = DS2482_READ_CHANNEL_2;
			break;
		case 3:
			ch = DS2482_WRITE_CHANNEL_3;
			ch_read = DS2482_READ_CHANNEL_3;
			break;
		case 4:
			ch = DS2482_WRITE_CHANNEL_4;
			ch_read = DS2482_READ_CHANNEL_4;
			break;
		case 5:
			ch = DS2482_WRITE_CHANNEL_5;
			ch_read = DS2482_READ_CHANNEL_5;
			break;
		case 6:
			ch = DS2482_WRITE_CHANNEL_6;
			ch_read = DS2482_READ_CHANNEL_6;
			break;
		case 7:
			ch = DS2482_WRITE_CHANNEL_7;
			ch_read = DS2482_READ_CHANNEL_7;
			break;
	};

	busyWait(true);
	switch(_wireType)
	{
		case(HARD_WIRE):
			begin();
			_hardPort->write(DS2482_SELECT_CHANNEL);
			_hardPort->write(ch);
			end();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			begin();
			_softPort->write(DS2482_SELECT_CHANNEL);
			_softPort->write(ch);
			end();
		#endif
			break;
	}
	busyWait();

	uint8_t check = readByte();

	return check == ch_read;
}



bool DS2482::wireReset()
{
	busyWait(true);

	switch(_wireType)
	{
		case(HARD_WIRE):
			begin();
			_hardPort->write(DS2482_ONE_WIRE_RESET);
			end();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			begin();
			_softPort->write(DS2482_ONE_WIRE_RESET);
			end();
		#endif
			break;
	}

	uint8_t status = busyWait();

	return status & DS2482_STATUS_PPD ? true : false;
}


void DS2482::wireWriteByte(uint8_t b)
{
	busyWait(true);

	switch(_wireType)
	{
		case(HARD_WIRE):
			begin();
			_hardPort->write(DS2482_ONE_WIRE_WRITE_BYTE);
			_hardPort->write(b);
			end();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			begin();
			_softPort->write(DS2482_ONE_WIRE_WRITE_BYTE);
			_softPort->write(b);
			end();
		#endif
			break;
	}
}

uint8_t DS2482::wireReadByte()
{
	busyWait(true);

	switch(_wireType)
	{
		case(HARD_WIRE):
			begin();
			_hardPort->write(DS2482_ONE_WIRE_READ_BYTE);
			end();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			begin();
			_softPort->write(DS2482_ONE_WIRE_READ_BYTE);
			end();
		#endif
			break;
	}

	busyWait();
	setReadPtr(DS2482_DATA_REG);
	return readByte();
}

void DS2482::wireWriteBit(uint8_t bit)
{
	busyWait(true);
	switch(_wireType)
	{
		case(HARD_WIRE):
			begin();
			_hardPort->write(DS2482_ONE_WIRE_SINGLE_BIT);
			_hardPort->write(bit ? 0x80 : 0);
			end();
			break;
		
		case(SOFT_WIRE):
		#ifdef SoftwareWire_h
			begin();
			_softPort->write(DS2482_ONE_WIRE_SINGLE_BIT);
			_softPort->write(bit ? 0x80 : 0);
			end();
		#endif
			break;
	}
}

uint8_t DS2482::wireReadBit()
{
	wireWriteBit(1);
	uint8_t status = busyWait(true);
	return status & DS2482_STATUS_SBR ? 1 : 0;
}

void DS2482::wireSkip()
{
	wireWriteByte(ONE_WIRE_SKIP_ROM);
}

void DS2482::wireSelect(uint8_t rom[8])
{
	wireWriteByte(ONE_WIRE_MATCH_ROM);
	for (int i=0;i<8;i++)
		wireWriteByte(rom[i]);
}


#if ONEWIRE_SEARCH
void DS2482::wireResetSearch()
{
	searchExhausted = 0;
	searchLastDisrepancy = 0;

	for(uint8_t i = 0; i<8; i++)
		searchAddress[i] = 0;
}

uint8_t DS2482::wireSearch(uint8_t *newAddr)
{
	uint8_t i;
	uint8_t direction;
	uint8_t last_zero=0;

	if (searchExhausted)
		return 0;

	if (!wireReset())
		return 0;

	busyWait(true);
	wireWriteByte(ONE_WIRE_SEARCH_ROM);

	for(i=1;i<65;i++)
	{
		int romByte = (i-1)>>3;
		int romBit = 1<<((i-1)&7);

		if (i < searchLastDisrepancy)
			direction = searchAddress[romByte] & romBit;
		else
			direction = i == searchLastDisrepancy;

		busyWait();

		switch(_wireType)
		{
			case(HARD_WIRE):
				begin();
				_hardPort->write(DS2482_ONE_WIRE_TRIPLET);
				_hardPort->write(direction ? 0x80 : 0);
				end();
				break;
			
			case(SOFT_WIRE):
			#ifdef SoftwareWire_h
				begin();
				_softPort->write(DS2482_ONE_WIRE_TRIPLET);
				_softPort->write(direction ? 0x80 : 0);
				end();
			#endif
				break;
		}

		uint8_t status = busyWait();

		uint8_t id = status & DS2482_STATUS_SBR;
		uint8_t comp_id = status & DS2482_STATUS_TSB;
		direction = status & DS2482_STATUS_DIR;

		if (id && comp_id)
			return 0;
		else
		{
			if (!id && !comp_id && !direction)
				last_zero = i;
		}

		if (direction)
			searchAddress[romByte] |= romBit;
		else
			searchAddress[romByte] &= (uint8_t)~romBit;
	}

	searchLastDisrepancy = last_zero;

	if (last_zero == 0)
		searchExhausted = 1;

	for (i=0;i<8;i++)
		newAddr[i] = searchAddress[i];

	return 1;
}
#endif

#if ONEWIRE_CRC
// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
//

uint8_t DS2482::crc8( uint8_t *addr, uint8_t len)
{
	uint8_t crc=0;

	for (uint8_t i=0; i<len;i++)
	{
		uint8_t inbyte = addr[i];
		for (uint8_t j=0;j<8;j++)
		{
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix)
				crc ^= 0x8C;

			inbyte >>= 1;
		}
	}
	return crc;
}

#endif
