/****************************************************************************************
**  LINX - Generic LINX device.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stddef.h>
#include "LinxDefines.h"
#include "LinxDevice.h"
#include "LinxUtilities.h"

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxDevice::LinxDevice()
{
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
	m_Debug = new LinxFmtChannel();
	m_Debug->AddRef();
}

LinxDevice::~LinxDevice()
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
int LinxDevice::EnableDebug(LinxCommChannel *channel)
{	
	return m_Debug->SetChannel(channel);
}

void LinxDevice::DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer)
{
	#if DEBUG_ENABLED >= 0
		if(direction == RX)
		{
			DebugPrint("Received :: ");
		}
		else if(direction == TX)
		{
			DebugPrint("Sending  :: ");
		}
		
		for (int i = 0; i<packetBuffer[1]; i++)
		{
			DebugPrint("[");
			DebugPrint(packetBuffer[i], HEX);
			DebugPrint("]");
		}
		DebugPrintln();
		
		if (direction == TX)
		{
			//Add Second New Line After TX
			DebugPrintln();
		}
	#endif
}


//--------------------------------------------------------Analog-------------------------------------------------------
int LinxDevice::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int status = L_OK;
	unsigned char responseByteOffset = 0;
	unsigned char responseBitsRemaining = 8;
	unsigned char dataBitsRemaining;

	values[responseByteOffset] = 0x00;				// Clear next response byte
	for (int i = 0; i < numChans; i++)
	{
		LinxAiChannel *chan = (LinxAiChannel*)LookupChannel(IID_LinxAiChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;

		// Acquire AI Sample
		unsigned int aiVal = 0;
		status = chan->Read(&aiVal);
		if (status)
			return status;

		dataBitsRemaining = AiResolution;
		// Byte packed AI values in response data
		while (dataBitsRemaining > 0)
		{
			*(values + responseByteOffset) |= (unsigned char)((aiVal >> (AiResolution - dataBitsRemaining)) << (8 - responseBitsRemaining));

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
		chan->Release();
	}
	return status;
}

int LinxDevice::AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxAiChannel *chan = (LinxAiChannel*)LookupChannel(IID_LinxAiChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;
		
		status = chan->Read(values + i);
		chan->Release();
	}
	return status;
}

int LinxDevice::AnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int status = L_OK;
	unsigned int aoVal = 0;
	unsigned char sourceByteOffset = 0;
	unsigned char sourceBitsRemaining = 8;
	unsigned char dataBitsRemaining;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxAoChannel *chan = (LinxAoChannel*)LookupChannel(IID_LinxAoChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;

		dataBitsRemaining = AoResolution;
		aoVal = 0;
		while (dataBitsRemaining)
		{
			aoVal |= ((unsigned int)*(values + sourceByteOffset) << (AoResolution - dataBitsRemaining)) >> (8 - sourceBitsRemaining);
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
		status = chan->Write(aoVal);
		chan->Release();
	}
	return status;
}

//--------------------------------------------------------Digital-------------------------------------------------------
int LinxDevice::DigitalSetState(unsigned char numChans, unsigned char* channels, unsigned char* states)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *chan = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;
		
		status = chan->SetState(states[i]);
		chan->Release();
	}
	return status;
}

int LinxDevice::DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *chan = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;
		
		status = chan->Write((values[i / 8] >> (i % 8)) & 0x01);
		chan->Release();
	}
	return status;
}

int LinxDevice::DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *chan = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;
		
		status = chan->Write(values[i]);
		chan->Release();
	}
	return status;
}

int LinxDevice::DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	unsigned char bitOffset = 8;
	unsigned char byteOffset = 0;
	unsigned char retVal = 0;
	unsigned char diVal = 0;
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *chan = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;

		//If bitOffset Is 0 We Have To Start A New Byte, Store Old Byte And Increment OFfsets
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
		status = chan->Read(&diVal);
		if (!status)
			retVal |= (diVal << bitOffset);	//Read Pin And Insert Value Into retVal
		chan->Release();
	}
	//Store Last Byte
	values[byteOffset] = retVal;
	return status;
}

int LinxDevice::DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxDioChannel *chan = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;
		
		status = chan->Read(&values[i]);
		chan->Release();
	}
	return status;
}

// ---------------- PWM Functions ------------------ 
int LinxDevice::PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxPwmChannel *chan = (LinxPwmChannel*)LookupChannel(IID_LinxPwmChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;
		
		status = chan->SetDutyCycle(values[i]);
		chan->Release();
	}
	return status;
}

int LinxDevice::PwmSetFrequency(unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxPwmChannel *chan = (LinxPwmChannel*)LookupChannel(IID_LinxPwmChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;
		
		status = chan->SetFrequency(values[i]);
		chan->Release();
	}
	return status;
}

// ---------------- SPI Functions ------------------ 
int LinxDevice::SpiOpenMaster(unsigned char channel)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		int status = chan->Open();
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;

}

int LinxDevice::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		int status = chan->SetBitOrder(bitOrder);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::SpiSetMode(unsigned char channel, unsigned char mode)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		int status = chan->SetMode(mode);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::SpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		int status = chan->SetSpeed(speed, actualSpeed);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		int status = chan->WriteRead(frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::SpiCloseMaster(unsigned char channel)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		int status = chan->Close();
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

// ---------------- I2C Functions ------------------ 
int LinxDevice::I2cOpenMaster(unsigned char channel)
{
	LinxI2cChannel *chan = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (chan)
	{
		int status = chan->Open();
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::I2cSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	LinxI2cChannel *chan = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (chan)
	{
		int status = chan->SetSpeed(speed, actualSpeed);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	LinxI2cChannel *chan = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (chan)
	{
		int status = chan->Write(slaveAddress, eofConfig, numBytes, sendBuffer);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	LinxI2cChannel *chan = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (chan)
	{
		int status = chan->Read(slaveAddress, eofConfig, numBytes, timeout, recBuffer);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::I2cTransfer(unsigned char channel, unsigned char slaveAddress, int numFrames, int *flags, int *numBytes, unsigned int timeout, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	LinxI2cChannel *chan = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (chan)
	{
		int status = chan->Transfer(slaveAddress, numFrames, flags, numBytes, timeout, sendBuffer, recBuffer);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::I2cClose(unsigned char channel)
{
	LinxI2cChannel *chan = (LinxI2cChannel*)LookupChannel(IID_LinxI2cChannel, channel);
	if (chan)
	{
		int status = chan->Close();
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}



// ---------------- UART Functions ------------------ 
int LinxDevice::UartOpen(unsigned char channel, LinxUartChannel **pChan)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		if (pChan)
			*pChan = chan;
		else
			chan->Release();
		return L_OK;
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartOpen(const char *deviceName, unsigned char *channel, LinxUartChannel **chan)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::UartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		int status = chan->SetSpeed(baudRate, actualBaud);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartSetBitSizes(unsigned char channel, unsigned char dataBits, unsigned char stopBits)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		int status = chan->SetBitSizes(dataBits, stopBits);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartSetParity(unsigned char channel, LinxUartParity parity)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		int status = chan->SetParity(parity);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		int status = chan->GetBytesAvail((int*)numBytes);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, int timeout, unsigned char* numBytesRead)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		int status = chan->Read(recBuffer, numBytes, timeout, (int*)numBytesRead);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer, int timeout)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		int status = chan->Write(sendBuffer, numBytes, timeout);
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartClose(unsigned char channel)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		int status = chan->Close();
		chan->Release();
		return status;
	}
	return LERR_BADPARAM;
}

//------------------------------------- Servo -------------------------------------
int LinxDevice::ServoOpen(unsigned char numChans, unsigned char* channels)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxServoChannel *chan = (LinxServoChannel*)LookupChannel(IID_LinxServoChannel, channels[i]);
		if (!chan)
			return LERR_BADPARAM;
		chan->Release();
	}
	return status;
}

int LinxDevice::ServoSetPulseWidth(unsigned char numChans, unsigned char* channels, unsigned short* pulseWidths)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxServoChannel *chan = (LinxServoChannel*)LookupChannel(IID_LinxServoChannel, channels[i]);
		if (!chan)
			return LERR_BADPARAM;

		status = chan->SetPulseWidth(pulseWidths[i]);
		chan->Release();
	}
	return status;
}

int LinxDevice::ServoClose(unsigned char numChans, unsigned char* channels)
{
	int status = L_OK;
	for (int i = 0; i < numChans; i++)
	{
		LinxServoChannel *chan = (LinxServoChannel*)LookupChannel(IID_LinxServoChannel, channels[i]);
		// don't abort loop on errors on close
		if (chan)
		{
			status = chan->Close();
			chan->Release();
		}
	}
	return status;
}

//----------------- WS2812 Functions -----------------------------
int LinxDevice::Ws2812Open(unsigned short numLeds, unsigned char dataChan)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::Ws2812WriteOnePixel(unsigned short pixelIndex, unsigned char red, unsigned char green, unsigned char blue, unsigned char refresh)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::Ws2812WriteNPixels(unsigned short startPixel, unsigned short numPixels, unsigned char* data, unsigned char refresh)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::Ws2812Refresh()
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::Ws2812Close()
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//----------------- Nonvolatile Functions -----------------------------
void LinxDevice::NonVolatileWrite(int address, unsigned char data)
{

}

unsigned char LinxDevice::NonVolatileRead(int address)
{
	return L_FUNCTION_NOT_SUPPORTED;
}


//----------------- Support Functions -----------------------------
unsigned int LinxDevice::GetMilliSeconds()
{
	return getMilliSeconds();
}

unsigned int LinxDevice::GetSeconds()
{
	return getSeconds();
}

void LinxDevice::DelayMs(unsigned int ms)
{
	delayMs(ms);	
}

bool LinxDevice::ChecksumPassed(unsigned char* buffer, int length)
{
	return ComputeChecksum(buffer, length) == buffer[length];
}

unsigned char LinxDevice::ComputeChecksum(unsigned char* buffer, int length)
{
	unsigned char checksum = 0;
	for (unsigned short i = 0; i < length; i++)
	{
		checksum += buffer[i];
	}
	return checksum;
}
