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
static uint8_t m_Unknown[] = "Uninitialized Device Client";

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxClient::LinxClient(const uint8_t *uartDevice, uint32_t *baudrate, uint8_t dataBits, uint8_t stopBits,  LinxUartParity parity, int32_t timeout) : LinxDevice(NULL)
{
	m_DeviceName = m_Unknown;
	m_Timeout = timeout;
	m_CommChannel = NULL;

	LinxUartChannel *channelObj = NULL;
#if Unix
	channelObj = new LinxUnixUartChannel(m_Debug, uartDevice);
#elif Win32
	channelObj = new LinxWindowsUartChannel(m_Debug, uartDevice);
#endif
	if (channelObj)
	{
		int32_t status = channelObj->SetSpeed(*baudrate, baudrate);
		if (!status)			
			status = channelObj->SetParameters(dataBits, stopBits, parity);
		if (!status)
		{
			Initialize(channelObj);
		}
	}
}

LinxClient::LinxClient(const uint8_t *netAddress, uint16_t port, int32_t timeout) : LinxDevice(NULL)
{
	m_DeviceName = m_Unknown;
	m_Timeout = timeout;
	m_CommChannel = NULL;

	LinxCommChannel *channelObj = NULL;
#if Unix
	channelObj = new LinxUnixCommChannel(m_Debug, netAddress, port);
#elif Win32
	channelObj = new LinxWindowsCommChannel(m_Debug, netAddress, port);
#endif
	if (channelObj)
	{
		Initialize(channelObj);
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
int32_t LinxClient::IsInitialized(void)
{
	return (m_CommChannel != NULL);
}

uint8_t LinxClient::GetDeviceName(uint8_t *buffer, uint8_t length)
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
	return (uint8_t)len; 
}

// ANALOG
int32_t LinxClient::AnalogRead(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	int32_t status = VerifyChannels(IID_LinxAiChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8[length] : channels
		// Response parameters
		// uint8 : analog input resolution
		// uint8[] : analog values, bit-packed with resolution bits per channel
		uint32_t offset, length = (numChans * this->AiResolution + 7) / 8;
		uint8_t *buffer = (uint8_t *)malloc(10 + numChans);
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_ANALOG_READ, numChans, length, &offset);
			if (!status)
			{
				memcpy(buffer + offset, channels, numChans);

				// send data and read response
				status = WriteAndRead(buffer, 10 + length, &offset, numChans, values, 0, &length, getMsTicks(), m_Timeout);
				if (!status && buffer[offset] != this->AiResolution)
				{
					status = LANALOG_REF_VAL_ERROR;
				}
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::AnalogReadNoPacking(uint8_t numChans, uint8_t* channels, uint32_t* values)
{
	int32_t status = VerifyChannels(IID_LinxAiChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8[length] : channels
		// Response parameters
		// uint8 : analog input resolution
		// uint8[] : analog values, bit-packed with resolution bits per channel
		uint32_t offset, length = (numChans * this->AiResolution + 7) / 8;
		uint8_t *buffer = (uint8_t *)malloc(10 + Max(numChans, length));
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_ANALOG_READ, numChans, length, &offset);
			if (!status)
			{
				memcpy(buffer + offset, channels, numChans);

				// send data and read response
				status = WriteAndRead(buffer, 10 + Max(numChans, length), &offset, numChans, &length);
				if (!status && buffer[offset] == this->AiResolution)
				{
					for (int32_t i = 0; i < numChans; i++)
					{
					}
				}
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::AnalogReadValues(uint8_t numChans, uint8_t* channels, double* values)
{
	int32_t status = VerifyChannels(IID_LinxAiChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8[length] : channels
		// Response parameters
		// uint8 : analog input resolution
		// uint8[] : analog values, bit-packed with resolution bits per channel
		uint32_t offset, length = (numChans * this->AiResolution + 7) / 8;
		uint8_t *buffer = (uint8_t *)malloc(10 + Max(numChans, length));
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_ANALOG_READ, numChans, length, &offset);
			if (!status)
			{
				memcpy(buffer + offset, channels, numChans);

				// send data and read response
				status = WriteAndRead(buffer, 10 + Max(numChans, length), &offset, numChans, &length);
				if (!status && buffer[offset] == this->AiResolution)
				{
					for (int32_t i = 0; i < numChans; i++)
					{
					}
				}
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::AnalogSetRef(uint8_t mode, uint32_t voltage)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int32_t LinxClient::AnalogWrite(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	int32_t status = VerifyChannels(IID_LinxAoChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8 : number of channels
		// uint8 : analog output resolution
		// uint8[numChans] : channels
		// uint8[] : analog values, bit-packed with resolution bits per channel
		// Response parameters
		// None
		uint32_t offset, readLen = 0, length = (numChans * this->AoResolution + 7) / 8;
		uint8_t *buffer = (uint8_t *)malloc(12 + numChans);
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_ANALOG_WRITE, length, 0, &offset);
			if (!status)
			{
				buffer[offset + 0] = numChans;
				buffer[offset + 1] = this->AoResolution;
				memcpy(buffer + offset + 2, channels, numChans);
				status = WriteAndRead(buffer, 12 + numChans, &offset, 2 + numChans, values, length, &readLen, getMsTicks(), m_Timeout);
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::AnalogWriteValues(uint8_t numChans, uint8_t* channels, double* values)
{
	int32_t status = VerifyChannels(IID_LinxAoChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8 : number of channels
		// uint8 : analog output resolution
		// uint8[numChans] : channels
		// uint8[] : analog values, bit-packed with resolution bits per channel
		// Response parameters
		// None
		uint32_t offset, length = 2 + numChans + (numChans * this->AoResolution + 7) / 8;
		uint8_t *buffer = (uint8_t *)malloc(10 + length);
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_ANALOG_WRITE, length, 0, &offset);
			if (!status)
			{
				buffer[offset + 0] = numChans;
				buffer[offset + 1] = this->AoResolution;
				memcpy(buffer + offset + 2, channels, numChans);





				status = WriteAndRead(buffer, 10 + length, &offset, length, &length);
			}
			free(buffer);
		}
	}
	return status;
}

// DIGITAL
int32_t LinxClient::DigitalSetState(uint8_t numChans, uint8_t* channels, uint8_t* states)
{
	int32_t status = VerifyChannels(IID_LinxDioChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8 : num channels
		// uint8[numChans] : channels
		// uint8[numChans] : states
		// Response parameters
		// None
		uint32_t offset, length = 1 + 2 * numChans;
		uint8_t *buffer = (uint8_t *)malloc(10 + length);
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_SET_PIN_MODE, length, 0, &offset);
			if (!status)
			{
				buffer[offset] = numChans;
				memcpy(buffer + offset + 1, channels, numChans);
				memcpy(buffer + offset + 1 + numChans, states, numChans);

				// send data and read response
				status = WriteAndRead(buffer, 10 + length, &offset, length, &length);
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::DigitalWrite(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	int32_t status = VerifyChannels(IID_LinxDioChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8 : num channels
		// uint8[numChans] : channels
		// uint8[numChans + 7 / 8] : packed values
		// Response parameters
		// None
		uint32_t offset, length = 1 + numChans + ((numChans + 7) >> 3);
		uint8_t *buffer = (uint8_t *)malloc(10 + length);
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_DIGITAL_WRITE, length, 0, &offset);
			if (!status)
			{
				buffer[offset] = numChans;
				memcpy(buffer + offset + 1, channels, numChans);
				memcpy(buffer + offset + 1 + numChans, values, (numChans + 7) >> 3);

				// send data and read response
				status = WriteAndRead(buffer, 10 + length, &offset, length, &length);
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::DigitalWriteNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	int32_t status = VerifyChannels(IID_LinxDioChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8 : num channels
		// uint8[numChans] : channels
		// uint8[numChans] : values
		// Response parameters
		// None
		uint32_t offset, length = 1 + numChans + ((numChans + 7) >> 3);
		uint8_t *buffer = (uint8_t *)malloc(10 + length);
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_DIGITAL_WRITE, length, 0, &offset);
			if (!status)
			{
				uint8_t bitOffset = 0;
				uint8_t byteOffset = 0;
				uint8_t diVal = 0;

				buffer[offset] = numChans;
				memcpy(buffer + offset + 1, channels, numChans);

				for (int32_t i = 0; i < numChans; i++)
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
				status = WriteAndRead(buffer, 10 + length, &offset, length, &length);
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::DigitalRead(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	int32_t status = VerifyChannels(IID_LinxDioChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8 : num channels
		// uint8[numChans] : channels
		// Response parameters
		// uint8[numChans + 7 / 8] : packed values
		uint32_t offset, length = 1 + numChans;
		uint8_t *buffer = (uint8_t *)malloc(10 + length);
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_DIGITAL_READ, length, (numChans + 7) / 8, &offset);
			if (!status)
			{
				buffer[offset] = numChans;
				memcpy(buffer + offset + 1, channels, numChans);

				// send data and read response
				status = WriteAndRead(buffer, 10 + length, &offset, length, &length);
				if (!status && length >= (uint32_t)((numChans + 7) >> 3))
				{
					memcpy(values, buffer + offset, ((numChans + 7) >> 3));
				}
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::DigitalReadNoPacking(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	int32_t status = VerifyChannels(IID_LinxDioChannel, numChans, channels);
	if (!status)
	{
		// Command parameters
		// uint8 : num channels
		// uint8[numChans] : channels
		// Response parameters
		// uint8[numChans] : values
		uint32_t offset, length = 1 + numChans;
		uint8_t *buffer = (uint8_t *)malloc(10 + length);
		if (buffer)
		{
			status = PrepareHeader(buffer, LCMD_DIGITAL_READ, length, (numChans + 7) / 8, &offset);
			if (!status)
			{
				buffer[offset] = numChans;
				memcpy(buffer + offset + 1, channels, numChans);

				// send data and read response
				status = WriteAndRead(buffer, 10 + length, &offset, length, &length);
				if (!status && length >=  (uint32_t)((numChans + 7) >> 3))
				{
					for (int32_t i = 0; i < numChans; i++)
					{
						values[i] = buffer[offset + i / 8] << (7 - i % 8);
					}
				}
			}
			free(buffer);
		}
	}
	return status;
}

int32_t LinxClient::DigitalWriteSquareWave(uint8_t channel, uint32_t freq, uint32_t duration)
{
	int32_t status = VerifyChannels(IID_LinxDioChannel, 1, &channel);
	if (!status)
	{
		// Command parameters
		// uint8  : channel
		// uint32 : frequency
		// uint32 : duration
		// Response parameters
		// None
		uint32_t offset, length = 9;
		uint8_t buffer[20];
		status = PrepareHeader(buffer, LCMD_SET_SQUARE_WAVE, length, 0, &offset);
		if (!status)
		{
			length = WriteU8ToBuff(buffer, offset, channel);
			length = WriteU32ToBuff(buffer, length, freq);
			length = WriteU32ToBuff(buffer, length, duration);

			// send data and read response
			status = WriteAndRead(buffer, 20, &offset, length, &length);
		}
	}
	return status;
}

int32_t LinxClient::DigitalReadPulseWidth(uint8_t stimChan, uint8_t stimType, uint8_t respChan, uint8_t respType, uint32_t timeout, uint32_t* width)
{
	uint8_t channels[2] = {stimChan, respChan};
	int32_t status = VerifyChannels(IID_LinxDioChannel, 2, channels);
	if (!status)
	{
		// Command parameters
		// uint8 : response channel
		// uint8 : stimulation channel
		// uint8 : stimulation type
		// uint8 : response type
		// uint32 : timeout
		// Response parameters
		// uint32 : pulse width
		uint32_t offset, length = 8;
		uint8_t buffer[20];
		status = PrepareHeader(buffer, LCMD_SET_SQUARE_WAVE, length, 4, &offset);
		if (!status)
		{
			length = WriteU8ToBuff(buffer, offset, respChan);
			length = WriteU8ToBuff(buffer, length, stimChan);
			length = WriteU8ToBuff(buffer, length, stimType);
			length = WriteU8ToBuff(buffer, length, respType);
			length = WriteU32ToBuff(buffer, length, timeout);

			// send data and read response
			status = WriteAndRead(buffer, 20, &offset, length, &length);
			if (!status && length >= sizeof(uint32_t))
			{
				ReadU32FromBuff(buffer, offset, width);
			}
		}
	}
	return status;
}

//PWM
int32_t LinxClient::PwmSetDutyCycle(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	int32_t status = VerifyChannels(IID_LinxPwmChannel, numChans, channels);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

//SPI
int32_t LinxClient::SpiOpenMaster(uint8_t channel)
{
	int32_t status = VerifyChannels(IID_LinxSpiChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::SpiSetBitOrder(uint8_t channel, uint8_t bitOrder)
{
	int32_t status = VerifyChannels(IID_LinxSpiChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::SpiSetMode(uint8_t channel, uint8_t mode)
{
	int32_t status = VerifyChannels(IID_LinxSpiChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::SpiSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	int32_t status = VerifyChannels(IID_LinxSpiChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::SpiWriteRead(uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, uint8_t* sendBuffer, uint8_t* recBuffer)
{
	int32_t status = VerifyChannels(IID_LinxSpiChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::SpiCloseMaster(uint8_t channel)
{
	int32_t status = VerifyChannels(IID_LinxSpiChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

//I2C
int32_t LinxClient::I2cOpenMaster(uint8_t channel)
{
	int32_t status = VerifyChannels(IID_LinxI2cChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::I2cSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	int32_t status = VerifyChannels(IID_LinxI2cChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::I2cWrite(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint8_t* sendBuffer)
{
	int32_t status = VerifyChannels(IID_LinxI2cChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::I2cRead(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, uint8_t* recBuffer)
{
	int32_t status = VerifyChannels(IID_LinxI2cChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::I2cClose(uint8_t channel)
{
	int32_t status = VerifyChannels(IID_LinxI2cChannel, 1, &channel);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

//UART
int32_t LinxClient::UartOpen(uint8_t channel, LinxUartChannel **channelObj)
{
	int32_t status = VerifyChannels(IID_LinxUartChannel, 1, &channel);
	if (!status)
	{
		// Command parameters
		// uint8 : channel
		// uint32 : baudrate
		// Response parameters
		// uint32 : actual baudrate
		uint8_t buffer[20];
		uint32_t offset, length = 1 + sizeof(uint32_t);
		
		status = PrepareHeader(buffer, LCMD_UART_OPEN, length, sizeof(uint32_t), &offset);
		if (!status)
		{
			buffer[offset] = channel;
			WriteU32ToBuff(buffer, offset, 9600);

			// send data and read response
			status = WriteAndRead(buffer, 20, &offset, length, &length);
			if (!status && length >=  sizeof(uint32_t))
			{

			}
		}
	}
	return status;
}

int32_t LinxClient::UartOpen(const unsigned char *deviceName, uint8_t *channel, LinxUartChannel **channelObj)
{
	// Command parameters
	// uint8[] : device name
	// Response parameters
	// uint8 : assigned channel
	int32_t status = LERR_MEMORY;
	uint32_t offset, length = (uint32_t)strlen((const char*)deviceName);
	uint8_t *buffer = (uint8_t *)malloc(10 + length);
	if (buffer)
	{
		status = PrepareHeader(buffer, LCMD_UART_OPEN_BY_NAME, length, 1, &offset);
		if (!status)
		{
			memcpy(buffer + offset, deviceName, length);
			// send data and read response
			status = WriteAndRead(buffer, 10 + length, &offset, length, &length);
			if (!status && length >= 1)
			{
				LinxUartChannel *channelObj = new LinxClientUartChannel(m_Debug, this, buffer[offset], m_ProtocolVersion, deviceName);
				RegisterChannel(IID_LinxUartChannel, buffer[offset], channelObj);
				*channel = buffer[offset];
			}
		}
	}
	return status;
}

//Servo
int32_t LinxClient::ServoOpen(uint8_t numChans, uint8_t* channels)
{
	int32_t status = VerifyChannels(IID_LinxServoChannel, numChans, channels);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::ServoSetPulseWidth(uint8_t numChans, uint8_t* channels, uint16_t* pulseWidths)
{
	int32_t status = VerifyChannels(IID_LinxServoChannel, numChans, channels);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

int32_t LinxClient::ServoClose(uint8_t numChans, uint8_t* channels)
{
	int32_t status = VerifyChannels(IID_LinxServoChannel, numChans, channels);
	if (!status)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	return status;
}

// General
void LinxClient::NonVolatileWrite(int32_t address, uint8_t data)
{
	return;
}

uint8_t LinxClient::NonVolatileRead(int32_t address)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int32_t LinxClient::Initialize(LinxCommChannel *channel)
{
	uint8_t buffer[255];
	uint32_t dataRead, offset;

	if (m_CommChannel)
		m_CommChannel->Release();
	m_CommChannel = channel;
	if (!channel)
		return L_DISCONNECT;
	
	m_PacketNum = (uint32_t)((double)rand() / RAND_MAX * 0xFFFF);

	int32_t status = GetSyncCommand(true);
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
		{
			ReadU32FromBuff(buffer, offset, &m_ListenerBufferSize);
		}
		//ignore error

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
	// uint8_t AiResolution;
	// uint32_t AiRefDefault;
	// uint32_t AiRefSet;
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

	uint32_t ethernetIp;
	uint16_t ethernetPort;

	uint32_t WifiIp;
	uint16_t WifiPort;
	uint8_t WifiSsidSize;
	char WifiSsid[32];
	uint8_t WifiSecurity;
	uint8_t WifiPwSize;
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
uint16_t LinxClient::GetNextPacketNum(void)
{
	return m_PacketNum++;
}


/*
   packet format
	uint8_t : header marker, 0xFF or 0xFE
	if header marker == 0xFF
	  uint8_t[1] : packet length
	if header marker == 0xFE
	  uint8_t[3] : MSB first packet length
	uint16_t : packet number
	uint16_t : command
	uint8_t[n - header - 1] : data
	uint8_t : checksum
*/
int32_t LinxClient::PrepareHeader(uint8_t* buffer, uint16_t command, uint32_t dataLength, uint32_t expLength, uint32_t *headerLength)
{
	uint32_t overhead = 7;
	if (!m_ProtocolVersion)
	{
		if (dataLength > 0xF8 || (dataLength + overhead) >= (uint32_t)m_ListenerBufferSize)
			return LERR_MSG_TO_LONG;
	}
	else
	{
		if (dataLength > 248 || expLength > 249)
			overhead = 9;
		if (dataLength > 0xFFFFF6 || (dataLength + overhead) >= (uint32_t)m_ListenerBufferSize)
			return LERR_MSG_TO_LONG;
	}

	if (overhead == 7)
	{
		*headerLength = WriteU16ToBuff(buffer, 0, 0xFF00 + (uint16_t)dataLength + overhead);
	}
	else
	{
		*headerLength = WriteU32ToBuff(buffer, 0, 0xFE000000 + dataLength + overhead);
	}
	*headerLength += WriteU16ToBuff(buffer, *headerLength, GetNextPacketNum());
	*headerLength += WriteU16ToBuff(buffer, *headerLength, command);
	return L_OK;
}

int32_t LinxClient::WriteAndRead(uint8_t *buffer, uint32_t bufLength, uint32_t *headerOffset, uint32_t dataLength, uint32_t *dataRead)
{
	return WriteAndRead(buffer, bufLength, headerOffset, dataLength, NULL, 0, dataRead, getMsTicks(), m_Timeout);
}

int32_t LinxClient::WriteAndRead(uint8_t *header, uint32_t headerLength, uint32_t *headerOffset, uint32_t headerData, uint8_t *dataBuffer, uint32_t dataLength, uint32_t *dataRead, uint32_t start, int32_t timeout)
{
	int32_t status;
	uint8_t checksum = ComputeChecksum(0, header, *headerOffset + headerData);
	uint16_t packetNum = GetU16FromBuff(header, *headerOffset - 4);

	if (!dataLength)
	{
		header[*headerOffset + headerData] = checksum;
		status = m_CommChannel->Write(header, *headerOffset + headerData + 1, start, timeout);
	}
	else
	{
		checksum = ComputeChecksum(checksum, dataBuffer, dataLength);
		status = m_CommChannel->Write(header, *headerOffset, start, timeout);
		if (!status)
		{
			status = m_CommChannel->Write(dataBuffer, dataLength, start, timeout);
			if (!status)
			{
				status = m_CommChannel->Write(&checksum, 1, start, timeout);
			}
		}
	}
	if (!status)
	{
		status = m_CommChannel->Read(header, 5, start, timeout, dataRead);
		if (!status && *dataRead == 5)
		{
			uint32_t extraData = 0;
			if ((header[0] & 0xFE) != 0xFE)
			{
				return LERR_INVALID_FRAME;
			}

			if (header[0] == 0xFF)
			{
				*headerOffset = 2;
				dataLength = header[1];
			}
			else
			{
				*headerOffset = 4;
				dataLength = GetU32FromBuff(header, 0) & 0x00FFFFFF;
				extraData = 2;
			}

			if (!dataBuffer || !(*dataRead))
			{
				extraData = dataLength - 5;
			}
				
			if (extraData)
			{
				status = m_CommChannel->Read(header + 5, extraData, start, timeout, dataRead);
				if (status || *dataRead != extraData)
				{
					return status ? status : LERR_IO;
				}
			}
			if (packetNum != GetU16FromBuff(header, *headerOffset))
			{
				*headerOffset += 3;
				return LERR_PACKET_NUM;
			}
			*headerOffset += 2;
			
			status = GetU8FromBuff(header, *headerOffset);
			*headerOffset += 1;
			if (status)
			{
				return status;
			}

			if (!dataBuffer || !(*dataRead))
			{
				checksum = ComputeChecksum(0, header, extraData + 4);
				if (checksum != header[extraData + 4])
				{
					return LERR_CHECKSUM;
				}
				*dataRead = dataLength - (*headerOffset + 1);
			}
			else
			{
				checksum = ComputeChecksum(0, header, extraData + 5);
				dataLength -= *headerOffset + 1;
				status = m_CommChannel->Read(dataBuffer, dataLength, start, timeout, dataRead);
				if (!status && *dataRead == dataLength)
				{
					checksum = ComputeChecksum(checksum, dataBuffer, *dataRead);
					status = m_CommChannel->Read(header + *headerOffset, 1, start, timeout, dataRead);
					if (!status && *dataRead == 1)
					{
						if (GetU8FromBuff(header, *headerOffset) != checksum)
							return LERR_CHECKSUM;
					}
					*dataRead = dataLength;
				}
			}
		}
	}
	return status;
}

int32_t LinxClient::GetSyncCommand(bool negotiate)
{
	uint8_t buffer[20];
	uint32_t headerLength;
	int32_t status = PrepareHeader(buffer, LCMD_SYNC, negotiate, negotiate, &headerLength);
	if (!status)
	{
		uint32_t dataRead = 0;
		buffer[headerLength] = PROTOCOL_VERSION;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 1, &dataRead);
		if (!status && dataRead >= sizeof(uint8_t))
			m_ProtocolVersion = buffer[headerLength];
	}
	return status;
}

int32_t LinxClient::GetU8Parameter(uint16_t command, uint8_t *val)
{
	uint8_t buffer[20];
	uint32_t headerLength;
	int32_t status = PrepareHeader(buffer, command, 0, 1, &headerLength);
	if (!status)
	{
		uint32_t dataRead;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
		if (!status && dataRead >= sizeof(uint8_t))
		{
			ReadU8FromBuff(buffer, headerLength, val);
		}
	}
	return status;
}

int32_t LinxClient::GetU16Parameter(uint16_t command, uint16_t *val)
{
	uint8_t buffer[20];
	uint32_t headerLength;
	int32_t status = PrepareHeader(buffer, command, 0, 2, &headerLength);
	if (!status)
	{
		uint32_t dataRead;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
		if (!status && dataRead >= sizeof(uint16_t))
		{
			ReadU16FromBuff(buffer, headerLength, val);
		}
	}
	return status;
}

int32_t LinxClient::GetU32Parameter(uint16_t command, uint32_t *val)
{
	uint8_t buffer[20];
	uint32_t headerLength;
	int32_t status = PrepareHeader(buffer, command, 0, 4, &headerLength);
	if (!status)
	{
		uint32_t dataRead;
		// send data and read response
		status = WriteAndRead(buffer, 20, &headerLength, 0, &dataRead);
		if (!status && dataRead  >= sizeof(uint32_t))
		{
			ReadU32FromBuff(buffer, headerLength, val);
		}
	}
	return status;
}

int32_t LinxClient::GetU8ArrParameter(uint16_t command, uint8_t *buffer, uint32_t bufLength, uint8_t param, uint32_t *headerLength, uint32_t *dataRead)
{
	int32_t status = PrepareHeader(buffer, command, param ? 1 : 0, bufLength - 7, headerLength);
	if (!status)
	{
		buffer[*headerLength] = param;
		// send data and read response
		status = WriteAndRead(buffer, bufLength, headerLength, param ? 1 : 0, dataRead);
	}
	return status;
}

void LinxClient::CopyArrayToSet(int32_t type, uint8_t *arr, uint32_t length, LinxChannel *chan) 
{
	ClearChannels(type);
	for (uint32_t i = 0; i < length; i++)
	{
		RegisterChannel(type, arr[i], chan);
	}
}

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxClientUartChannel::LinxClientUartChannel(LinxFmtChannel *debug, LinxClient *client, uint8_t channel, uint8_t protVersion, const unsigned char *deviceName) : LinxUartChannel(debug, deviceName)
{
	m_Client = client;
	m_Client->AddRef();
	m_Channel = channel;
	m_ProtocolVersion = protVersion;
}

LinxClientUartChannel::~LinxClientUartChannel(void)
{
	m_Client->Release();
}

int32_t LinxClientUartChannel::SetSpeed(uint32_t speed, uint32_t* actualSpeed)
{
	// Command parameters
	// uint8 : channel
	// uint32 : baudrate
	// Response parameters
	// uint32 : actual baudrate
	uint8_t buffer[20];
	uint32_t offset, length = 1 + sizeof(uint32_t);
	int32_t status = m_Client->PrepareHeader(buffer, LCMD_UART_SET_BAUD, length, sizeof(uint32_t), &offset);
	if (!status)
	{
		buffer[offset] = m_Channel;
		WriteU32ToBuff(buffer, offset, speed);

		// send data and read response
		status = m_Client->WriteAndRead(buffer, 20, &offset, length, &length);
		if (!status && length >= sizeof(uint32_t) && actualSpeed)
		{
			*actualSpeed = GetU32FromBuff(buffer, offset);
		}
	}
	return status;
}

int32_t LinxClientUartChannel::SetParameters(uint8_t dataBits, uint8_t stopBits, LinxUartParity parity)
{
	// Command parameters
	// uint8 : channel
	// uint8 : data bits
	// uint8 : stop bits
	// uint8 : parity
	// Response parameters
	// None
	uint8_t buffer[20];
	uint32_t offset, length = 4;
	int32_t status = m_Client->PrepareHeader(buffer, LCMD_UART_SET_ATTRIBUTE, length, 0, &offset);
	if (!status)
	{
		buffer[offset + 0] = m_Channel;
		buffer[offset + 1] = dataBits;
		buffer[offset + 2] = stopBits;
		buffer[offset + 3] = parity;

		// send data and read response
		status = m_Client->WriteAndRead(buffer, 20, &offset, length, &length);
	}
	return status;
}

int32_t LinxClientUartChannel::Read(uint8_t* dataBuffer, uint32_t numBytes, uint32_t start, int32_t timeout, uint32_t* numBytesRead)
{
	int32_t status;
	uint8_t header[20];
	uint32_t offset = 0, length = 1;
	if (!dataBuffer || !numBytes)
	{
		// Command parameters
		// uint8 : channel
		// Response parameters
		//  uint8 : available bytes
		// or
		//  uint32 : available bytes
		status = m_Client->PrepareHeader(header, LCMD_UART_GET_BYTES, length, sizeof(uint32_t), &offset);
		if (!status)
		{
			header[offset] = m_Channel;

			// send data and read response
			status = m_Client->WriteAndRead(header, 20, &offset, length, NULL, 0, &numBytes, start, timeout);
			if (!status)
			{
				if (numBytes >= sizeof(uint32_t))
				{
					*numBytesRead = GetU32FromBuff(header, offset);
				}
				else if (numBytes >= sizeof(uint8_t))
				{
					*numBytesRead = header[offset];
				}
				else
				{
					*numBytesRead = 0;
				}
			}
			
		}
	}
	else
	{
		// Command parameters
		// Either:
		//  uint8 : channel
		//  uint8 : bytes to read
		// or
		//  uint8 : 0
		//  uint8 : channel
		//  int32 : timeout
		//  uint32 : bytes to read
		// end
		// Response parameters
		// uint8[] : read bytes
		if (m_ProtocolVersion && (numBytes > 249 || timeout >= 0))
		{
			length = 10;
		}
		else
		{
			length = 2;
		}
		status = m_Client->PrepareHeader(header, LCMD_UART_READ, length, numBytes, &offset);
		if (!status)
		{
			if (length == 10)
			{
				WriteU8ToBuff(header, offset + 0, 0);
				WriteU8ToBuff(header, offset + 1, m_Channel);
				WriteU32ToBuff(header, offset + 2, timeout);
				WriteU32ToBuff(header, offset + 6, numBytes);
			}
			else
			{
				WriteU8ToBuff(header, offset + 0, m_Channel);
				WriteU8ToBuff(header, offset + 1, numBytes);
			}
			*numBytesRead = numBytes;

			// send data and read response
			status = m_Client->WriteAndRead(header, 20, &offset, length, dataBuffer, 0, numBytesRead, start, timeout);
		}
	}
	return status;
}

int32_t LinxClientUartChannel::Write(const uint8_t* dataBuffer, uint32_t numBytes, uint32_t start, int32_t timeout)
{
	// Command parameters
	// Either:
    //  uint8 : channel
	// or
	//  uint8 : 0
	//  uint8 : channel
	//  int32 : timeout
	//  int32 : number of bytes
	// end
	// uint8[] : bytes to write
	// Response parameters
	// None
	int32_t status;
	uint8_t header[20];
	uint32_t offset, length, dataRead = 0;				
	if (m_ProtocolVersion && (numBytes > 248 || timeout >= 0))
	{
		length = 10;
	}
	else
	{
		length = 2;
	}
	status = m_Client->PrepareHeader(header, LCMD_UART_WRITE, length, sizeof(uint32_t), &offset);
	if (!status)
	{
		if (length == 10)
		{
			WriteU8ToBuff(header, offset + 0, 0);
			WriteU8ToBuff(header, offset + 1, m_Channel);
			WriteU32ToBuff(header, offset + 2, timeout);
			WriteU32ToBuff(header, offset + 6, numBytes);
		}
		else
		{
			WriteU8ToBuff(header, offset + 0, m_Channel);
			WriteU8ToBuff(header, offset + 1, numBytes);
		}

		// send data and read response
		status = m_Client->WriteAndRead(header, 20, &offset, length, (uint8_t*)dataBuffer, numBytes, &dataRead, start, timeout);
	}
	return status;
}

int32_t LinxClientUartChannel::Close(void)
{
	// Command parameters
	// uint8 : channel
	// Response parameters
	// None
	uint8_t buffer[20];
	uint32_t offset, length = 1;
	int32_t status = m_Client->PrepareHeader(buffer, LCMD_UART_CLOSE, length, sizeof(uint32_t), &offset);
	if (!status)
	{
		buffer[offset] = m_Channel;

		// send data and read response
		status = m_Client->WriteAndRead(buffer, 20, &offset, length, &length);
	}
	return status;
}
