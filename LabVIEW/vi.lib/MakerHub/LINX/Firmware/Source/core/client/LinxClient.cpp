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

static unsigned char m_Unknown[] = "Uninitialized Device Client";

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxClient::LinxClient()
{
	m_DeviceName = m_Unknown;

	//DIO
	m_DigitalChans.clear();

	//AI
	m_AiChans.clear();

	//AO
	m_AoChans.clear();

	//PWM
	m_PwmChans.clear();

	//QE
	m_QeChans.clear();

	//UART
	m_UartChans.clear();

	//I2C
	m_I2cChans.clear();

	//SPI
	m_SpiChans.clear();

	//CAN
	m_CanChans.clear();

	//Servo
	m_ServoChans.clear();

	m_ListenerBufferSize = 255;

	m_PacketNum = (unsigned int)((double)rand() / RAND_MAX * 0xFFFF);
}

LinxClient::~LinxClient()
{
	if (m_DeviceName != m_Unknown)
		free(m_DeviceName);

	//DIO

	//AI

	//AO

	//PWM

	//QE

	//UART

	//I2C

	//SPI

	//CAN

	//Servo
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
unsigned short LinxClient::GetNextPacketNum()
{
	return m_PacketNum++;
}

int LinxClient::PrepareHeader(unsigned char* buffer, unsigned short command, int dataLength, int *headerLength)
{
	dataLength += 7;
	if (dataLength >= (int)m_ListenerBufferSize || dataLength + 2 > 0xFFFFFF)
		return LERR_MSG_TO_LONG;

	if (dataLength <= 255)
	{
		*headerLength = WriteU16ToBuff(buffer, 0,  0xFF00 + (unsigned short)dataLength);
	}
	else
	{
		*headerLength = WriteU32ToBuff(buffer, 0, 0xFE000000 + (unsigned int)dataLength + 2);
	}
	*headerLength = WriteU16ToBuff(buffer, *headerLength, GetNextPacketNum());
	*headerLength = WriteU16ToBuff(buffer, *headerLength, command);
	return L_OK;
}

int LinxClient::WriteAndRead(unsigned char *buffer, int buffLength, int *headerLength, int dataLength, int *dataRead)
{
	unsigned short packetNum = GetU16FromBuff(buffer, *headerLength - 2);
	unsigned int start = GetMilliSeconds();

	dataLength += *headerLength;
	buffer[dataLength] = ComputeChecksum(buffer, dataLength);
	int status = WriteData(buffer, start,  m_Timeout, dataLength + 1);
	if (!status)
	{
		status = ReadData(buffer, start,  m_Timeout, 4, dataRead);
		if (!status)
		{
			if ((buffer[0] & 0xFE) != 0xFE)
				return LERR_INVALID_FRAME;

			*headerLength = 2;
			if (buffer[0] == 0xFF)
			{
				dataLength = buffer[1];
			}
			else
			{
				dataLength = GetU32FromBuff(buffer, 0) & 0x00FFFFFF;
				*headerLength += 2;
			}
			if (buffLength < dataLength)
				dataLength = buffLength;
			status = ReadData(buffer + 4, start,  m_Timeout, dataLength - 4, dataRead);
			if (!status)
			{
				*dataRead += 4;
				if (!ChecksumPassed(buffer, dataLength - 1))
					return LERR_CHECKSUM;

				if (packetNum != GetU16FromBuff(buffer, *headerLength))
					return LERR_PACKET_NUM;

				*headerLength += 2;
				status = buffer[(*headerLength)++];
			}
		}
	}
	return status;
}

int LinxClient::GetNoParameter(unsigned short command)
{
	unsigned char buffer[20];
	int headerLength;
	int status = PrepareHeader(buffer, command, 0, &headerLength);
	if (!status)
	{
		int dataRead;
		// send data and read response
		return WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
	}
	return status;
}

int LinxClient::GetU8Parameter(unsigned short command, unsigned char *val)
{
	unsigned char buffer[20];
	int headerLength;
	int status = PrepareHeader(buffer, command, 0, &headerLength);
	if (!status)
	{
		int dataRead;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
		if (!status)
		{
			ReadU8FromBuff(buffer, headerLength, val);
		}
	}
	return status;
}

int LinxClient::GetU16Parameter(unsigned short command, unsigned short *val)
{
	unsigned char buffer[20];
	int headerLength;
	int status = PrepareHeader(buffer, command, 0, &headerLength);
	if (!status)
	{
		int dataRead;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
		if (!status)
		{
			ReadU16FromBuff(buffer, headerLength, val);
		}
	}
	return status;
}

int LinxClient::GetU32Parameter(unsigned short command, unsigned int *val)
{
	unsigned char buffer[20];
	int headerLength;
	int status = PrepareHeader(buffer, command, 0, &headerLength);
	if (!status)
	{
		int dataRead;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
		if (!status)
		{
			ReadU32FromBuff(buffer, headerLength, val);
		}
	}
	return status;
}

int LinxClient::GetU8ArrParameter(unsigned short command, unsigned char *buffer, int buffLength, int *headerLength, int *dataRead)
{
	int status = PrepareHeader(buffer, command, 0, headerLength);
	if (!status)
	{
		// send data and read response
		status = WriteAndRead(buffer, buffLength, headerLength, 0, dataRead);
		if (!status)
		{
			*dataRead -= *headerLength;
		}
	}
	return status;
}

static void CopyArrayToSet(std::set<unsigned char> &s, unsigned char *arr, int length) 
{
	s.clear();
	for (int i = 0; i < length; i++)
		s.insert(arr[i]);
}

static int CopySetToArray(std::set<unsigned char> &s, unsigned char *arr, int length)
{
	int i = 0, len = (int)s.size();
	if (arr)
	{
		if (len > length)
			len = length;
		for (std::set<unsigned char>::iterator it = s.begin(); i < len && it != s.end(); it++, i++)
			arr[i] = *it;
	}
	return len;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int LinxClient::Initialize()
{
	unsigned char buffer[255];
	int dataRead, offset;
	int status = GetNoParameter(LCMD_SYNC);
	if (!status)
	{
		status = GetU8ArrParameter(LCMD_GET_API_VER, buffer, 255, &offset, &dataRead);
	}
	if (!status)
	{
		if (dataRead >= 3)
		{
			LinxApiMajor = buffer[offset++];
			LinxApiMinor = buffer[offset++];
			LinxApiSubminor = buffer[offset++];
		}
		status = GetU8ArrParameter(LCMD_GET_DEV_ID, buffer, 255, &offset, &dataRead);
	}
	if (!status)
	{
		if (dataRead >= 2)
		{
			DeviceFamily = buffer[offset++];
			DeviceId = buffer[offset++];
		}
		status = GetU8ArrParameter(LCMD_GET_DEV_NAME, buffer, 255, &offset, &dataRead);
	}
	if (!status)
	{
		if	(dataRead > 0)
		{
			if (m_DeviceName == m_Unknown)
				m_DeviceName = (unsigned char*)malloc(dataRead + 1);
			else
				m_DeviceName = (unsigned char*)realloc(m_DeviceName, dataRead + 1);

			if (m_DeviceName)
			{
				memcpy(m_DeviceName, buffer + offset, dataRead);
				m_DeviceName[dataRead] = 0;
			}
		}

		status = GetU8ArrParameter(LCMD_GET_MAX_PACK_SIZE, buffer, 255, &offset, &dataRead);
		if (!status && dataRead >= 4)
			ReadU32FromBuff(buffer, offset, &m_ListenerBufferSize);
		// ignore error

		status = GetU8ArrParameter(LCMD_GET_AI_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_AiChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_AO_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_AoChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_DIO_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_DigitalChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_PWM_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_PwmChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_QE_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_QeChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_UART_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_UartChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_I2C_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_I2cChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_SPI_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_SpiChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_CAN_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_CanChans, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_SERVO_CHANS, buffer, 255, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(m_ServoChans, buffer, dataRead);
	}
	//----Peripherals----
	// Uart
	if (!status)
	{
		status = GetU32Parameter(LCMD_GET_UART_MAX_BAUD, &UartMaxBaud);
	}
	// AI
	if (!status)
	{
		status = GetU32Parameter(LCMD_GET_AI_REF_VOLT, &AiRefSet);
	}
	if (!status)
	{
		status = GetU32Parameter(LCMD_GET_AI_REF_VOLT, &AiRefSet);
	}
/*


	// AI
	unsigned char AiResolution;
	unsigned int AiRefDefault;
	unsigned int AiRefSet;

	// AO
	AoResolution;
	AoRefDefault;
	AoRefSet;

	// Uart
	UartMaxBaud = 0;

	// User Configured Values
	userId = 0;

	unsigned int ethernetIp;
	unsigned short ethernetPort;

	unsigned int WifiIp;
	unsigned short WifiPort;
	unsigned char WifiSsidSize;
	char WifiSsid[32];
	unsigned char WifiSecurity;
	unsigned char WifiPwSize;
	char WifiPw[64];

	*/
	return status;
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
unsigned char LinxClient::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	size_t len = strlen((char*)m_DeviceName);
	if (buffer)
	{
		if (len > length)
			len = length;
		memcpy(buffer, m_DeviceName, len);
		if (length > len)
			buffer[len] = 0;
	}
	return (unsigned char)len; 
}

unsigned char LinxClient::GetAiChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_AiChans, buffer, length);
}

unsigned char LinxClient::GetAoChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_AoChans, buffer, length);
}

unsigned char LinxClient::GetDioChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_DigitalChans, buffer, length);
}

unsigned char LinxClient::GetQeChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_QeChans, buffer, length);
}

unsigned char LinxClient::GetPwmChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_PwmChans, buffer, length);
}

unsigned char LinxClient::GetSpiChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_SpiChans, buffer, length);
}

unsigned char LinxClient::GetI2cChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_I2cChans, buffer, length);
}

unsigned char LinxClient::GetUartChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_UartChans, buffer, length);
}

unsigned char LinxClient::GetCanChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_CanChans, buffer, length);
}

unsigned char LinxClient::GetServoChans(unsigned char *buffer, unsigned char length)
{
	return (unsigned char)CopySetToArray(m_ServoChans, buffer, length);
}

