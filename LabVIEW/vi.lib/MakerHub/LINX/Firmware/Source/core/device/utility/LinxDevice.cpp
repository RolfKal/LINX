/****************************************************************************************
**  LINX - Generic LINX device.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written by Sam Kristoff
**  Modifications by Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stddef.h>
#include <string.h>
#include "LinxDefines.h"
#if Unix
#include "LinxLinuxChannel.h"
#elif Win32
#include "LinxWindowsChannel.h"
#endif
#include "LinxDevice.h"
#include "LinxUtilities.h"

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxDevice::LinxDevice(LinxFmtChannel *debug)
{
	if (debug)
	{
		debug->AddRef();
	}
	else
	{
		debug = new LinxFmtChannel();
	}
	m_Debug = debug;

	//LINX API Version
	LinxApiMajor = 0;
	LinxApiMinor = 0;
	LinxApiSubminor = 0;

	DeviceFamily = 0xFE;
	DeviceId = 0x00;	

	//----Peripherals----

	// DIO

	// AI
	AiResolution = 0;
	AiRefDefault = 0;
	AiRefSet = 0;

	// AO
	AoResolution = 0;
	AoRefDefault = 0;
	AoRefSet = 0;

	// PWM

	// QE

	// UART
	UartMaxBaud = 0;

	// I2C

	// SPI

	// CAN

	// Servo

	// Debug
}

LinxDevice::~LinxDevice(void)
{
	if (m_Debug)
		m_Debug->Release();

	// Close Ai channels
	ClearChannels(IID_LinxAiChannel);

	// Close Ao channels
	ClearChannels(IID_LinxAoChannel);

	// Close Dio channels
	ClearChannels(IID_LinxDioChannel);

	// Close Pwm channels
	ClearChannels(IID_LinxPwmChannel);

	// Close Uart channels
	ClearChannels(IID_LinxUartChannel);

	// Close I2C channels
	ClearChannels(IID_LinxI2cChannel);

	// Close Spi channels
	ClearChannels(IID_LinxSpiChannel);

	// Close Can channels
	ClearChannels(IID_LinxCanChannel);

	// Close Servo channels
	ClearChannels(IID_LinxServoChannel);
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
void LinxDevice::DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer)
{
	if (direction == RX)
	{
		m_Debug->Write("Received :: ");
	}
	else if(direction == TX)
	{
		m_Debug->Write("Sending  :: ");
	}
		
	for (int32_t i = 0; i < packetBuffer[1]; i++)
	{
		m_Debug->Write("[");
		m_Debug->Write(packetBuffer[i], HEX);
		m_Debug->Write("]");
	}
	m_Debug->Writeln();
		
	if (direction == TX)
	{
		//Add Second New Line After TX
		m_Debug->Writeln();
	}
}

void LinxDevice::DebugEnable(LinxCommChannel *channel)
{
	m_Debug->SetDebugChannel(channel);
}

void LinxDevice::DebugDisable(void)
{
	m_Debug->SetDebugChannel(NULL);
}

LinxFmtChannel* LinxDevice::GetDebug(void)
{
	m_Debug->AddRef();
	return m_Debug;
}


//--------------------------------------------------------Analog-------------------------------------------------------
int32_t LinxDevice::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int32_t status = L_OK;
	uint8_t responseByteOffset = 0;
	uint8_t responseBitsRemaining = 8;
	uint8_t dataBitsRemaining;

	values[responseByteOffset] = 0x00;				// Clear next response byte
	for (int32_t i = 0; i < numChans; i++)
	{
		LinxAiChannel *channelObj = (LinxAiChannel*)LookupChannel(IID_LinxAiChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;

		// Acquire AI Sample
		uint32_t aiVal = 0;
		status = channelObj->Read(&aiVal);
		if (status)
			return status;

		dataBitsRemaining = AiResolution;
		// Byte packed AI values in response data
		while (dataBitsRemaining > 0)
		{
			*(values + responseByteOffset) |= (uint8_t)((aiVal >> (AiResolution - dataBitsRemaining)) << (8 - responseBitsRemaining));

			if (responseBitsRemaining > dataBitsRemaining)
			{
				// Current Byte Still Has Empty Bits
				responseBitsRemaining -= dataBitsRemaining;
				dataBitsRemaining = 0;
			}
			else
			{
				// Current Byte Full
				dataBitsRemaining -= responseBitsRemaining;
				responseByteOffset++;
				responseBitsRemaining = 8;
				values[responseByteOffset] = 0x00;	// Clear next response byte
			}
		}
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, uint32_t* values)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxAiChannel *channelObj = (LinxAiChannel*)LookupChannel(IID_LinxAiChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		status = channelObj->Read(values + i);
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::AnalogReadValues(uint8_t numChans, uint8_t* channels, double* values)
{
	int32_t status = L_OK;
	uint32_t aiVal, resolution = ((1 << this->AiResolution) - 1);
	double scale = ((double)this->AiRefSet / 1000000.0) / resolution;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxAiChannel *channelObj = (LinxAiChannel*)LookupChannel(IID_LinxAiChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		status = channelObj->Read(&aiVal);
		if (!status)
		{
			values[i] = scale * aiVal;
		}
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::AnalogSetRef(unsigned char mode, uint32_t voltage)
{
	int32_t status = L_FUNCTION_NOT_SUPPORTED;
	
	return status;
}

int32_t LinxDevice::AnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int32_t status = L_OK;
	uint32_t aoVal = 0;
	uint8_t sourceByteOffset = 0;
	uint8_t sourceBitsRemaining = 8;
	uint8_t dataBitsRemaining;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxAoChannel *channelObj = (LinxAoChannel*)LookupChannel(IID_LinxAoChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;

		dataBitsRemaining = AoResolution;
		aoVal = 0;
		while (dataBitsRemaining)
		{
			aoVal |= ((uint32_t)*(values + sourceByteOffset) << (AoResolution - dataBitsRemaining)) >> (8 - sourceBitsRemaining);
			if (sourceBitsRemaining > dataBitsRemaining)
			{
				// Current byte still has unused bits
				sourceBitsRemaining -= dataBitsRemaining;
				dataBitsRemaining = 0;
			}
			else
			{
				// Current Byte Used up
				dataBitsRemaining -= sourceBitsRemaining;
				sourceByteOffset++;
				sourceBitsRemaining = 8;
			}
		}
		status = channelObj->Write(aoVal);
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::AnalogWriteValues(uint8_t numChans, uint8_t* channels, double *values)
{
	int32_t status = L_OK;
	uint32_t aoVal, resolution = ((1 << this->AoResolution) - 1);
	double scale = resolution / ((double)this->AoRefSet / 1000000.0);
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxAoChannel *channelObj = (LinxAoChannel*)LookupChannel(IID_LinxAoChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		aoVal = (uint32_t)(values[i] * scale);
		status = channelObj->Write(aoVal);
		channelObj->Release();
	}
	return status;
}

//--------------------------------------------------------Digital-------------------------------------------------------
int32_t LinxDevice::DigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char* states)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *channelObj = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		status = channelObj->SetState(states[i]);
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *channelObj = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		status = channelObj->Write((values[i / 8] >> (i % 8)) & 0x01);
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *channelObj = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		status = channelObj->Write(values[i]);
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	unsigned char bitOffset = 8;
	unsigned char byteOffset = 0;
	unsigned char retVal = 0;
	unsigned char diVal = 0;
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *channelObj = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;

		//If bitOffset is 0 we have to start a new byte, store old byte and increment offset
		if (bitOffset == 0)
		{
			//Insert retVal Into Response Buffer
			values[byteOffset] = retVal;
			retVal = 0x00;
			byteOffset++;
			bitOffset = 7;
		}
		else
		{
			bitOffset--;
		}
		status = channelObj->Read(&diVal);
		if (!status)
			retVal |= (diVal << bitOffset);	//Read pin and insert value into retVal
		channelObj->Release();
	}
	//Store Last Byte
	values[byteOffset] = retVal;
	return status;
}

int32_t LinxDevice::DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *channelObj = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		status = channelObj->Read(&values[i]);
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::DigitalWriteSquareWave(unsigned char channel, uint32_t freq, uint32_t duration)
{
	LinxDioChannel *channelObj = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channel);
	if (!channelObj)
		return LERR_BADCHAN;
		
	int32_t status = channelObj->WriteSquareWave(freq, duration);
	channelObj->Release();
	return status;
}

int32_t LinxDevice::DigitalReadPulseWidth(unsigned char channel, unsigned char stimType, unsigned char respChan, unsigned char respType, uint32_t timeout, uint32_t* width)
{
	LinxDioChannel *channelObj = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channel);
	if (!channelObj)
		return LERR_BADCHAN;
		
	int32_t status = channelObj->ReadPulseWidth(stimType, respChan, respType, timeout, width);
	channelObj->Release();
	return status;
}

// ---------------- PWM Functions ------------------ 
int32_t LinxDevice::PwmSetDutyCycle(uint8_t numChans, uint8_t* channels, uint8_t* values)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxPwmChannel *channelObj = (LinxPwmChannel*)LookupChannel(IID_LinxPwmChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		status = channelObj->SetDutyCycle(values[i]);
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::PwmSetFrequency(uint8_t numChans, uint8_t* channels, uint32_t* values)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxPwmChannel *channelObj = (LinxPwmChannel*)LookupChannel(IID_LinxPwmChannel, channels[i]);
		if (!channelObj)
			return LERR_BADCHAN;
		
		status = channelObj->SetFrequency(values[i]);
		channelObj->Release();
	}
	return status;
}

// ---------------- SPI Functions ------------------ 
int32_t LinxDevice::SpiOpenMaster(uint8_t channel)
{
	LinxSpiChannel *channelObj = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Open();
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;

}

int32_t LinxDevice::SpiSetBitOrder(uint8_t channel, uint8_t bitOrder)
{
	LinxSpiChannel *channelObj = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->SetBitOrder(bitOrder);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::SpiSetMode(uint8_t channel, uint8_t mode)
{
	LinxSpiChannel *channelObj = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->SetMode(mode);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::SpiSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	LinxSpiChannel *channelObj = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->SetSpeed(speed, actualSpeed);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::SpiWriteRead(uint8_t channel, uint8_t frameSize, uint8_t numFrames, uint8_t csChan, uint8_t csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	LinxSpiChannel *channelObj = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->WriteRead(frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::SpiCloseMaster(uint8_t channel)
{
	LinxSpiChannel *channelObj = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Close();
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

// ---------------- I2C Functions ------------------ 
int32_t LinxDevice::I2cOpenMaster(uint8_t channel)
{
	LinxI2cChannel *channelObj = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Open();
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::I2cSetSpeed(uint8_t channel, uint32_t speed, uint32_t* actualSpeed)
{
	LinxI2cChannel *channelObj = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->SetSpeed(speed, actualSpeed);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::I2cWrite(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, unsigned char* sendBuffer)
{
	LinxI2cChannel *channelObj = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Write(slaveAddress, eofConfig, numBytes, sendBuffer);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::I2cRead(uint8_t channel, uint8_t slaveAddress, uint8_t eofConfig, uint8_t numBytes, uint32_t timeout, unsigned char* recBuffer)
{
	LinxI2cChannel *channelObj = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Read(slaveAddress, eofConfig, numBytes, timeout, recBuffer);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::I2cTransfer(uint8_t channel, uint8_t slaveAddress, int32_t numFrames, int32_t *flags, int32_t *numBytes, uint32_t timeout, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	LinxI2cChannel *channelObj = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Transfer(slaveAddress, numFrames, flags, numBytes, timeout, sendBuffer, recBuffer);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::I2cClose(uint8_t channel)
{
	LinxI2cChannel *channelObj = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Close();
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

// ---------------- UART Functions ------------------ 
int32_t LinxDevice::UartOpen(uint8_t channel, LinxUartChannel **comm)
{
	LinxUartChannel *channelObj = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (channelObj)
	{
		if (comm)
			*comm = channelObj;
		else
			channelObj->Release();
		return L_OK;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::UartOpen(const unsigned char *deviceName, uint8_t *channel, LinxUartChannel **comm)
{
	LinxUartChannel *channelObj = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, deviceName, channel);
	if (!channelObj)
	{
#if Unix
		channelObj = new LinxUnixUartChannel(m_Debug, deviceName);
#elif Win32
		channelObj = new LinxWindowsUartChannel(m_Debug, deviceName);
#endif
		if (!channelObj)
			return LERR_BADPARAM;

		*channel = RegisterChannel(IID_LinxUartChannel, channelObj);
	}
	if (comm)
		*comm = channelObj;
	else
		channelObj->Release();
	return L_OK;
}

int32_t LinxDevice::UartSetBaudRate(unsigned char channel, uint32_t baudRate, uint32_t* actualBaud)
{
	LinxUartChannel *channelObj = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->SetSpeed(baudRate, actualBaud);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::UartSetParameters(unsigned char channel, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity)
{
	LinxUartChannel *channelObj = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->SetParameters(dataBits, stopBits, parity);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::UartGetBytesAvailable(unsigned char channel, uint32_t *numBytes)
{
	LinxUartChannel *channelObj = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Read(NULL, 0, 0, numBytes);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::UartRead(unsigned char channel, uint32_t numBytes, unsigned char* recBuffer, int32_t timeout, uint32_t* numBytesRead)
{
	LinxUartChannel *channelObj = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Read(recBuffer, numBytes, timeout, numBytesRead);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::UartWrite(unsigned char channel, uint32_t numBytes, unsigned char* sendBuffer, int32_t timeout)
{
	LinxUartChannel *channelObj = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Write(sendBuffer, numBytes, timeout);
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int32_t LinxDevice::UartClose(unsigned char channel)
{
	LinxUartChannel *channelObj = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (channelObj)
	{
		int32_t status = channelObj->Close();
		channelObj->Release();
		return status;
	}
	return LERR_BADPARAM;
}

//------------------------------------- Servo -------------------------------------
int32_t LinxDevice::ServoOpen(unsigned char numChans, unsigned char* channels)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxServoChannel *channelObj = (LinxServoChannel*)LookupChannel(IID_LinxServoChannel, channels[i]);
		if (!channelObj)
			return LERR_BADPARAM;
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::ServoSetPulseWidth(unsigned char numChans, unsigned char* channels, uint16_t* pulseWidths)
{
	int32_t status = L_OK;
	for (int32_t i = 0; !status && i < numChans; i++)
	{
		LinxServoChannel *channelObj = (LinxServoChannel*)LookupChannel(IID_LinxServoChannel, channels[i]);
		if (!channelObj)
			return LERR_BADPARAM;

		status = channelObj->SetPulseWidth(pulseWidths[i]);
		channelObj->Release();
	}
	return status;
}

int32_t LinxDevice::ServoClose(unsigned char numChans, unsigned char* channels)
{
	int32_t status = L_OK;
	for (int32_t i = 0; i < numChans; i++)
	{
		LinxServoChannel *channelObj = (LinxServoChannel*)LookupChannel(IID_LinxServoChannel, channels[i]);
		// don't abort loop on errors on close
		if (channelObj)
		{
			status = channelObj->Close();
			channelObj->Release();
		}
	}
	return status;
}

//----------------- WS2812 Functions -----------------------------
int32_t LinxDevice::Ws2812Open(uint16_t numLeds, unsigned char dataChan)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int32_t LinxDevice::Ws2812WriteOnePixel(uint16_t pixelIndex, unsigned char red, unsigned char green, unsigned char blue, unsigned char refresh)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int32_t LinxDevice::Ws2812WriteNPixels(uint16_t startPixel, uint16_t numPixels, unsigned char* data, unsigned char refresh)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int32_t LinxDevice::Ws2812Refresh(void)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int32_t LinxDevice::Ws2812Close(void)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//----------------- Nonvolatile Functions -----------------------------
void LinxDevice::NonVolatileWrite(int32_t address, unsigned char data)
{

}

unsigned char LinxDevice::NonVolatileRead(int32_t address)
{
	return L_FUNCTION_NOT_SUPPORTED;
}


//----------------- Support Functions -----------------------------
uint32_t LinxDevice::GetMilliSeconds(void)
{
	return getMsTicks();
}

uint32_t LinxDevice::GetSeconds(void)
{
	return getSeconds();
}

void LinxDevice::DelayMs(uint32_t ms)
{
	delayMs(ms);	
}

/****************************************************************************************
**  Public Channel Registry Functions
****************************************************************************************/
int32_t LinxDevice::EnumerateChannels(int32_t type, uint8_t *buffer, uint32_t length, uint32_t *reqLen)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	uint32_t i = 0, off = 0, num = (uint32_t)m.size();
	if (num)
	{
		off = num;
		for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin(); it != m.end(); ++it, i++)
		{
			if (buffer)
			{
				if (i < length)
					buffer[i] = it->first;
				if (it->second && reqLen && off < length + 1)
				{
					buffer[off] = Min(it->second->GetName(buffer + off + 1, length - off - 1), length - off - 1);
					off += buffer[off] + 1;
				}
			}
			else if (it->second)
			{
				off += it->second->GetName(buffer, length) + 1;
			}
		}
	}
	if (reqLen)
		*reqLen = off;
	return num;
}

/****************************************************************************************
**  Protected Channel Registry Functions
****************************************************************************************/
unsigned char LinxDevice::RegisterChannel(int32_t type, LinxChannel *channelObj)
{
	uint8_t channel = 0;
	std::map<uint8_t, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (std::map<uint8_t, LinxChannel*>::iterator it = m.begin(); it != m.end(); it++)
	{
		if (it->first != channel)
			break;

		channel = it->first + 1;
	}
	m.insert(std::pair<uint8_t, LinxChannel*>(channel, channelObj));
	m_ChannelRegistry[type - 1] = m;
	return channel;
}

void LinxDevice::RegisterChannel(int32_t type, uint8_t channel, LinxChannel *channelObj)
{
	std::map<uint8_t, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::pair<std::map<uint8_t, LinxChannel*>::iterator, bool> result = m.insert(std::pair<uint8_t, LinxChannel*>(channel, channelObj));
	if (!result.second)
	{
		result.first->second->Release();
		result.first->second = channelObj;
		result.first->second->AddRef();
	}
	m_ChannelRegistry[type - 1] = m;
}

LinxChannel* LinxDevice::LookupChannel(int32_t type, uint8_t channel)
{
	std::map<uint8_t, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::map<uint8_t, LinxChannel*>::iterator it = m.find(channel);
	if (it != m.end())
	{
		if (it->second)
		{
			it->second->AddRef();
			return it->second;
		}
	}
	return NULL;
}

LinxChannel* LinxDevice::LookupChannel(int32_t type, const unsigned char *channelName, uint8_t *channel)
{
	std::map<uint8_t, LinxChannel*> m = m_ChannelRegistry[type - 1];
	int32_t num = (int32_t)m.size();
	if (num)
	{
		unsigned char buffer[64];
		for (std::map<uint8_t, LinxChannel*>::iterator it = m.begin();  it != m.end(); ++it)
		{
			if (it->second)
			{
				it->second->GetName(buffer, 64);
				if (!strcmp((char*)channelName, (char*)buffer))
				{
					if (channel)
						*channel = it->first;
					it->second->AddRef();
					return it->second;
				}
			}
		}
	}
	return NULL;
}

int32_t LinxDevice::VerifyChannels(int32_t type, uint8_t numChans, uint8_t *channel)
{
	std::map<uint8_t, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (uint8_t i = 0; i < numChans; i++)
	{
		std::map<uint8_t, LinxChannel*>::iterator it = m.find(channel[i]);
		if (it == m.end())
			return LERR_BADCHAN;
	}
	return L_OK;
}

void LinxDevice::RemoveChannel(int32_t type, uint8_t channel)
{
	std::map<uint8_t, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::map<uint8_t, LinxChannel*>::iterator it = m.find(channel);
	if (it != m.end())
	{
		if (it->second)
			it->second->Release();
		m.erase(it);
	}
}

void LinxDevice::ClearChannels(int32_t type)
{
	std::map<uint8_t, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (std::map<uint8_t, LinxChannel*>::iterator it = m.begin(); it != m.end(); it++)
	{
		int32_t count = it->second ? it->second->Release() : 0;
		if (count)
		{
			m_Debug->Write("Channel not released! Bad refcount");
		}
	}
	m.clear();
}
