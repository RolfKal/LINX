/****************************************************************************************
**  Generic LINX Client code. This file contains the parent class for LINX client communicators.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/

/****************************************************************************************
** Includes
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinxDevice.h"
#include "LinxCommand.h"
#include "LinxUtilities.h"
#include "LinxClient.h"

static unsigned char m_DeviceName[] = "Unknown Device";

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxClient::LinxClient()
{
	m_DeviceName = NULL;

	//DIO
	m_DigitalChans = NULL;

	//AI
	m_AiChans = NULL;

	//AO
	m_AoChans = NULL;

	//PWM
	m_PwmChans = NULL;

	//QE
	m_QeChans = NULL;

	//UART
	m_UartChans = NULL;

	//I2C
	m_I2cChans = NULL;

	//SPI
	m_SpiChans = NULL;

	//CAN
	m_CanChans = NULL;

	//Servo
	m_ServoChans = NULL;
}

LinxClient::~LinxClient()
{
	free(m_DeviceName);

	//DIO
	free(m_DigitalChans);

	//AI
	free(m_AiChans);

	//AO
	free(m_AoChans);

	//PWM
	free(m_PwmChans);

	//QE
	free(m_QeChans);

	//UART
	free(m_UartChans);

	//I2C
	free(m_I2cChans);

	//SPI
	free(m_SpiChans);

	//CAN
	free(m_CanChans);

	//Servo
	free(m_ServoChans);
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
unsigned short LinxClient::GetNextPacketNum()
{
	return 0;
}

unsigned char LinxClient::ComputeChecksum(unsigned char* packetBuffer, unsigned int size)
{
	unsigned char checksum = 0;

	//Sum All Bytes In The Packet Except The Last (Checksum Byte)
	for (unsigned int i = 0; i < size; i++)
	{
		checksum += packetBuffer[i];
	}
	return checksum;
}


bool LinxClient::ChecksumPassed(unsigned char* packetBuffer, unsigned int size)
{
	return (ComputeChecksum(packetBuffer, size) == packetBuffer[size]);
}

// Return the data offset or 0 if an error
unsigned int LinxClient::PrepareHeader(unsigned char* buffer, unsigned short command, unsigned short packetNum, unsigned int dataSize)
{
	unsigned int offset = 0;
	dataSize += 7;
	if (dataSize >= ListenerBufferSize)
		return 0;

	if (dataSize > 255)
	{
		dataSize += 3;
		offset = WriteU8ToBuff(buffer, offset, 0xFE);
		offset = WriteU32ToBuff(buffer, offset, dataSize);
	}
	else
	{
		offset = WriteU8ToBuff(buffer, offset, 0xFF);
		offset = WriteU8ToBuff(buffer, offset, (unsigned char)dataSize);
	}
	offset = WriteU16ToBuff(buffer, offset, packetNum);
	offset = WriteU16ToBuff(buffer, offset, command);
	return offset;
}

int LinxClient::WriteAndRead(unsigned char *buffer, unsigned int *offset, unsigned short packetNum, unsigned int timeout, unsigned int writeData, unsigned int readData, unsigned int *dataRead)
{
	writeData += *offset;
	buffer[writeData] = ComputeChecksum(buffer, writeData);
	int status = WriteCommand(buffer, writeData + 1, timeout);
	if (!status)
	{
		*offset--;
		readData += *offset;
		status = ReadResponse(buffer, readData + 1, timeout);
		if (!status && (buffer[0] & 0xFE) == 0xFE)
		{
			if (ChecksumPassed(buffer, readData))
			{
				unsigned short temp;
				if (dataRead)
				{
					if (buffer[0] == 0xFF)
						*dataRead = buffer[1];
					else
						ReadU32FromBuff(buffer, 1, dataRead);
					*dataRead -= *offset;
				}
				ReadU16FromBuff(buffer, *offset - 2, &temp);
				if (temp != packetNum)
					status = LERR_PACKET_NUM;
				else
					status = buffer[4];
			}
			else
				status = LERR_CHECKSUM;
		}
		else if (!status)
			status = LERR_INVALID_FRAME;
	}
	return status;
}

int LinxClient::GetNoParameter(unsigned short command)
{
	unsigned char buffer[10];
	unsigned short packetNum = GetNextPacketNum();
	unsigned int offset = PrepareHeader(buffer, command, packetNum, 0);
	if (offset)
	{
		// send data and read response
		return WriteAndRead(buffer, &offset, packetNum, m_Timeout, 0, 0, NULL);
	}
	return LERR_LENGTH_NOT_SUPPORTED;
}

int LinxClient::GetU8Parameter(unsigned short command, unsigned char *val)
{
	unsigned char buffer[10];
	unsigned short packetNum = GetNextPacketNum();
	unsigned int offset = PrepareHeader(buffer, command, packetNum, 0);
	if (offset)
	{
		// send data and read response
		int status = WriteAndRead(buffer, &offset, packetNum, m_Timeout, 0, 2, NULL);
		if (!status)
		{
			ReadU8FromBuff(buffer, offset, val);
		}
	}
	return LERR_LENGTH_NOT_SUPPORTED;
}

int LinxClient::GetU32Parameter(unsigned short command, unsigned int *val)
{
	unsigned char buffer[10];
	unsigned short packetNum = GetNextPacketNum();
	unsigned int offset = PrepareHeader(buffer, command, packetNum, 0);
	if (offset)
	{
		// send data and read response
		int status = WriteAndRead(buffer, &offset, packetNum, m_Timeout, 0, 4, NULL);
		if (!status)
		{
			ReadU32FromBuff(buffer, offset, val);
		}
	}
	return LERR_LENGTH_NOT_SUPPORTED;
}

int LinxClient::GetU8ArrParameter(unsigned short command, unsigned char *buffer, unsigned int *offset, unsigned int length, unsigned int *dataRead)
{
	unsigned short packetNum = GetNextPacketNum();
	*offset = PrepareHeader(buffer, command, packetNum, 0);
	if (*offset)
	{
		// send data and read response
		return WriteAndRead(buffer, offset, packetNum, m_Timeout, 0, length, dataRead);
	}
	return LERR_LENGTH_NOT_SUPPORTED;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int LinxClient::Initialize()
{
	unsigned int dataRead, offset;
	unsigned char buffer[255];
	int status = GetNoParameter(LCMD_SYNC);
	if (!status)
	{
		status = GetU8ArrParameter(LCMD_GET_API_VER, buffer, &offset, 3, &dataRead);
	}
	if (!status)
	{
		LinxApiMajor = buffer[offset++];
		LinxApiMinor = buffer[offset++];
		LinxApiSubminor = buffer[offset++];

		status = GetU8ArrParameter(LCMD_GET_DEV_ID, buffer, &offset, 2, &dataRead);
	}
	if (!status)
	{
		DeviceFamily = buffer[offset++];
		DeviceId = buffer[offset++];

		status = GetU8ArrParameter(LCMD_GET_DEV_NAME, buffer, &offset, 255, &dataRead);
	}
	if (!status)
	{
		m_DeviceName = (unsigned char*)malloc(dataRead);
		if (m_DeviceName)
			memcpy(m_DeviceName, buffer + offset, dataRead);

		status = GetU8ArrParameter(LCMD_GET_MAX_PACK_SIZE, buffer, &offset, 4, &dataRead);
		if (!status)
			ReadU32FromBuff(buffer, offset, &ListenerBufferSize);
		// ignore error
		status = L_OK;
	}


/*
		//----Peripherals----


		//AI
		unsigned char NumAiChans;
		const unsigned char* AiChans;
		unsigned char AiResolution;
		unsigned int AiRefDefault;
		unsigned int AiRefSet;

		//AO
		unsigned char NumAoChans;
		const unsigned char* AoChans;
		unsigned char AoResolution;
		unsigned int AoRefDefault;
		unsigned int AoRefSet;

		//PWM
		unsigned char NumPwmChans;
		const unsigned char* PwmChans;

		//QE
		unsigned char NumQeChans;
		const unsigned char* QeChans;

		//UART
		unsigned char NumUartChans;
		const unsigned char* UartChans;
		unsigned int UartMaxBaud;

		//I2C
		unsigned char NumI2cChans;
		const unsigned char* I2cChans;

		//SPI
		unsigned char NumSpiChans;
		const unsigned char* SpiChans;

		//CAN
		unsigned char NumCanChans;
		const unsigned char* CanChans;

		//Servo
		unsigned char NumServoChans;
		const unsigned char* ServoChans;

		//User Configured Values
		unsigned short userId;

		unsigned int ethernetIp;
		unsigned short ethernetPort;

		unsigned int WifiIp;
		unsigned short WifiPort;
		unsigned char WifiSsidSize;
		char WifiSsid[32];
		unsigned char WifiSecurity;
		unsigned char WifiPwSize;
		char WifiPw[64];

		unsigned int serialInterfaceMaxBaud;
		*/
	return status;
}

int LinxClient::WriteCommand(unsigned char *packetBuffer, unsigned int packetLength, unsigned int timeout)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::ReadResponse(unsigned char *packetBuffer, unsigned int packetLength, unsigned int timeout)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
unsigned char LinxClient::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	return L_FUNCTION_NOT_SUPPORTED;
}
