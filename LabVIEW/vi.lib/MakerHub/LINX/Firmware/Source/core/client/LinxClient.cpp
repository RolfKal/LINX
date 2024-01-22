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

#include "LinxDefines.h"
#include "LinxDevice.h"
#if Unix
#include "LinxLinuxChannel.h"
#include "LinxLinuxDevice.h"
#elif Win32
#include "LinxWindowsChannel.h"
#include "LinxWindowsDevice.h"
#endif
#include "LinxCommand.h"
#include "LinxUtilities.h"
#include "LinxClient.h"
static unsigned char m_Unknown[] = "Uninitialized Device Client";

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxClient::LinxClient(const unsigned char *uartDevice, unsigned int *baudrate, unsigned char dataBits, unsigned char stopBits,  LinxUartParity parity, int timeout) : LinxDevice(NULL)
{
	m_DeviceName = m_Unknown;
	m_Timeout = timeout;
	m_CommChannel = NULL;

	LinxUartChannel *channel = NULL;
#if Unix
	channel = new LinxUnixUartChannel(m_Debug, uartDevice);
#elif Win32
	channel = new LinxWindowsUartChannel(m_Debug, uartDevice);
#endif
	if (channel)
	{
		int status = channel->SetSpeed(*baudrate, baudrate);
		if (!status)			
			status = channel->SetParameters(dataBits, stopBits, parity);
		if (!status)
		{
			Initialize(channel);
		}
	}
}

LinxClient::LinxClient(const unsigned char *netAddress, unsigned short port, int timeout) : LinxDevice(NULL)
{
	m_DeviceName = m_Unknown;
	m_Timeout = timeout;
	m_CommChannel = NULL;

	LinxCommChannel *channel = NULL;
#if Unix
	channel = new LinxUnixCommChannel(m_Debug, netAddress, port);
#elif Win32
	channel = new LinxWindowsCommChannel(m_Debug, netAddress, port);
#endif
	if (channel)
	{
		Initialize(channel);
	}
}

LinxClient::~LinxClient(void)
{
	if (m_CommChannel)
		m_CommChannel->Release();
	if (m_DeviceName != m_Unknown)
		free(m_DeviceName);
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxClient::IsInitialized(void)
{
	return (m_CommChannel != NULL);
}

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

int LinxClient::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::AnalogSetRef(unsigned char mode, unsigned int voltage)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::AnalogWrite(unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//DIGITAL
int LinxClient::DigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char* states)
{
	// Command parameters
	// uint8 : num channels
	// uint8[numChans] : channels
	// uint8[numChans] : states
	// Response parameters
	// None
	int status = LERR_MEMORY;
	unsigned int offset, length = 1 + 2 * numChans;
	unsigned char *buffer = (unsigned char *)malloc(10 + length);
	if (buffer)
	{
		status = PrepareHeader(buffer, LCMD_SET_PIN_MODE, length, 0, &offset);
		if (!status)
		{
			buffer[offset] = numChans;
			memcpy(buffer + offset + 1, channels, numChans);
			memcpy(buffer + offset + 1 + numChans, states, numChans);

			// send data and read response
			status = WriteAndRead(buffer, offset + length, &offset, 0, &length);
		}
		free(buffer);
	}
	return status;
}

int LinxClient::DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	// Command parameters
	// uint8 : num channels
	// uint8[numChans] : channels
	// uint8[numChans + 7 / 8] : packed values
	// Response parameters
	// None
	int status = LERR_MEMORY;
	unsigned int offset, length = 1 + numChans + ((numChans + 7) >> 3);
	unsigned char *buffer = (unsigned char *)malloc(10 + length);
	if (buffer)
	{
		status = PrepareHeader(buffer, LCMD_DIGITAL_WRITE, length, 0, &offset);
		if (!status)
		{
			buffer[offset] = numChans;
			memcpy(buffer + offset + 1, channels, numChans);
			memcpy(buffer + offset + 1 + numChans, values, (numChans + 7) >> 3);

			// send data and read response
			status = WriteAndRead(buffer, offset + length, &offset, 0, &length);
		}
		free(buffer);
	}
	return status;
}

int LinxClient::DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	// Command parameters
	// uint8 : num channels
	// uint8[numChans] : channels
	// uint8[numChans] : values
	// Response parameters
	// None
	int status = LERR_MEMORY;
	unsigned int offset, length = 1 + numChans + ((numChans + 7) >> 3);
	unsigned char *buffer = (unsigned char *)malloc(10 + length);
	if (buffer)
	{
		status = PrepareHeader(buffer, LCMD_DIGITAL_WRITE, length, 0, &offset);
		if (!status)
		{
			unsigned char bitOffset = 0;
			unsigned char byteOffset = 0;
			unsigned char diVal = 0;

			buffer[offset] = numChans;
			memcpy(buffer + offset + 1, channels, numChans);

			for (int i = 0; i < numChans; i++)
			{
				if (bitOffset == 8)
				{
					buffer[offset + 1 + numChans + byteOffset] = diVal;
					bitOffset = 0;
					byteOffset++;
					diVal = 0;
				}
				diVal |= (values[i] ? 0x1 : 0x0) << bitOffset++;
			}
			buffer[offset + 1 + numChans + byteOffset] = diVal;
			// send data and read response
			status = WriteAndRead(buffer, offset + length, &offset, 0, &length);
		}
		free(buffer);
	}
	return status;
}

int LinxClient::DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	// Command parameters
	// uint8 : num channels
	// uint8[numChans] : channels
	// Response parameters
	// uint8[numChans + 7 / 8] : packed values
	int status = LERR_MEMORY;
	unsigned int offset, length = 1 + numChans;
	unsigned char *buffer = (unsigned char *)malloc(10 + length);
	if (buffer)
	{
		status = PrepareHeader(buffer, LCMD_DIGITAL_READ, length, (numChans + 7) / 8, &offset);
		if (!status)
		{
			buffer[offset] = numChans;
			memcpy(buffer + offset + 1, channels, numChans);

			// send data and read response
			status = WriteAndRead(buffer, offset + length, &offset, 0, &length);
			if (!status && length >= offset + ((numChans + 7) >> 3))
			{
				memcpy(values, buffer + offset, ((numChans + 7) >> 3));
			}
		}
		free(buffer);
	}
	return status;
}

int LinxClient::DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	// Command parameters
	// uint8 : num channels
	// uint8[numChans] : channels
	// Response parameters
	// uint8[numChans] : values
	int status = LERR_MEMORY;
	unsigned int offset, length = 1 + numChans;
	unsigned char *buffer = (unsigned char *)malloc(10 + length);
	if (buffer)
	{
		status = PrepareHeader(buffer, LCMD_DIGITAL_READ, length, (numChans + 7) / 8, &offset);
		if (!status)
		{
			buffer[offset] = numChans;
			memcpy(buffer + offset + 1, channels, numChans);

			// send data and read response
			status = WriteAndRead(buffer, offset + length, &offset, 0, &length);
			if (!status && length >= offset + ((numChans + 7) >> 3))
			{
				for (int i = 0; i < numChans; i++)
				{
					values[i] = buffer[offset + i / 8] << (7 - i % 8);
				}
			}
		}
		free(buffer);
	}
	return status;
}

int LinxClient::DigitalWriteSquareWave(unsigned char channel, unsigned int freq, unsigned int duration)
{
	// Command parameters
	// uint8  : channel
	// uint32 : frequency
	// uint32 : duration
	// Response parameters
	// None
	unsigned int offset, length = 9;
	unsigned char buffer[20];
	int status = PrepareHeader(buffer, LCMD_SET_SQUARE_WAVE, length, 0, &offset);
	if (!status)
	{
		length = WriteU8ToBuff(buffer, offset, channel);
		length = WriteU32ToBuff(buffer, length, freq);
		length = WriteU32ToBuff(buffer, length, duration);

		// send data and read response
		status = WriteAndRead(buffer, 20, &offset, length - offset, &length);
	}
	return status;
}

int LinxClient::DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned int timeout, unsigned int* width)
{
	// Command parameters
	// uint8 : response channel
	// uint8 : stimulation channel
	// uint8 : stimulation type
	// uint8 : response type
	// uint32 : timeout
	// Response parameters
	// uint32 : pulse width
	unsigned int offset, length = 8;
	unsigned char buffer[20];
	int status = PrepareHeader(buffer, LCMD_SET_SQUARE_WAVE, length, 4, &offset);
	if (!status)
	{
		length = WriteU8ToBuff(buffer, offset, respChan);
		length = WriteU8ToBuff(buffer, length, stimChan);
		length = WriteU8ToBuff(buffer, length, stimType);
		length = WriteU8ToBuff(buffer, length, respType);
		length = WriteU32ToBuff(buffer, length, timeout);

		// send data and read response
		status = WriteAndRead(buffer, 20, &offset, length - offset, &length);
		if (!status && length >= offset + 4)
		{
			ReadU32FromBuff(buffer, offset, width);
		}
	}
	return status;
}

//PWM
int LinxClient::PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//SPI
int LinxClient::SpiOpenMaster(unsigned char channel)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::SpiSetMode(unsigned char channel, unsigned char mode)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::SpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::SpiCloseMaster(unsigned char channel)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//I2C
int LinxClient::I2cOpenMaster(unsigned char channel)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::I2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::I2cClose(unsigned char channel)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//UART
int LinxClient::UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::UartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::UartGetBytesAvailable(unsigned char channel, unsigned int *numBytes)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::UartRead(unsigned char channel, unsigned int numBytes, unsigned char* recBuffer, unsigned int* numBytesRead)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::UartWrite(unsigned char channel, unsigned int numBytes, unsigned char* sendBuffer)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::UartClose(unsigned char channel)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//Servo
int LinxClient::ServoOpen(unsigned char numChans, unsigned char* chans)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::ServoSetPulseWidth(unsigned char numChans, unsigned char* chans, unsigned short* pulseWidths)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxClient::ServoClose(unsigned char numChans, unsigned char* chans)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

// General
void LinxClient::NonVolatileWrite(int address, unsigned char data)
{
	return;
}

unsigned char LinxClient::NonVolatileRead(int address)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int LinxClient::Initialize(LinxCommChannel *channel)
{
	unsigned char buffer[255];
	unsigned int dataRead, offset;

	if (m_CommChannel)
		m_CommChannel->Release();
	m_CommChannel = channel;
	if (!channel)
		return L_DISCONNECT;
	
	m_PacketNum = (unsigned int)((double)rand() / RAND_MAX * 0xFFFF);
	m_ListenerBufferSize = 255;

	int status = GetSyncCommand(true);
	if (!status)
	{
		status = GetU8ArrParameter(LCMD_GET_API_VER, buffer, 255, 0, &offset, &dataRead);
	}
	if (!status)
	{
		if (dataRead >= 3)
		{
			LinxApiMajor = buffer[offset++];
			LinxApiMinor = buffer[offset++];
			LinxApiSubminor = buffer[offset++];
		}
		status = GetU8ArrParameter(LCMD_GET_DEV_ID, buffer, 255, 0, &offset, &dataRead);
	}
	if (!status)
	{
		if (dataRead >= 2)
		{
			DeviceFamily = buffer[offset++];
			DeviceId = buffer[offset++];
		}
		status = GetU8ArrParameter(LCMD_GET_DEV_NAME, buffer, 255, 0, &offset, &dataRead);
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
	}

	if (!status)
	{
		status = GetU8ArrParameter(LCMD_GET_MAX_PACK_SIZE, buffer, 255, 0, &offset, &dataRead);
		if (!status && dataRead >= 4)
			ReadU32FromBuff(buffer, offset, &m_ListenerBufferSize);
		// ignore error

		status = GetU8ArrParameter(LCMD_GET_AI_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxAiChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_AO_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxAoChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_DIO_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxDioChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_PWM_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxPwmChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_QE_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxQeChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_UART_CHANS, buffer, 255, 1, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxUartChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_I2C_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxI2cChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_SPI_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxSpiChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_CAN_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxCanChannel, buffer, dataRead);

		status = GetU8ArrParameter(LCMD_GET_SERVO_CHANS, buffer, 255, 0, &offset, &dataRead);
		if (!status)
			CopyArrayToSet(IID_LinxServoChannel, buffer, dataRead);
	}
	//----Peripherals----
	// Uart
	// UartMaxBaud = 0;
	if (!status)
	{
		status = GetU32Parameter(LCMD_GET_UART_MAX_BAUD, &UartMaxBaud);
	}

	// AI
	// unsigned char AiResolution;
	// unsigned int AiRefDefault;
	// unsigned int AiRefSet;
	if (!status)
	{
		status = GetU32Parameter(LCMD_GET_AI_REF_VOLT, &AiRefSet);
	}
	if (!status)
	{
		status = GetU8Parameter(LCMD_GET_AI_RESOLUTION, &AiResolution);
	}
	// AO
	// AoResolution;
	// AoRefDefault;
	// AoRefSet;
	if (!status)
	{
		status = GetU8Parameter(LCMD_GET_AO_RESOLUTION, &AoResolution);
	}
/*
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
	if (status && m_CommChannel)
	{
		m_CommChannel->Release();
		m_CommChannel = NULL;
	}
	return status;
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
unsigned short LinxClient::GetNextPacketNum(void)
{
	return m_PacketNum++;
}

int LinxClient::PrepareHeader(unsigned char* buffer, unsigned short command, unsigned int dataLength, unsigned int expLength, unsigned int *headerLength)
{
	dataLength += 7;
	if (dataLength >= (int)m_ListenerBufferSize || dataLength > 0xFFFFFD)
		return LERR_MSG_TO_LONG;

	if (dataLength <= 248 && expLength <= 249)
	{
		*headerLength = WriteU16ToBuff(buffer, 0, 0xFF00 + (unsigned short)dataLength + 7);
	}
	else
	{
		*headerLength = WriteU32ToBuff(buffer, 0, 0xFE000000 + dataLength + 9);
	}
	*headerLength += WriteU16ToBuff(buffer, *headerLength, GetNextPacketNum());
	*headerLength += WriteU16ToBuff(buffer, *headerLength, command);
	return L_OK;
}

int LinxClient::WriteAndRead(unsigned char *buffer, unsigned int buffLength, unsigned int *headerLength, unsigned int dataLength, unsigned int *dataRead)
{
	unsigned short packetNum = GetU16FromBuff(buffer, *headerLength - 4);
	unsigned int start = getMilliSeconds();
	int timeout = m_Timeout;

	dataLength += *headerLength;
	buffer[dataLength] = ComputeChecksum(buffer, dataLength);
	int status = m_CommChannel->Write(buffer, dataLength + 1, timeout);
	if (!status)
	{
		if (m_Timeout >= 0)
		{
			timeout = m_Timeout - (getMilliSeconds() - start);
			if (timeout < 0)
				timeout = 0;
		}
		status = m_CommChannel->Read(buffer, 4, timeout, dataRead);
		if (!status && *dataRead == 4)
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

			if (m_Timeout >= 0)
			{
				timeout = m_Timeout - (getMilliSeconds() - start);
				if (timeout < 0)
					timeout = 0;
			}
			status = m_CommChannel->Read(buffer + 4, dataLength - 4, timeout, dataRead);
			if (!status)
			{
				*dataRead += 4;
				if (!ChecksumPassed(buffer, *dataRead - 1))
					return LERR_CHECKSUM;

				if (packetNum != GetU16FromBuff(buffer, *headerLength))
					return LERR_PACKET_NUM;

				*headerLength += 2;
				status = buffer[*headerLength++];
			}
		}
	}
	return status;
}

int LinxClient::GetSyncCommand(bool negotiate)
{
	unsigned char buffer[20];
	unsigned int headerLength;
	int status = PrepareHeader(buffer, LCMD_SYNC, negotiate, negotiate, &headerLength);
	if (!status)
	{
		unsigned int dataRead = 0;
		buffer[headerLength] = PROTOCOL_VERSION;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
		if (!status && dataRead)
			m_ProtocolVersion = buffer[headerLength];
	}
	return status;
}

int LinxClient::GetU8Parameter(unsigned short command, unsigned char *val)
{
	unsigned char buffer[20];
	unsigned int headerLength;
	int status = PrepareHeader(buffer, command, 0, 1, &headerLength);
	if (!status)
	{
		unsigned int dataRead;
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
	unsigned int headerLength;
	int status = PrepareHeader(buffer, command, 0, 2, &headerLength);
	if (!status)
	{
		unsigned int dataRead;
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
	unsigned int headerLength;
	int status = PrepareHeader(buffer, command, 0, 4, &headerLength);
	if (!status)
	{
		unsigned int dataRead;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
		if (!status && dataRead >= headerLength + 4)
		{
			ReadU32FromBuff(buffer, headerLength, val);
		}
	}
	return status;
}

int LinxClient::GetU8ArrParameter(unsigned short command, unsigned char *buffer, unsigned int bufLength, unsigned char param, unsigned int *headerLength, unsigned int *dataRead)
{
	int status = PrepareHeader(buffer, command, param ? 1 : 0, bufLength - 7, headerLength);
	if (!status)
	{
		buffer[*headerLength] = param;
		// send data and read response
		status = WriteAndRead(buffer, bufLength, headerLength, 0, dataRead);
		if (!status)
		{
			*dataRead -= *headerLength;
		}
	}
	return status;
}

void LinxClient::CopyArrayToSet(int type, unsigned char *arr, unsigned int length) 
{
	ClearChannels(type);
	for (unsigned int i = 0; i < length; i++)
		RegisterChannel(type, arr[i], NULL);
}
