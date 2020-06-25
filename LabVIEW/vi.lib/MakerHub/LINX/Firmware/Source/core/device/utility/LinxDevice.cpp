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
#include "LinxDefines.h"
#include <stdio.h>
#include <string.h>
#if Win32
#include <malloc.h>
#include <windows.h>
#else
#include <alloca.h>
#endif
#include "LinxDevice.h"
#include "LinxUtilities.h"

LinxChannel::LinxChannel(const char *channelName, LinxFmtChannel *debug)
{
	m_Debug = debug;
	if (!m_Debug)
		m_Debug = new LinxFmtChannel();
	else
		m_Debug->AddRef();
	m_ChannelName = channelName;
	// Start with refcount 1
	m_Refcount = 1;
}

LinxChannel::~LinxChannel()
{
	m_Debug->Release();
}

unsigned int LinxChannel::AddRef()
{
#if Win32
	return InterlockedIncrement(&m_Refcount);
#elif Unix
	return __sync_fetch_and_add(&m_Refcount, 1);
#endif
}

unsigned int LinxChannel::Release()
{
	unsigned int refcount = 
#if Win32
	InterlockedDecrement(&m_Refcount);
#elif Unix
	__sync_sub_and_fetch(&m_Refcount, 1);
#endif
	if (!refcount)
		delete this;
	return refcount;
}

LinxChannel* LinxChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxChannel)
	{
		AddRef();
		return this;
	}
	return LinxChannel::QueryInterface(interfaceId);
}

LinxChannel *LinxDioChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxDioChannel)
	{
		AddRef();
		return this;
	}
	return LinxChannel::QueryInterface(interfaceId);
}

LinxChannel *LinxI2cChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxI2cChannel)
	{
		AddRef();
		return this;
	}
	return LinxChannel::QueryInterface(interfaceId);
}

LinxChannel *LinxSpiChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxSpiChannel)
	{
		AddRef();
		return this;
	}
	return LinxChannel::QueryInterface(interfaceId);
}

LinxChannel *LinxCommChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxCommChannel)
	{
		AddRef();
		return this;
	}
	return LinxChannel::QueryInterface(interfaceId);
}

LinxChannel *LinxUartChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxUartChannel)
	{
		AddRef();
		return this;
	}
	return LinxCommChannel::QueryInterface(interfaceId);
}

LinxFmtChannel::LinxFmtChannel() : LinxCommChannel("FormatChannel", NULL)
{
	m_Channel = NULL;
}

LinxFmtChannel::LinxFmtChannel(LinxCommChannel *channel) : LinxCommChannel("FormatChannel", NULL)
{
	if (channel)
		channel->AddRef();
	m_Channel = channel;
}

LinxFmtChannel::~LinxFmtChannel()
{
	if (m_Channel)
	{
		m_Channel->Close();
		m_Channel->Release();
	}
}

LinxChannel *LinxFmtChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxFmtChannel)
	{
		AddRef();
		return this;
	}
	return LinxCommChannel::QueryInterface(interfaceId);
}

int LinxFmtChannel::Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead)
{
	if (m_Channel)
		return m_Channel->Read(recBuffer, numBytes, timeout, numBytesRead);
	return L_OK;
}

int LinxFmtChannel::Write(unsigned char* sendBuffer, int numBytes, int timeout)
{
	if (m_Channel)
		return m_Channel->Write(sendBuffer, numBytes, timeout);
	return L_OK;
}

int LinxFmtChannel::Write(char c)
{
	if (m_Channel)
		return m_Channel->Write((unsigned char*)&c, 1, TIMEOUT_INFINITE);
	return L_OK;
}

int LinxFmtChannel::Write(const char s[])
{
	if (m_Channel)
		return m_Channel->Write((unsigned char*)s, (int)strlen(s), TIMEOUT_INFINITE);
	return L_OK;
}

int LinxFmtChannel::Write(unsigned char c)
{
	if (m_Channel)
		return m_Channel->Write(&c, 1, TIMEOUT_INFINITE);
	return L_OK;
}

int LinxFmtChannel::Write(int n)
{
	return Write((long)n);
}

int LinxFmtChannel::Write(unsigned int n)
{
	return Write((unsigned long)n);
}

int LinxFmtChannel::Write(long n)
{
	int status = L_OK;
	if (n < 0) 
	{
		status = Write((unsigned char*)"-", 1, TIMEOUT_INFINITE);
		n = -n;
	}
	if (!status)
		status = WriteNumber(n, 10);
	return status;
}

int LinxFmtChannel::Write(unsigned long n)
{
	return  WriteNumber(n, 10);
}

int LinxFmtChannel::Write(long n, int base)
{
	if (base == 0)
	{
		return Write((unsigned char*)&n, 1, TIMEOUT_INFINITE);
	}
	else if (base == 10)
	{
		return Write(n);
	}
	int status = L_OK;
	if (n < 0) 
	{
		status = Write((unsigned char*)"-", 1, TIMEOUT_INFINITE);
		n = -n;
	}
	if (!status)
		return WriteNumber(n, base);
	return status;
}

int LinxFmtChannel::Writeln()
{
	if (m_Channel)
		return m_Channel->Write((unsigned char*)"\r\n", 2, TIMEOUT_INFINITE);
	return L_OK;
}

int LinxFmtChannel::Writeln(char c)
{
	if (m_Channel)
	{
		int status = m_Channel->Write((unsigned char*)&c, 1, TIMEOUT_INFINITE);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(const char s[])
{
	if (m_Channel)
	{
		int status = m_Channel->Write((unsigned char*)s, (int)strlen(s), TIMEOUT_INFINITE);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(unsigned char c)
{
	if (m_Channel)
	{
		int status = m_Channel->Write(&c, 1, TIMEOUT_INFINITE);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(int n)
{
	if (m_Channel)
	{
		int status = Write((long)n);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(long n)
{
	if (m_Channel)
	{
		int status = Write(n);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(unsigned long n)
{
	if (m_Channel)
	{
		int status = Write(n);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(long n, int base)
{
	if (m_Channel)
	{
		int status = Write(n, base);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Close()
{
	if (m_Channel)
		return m_Channel->Close();
	return L_OK;
}

int LinxFmtChannel::WriteNumber(unsigned long n, unsigned char base)
{
	unsigned int i, e = 8 * sizeof(long);
	unsigned char *buf = (unsigned char*)alloca(e); // Assumes 8-bit chars. 

	if (n == 0) 
	{
		return m_Channel->Write((unsigned char*)"0", 1, TIMEOUT_INFINITE);
	} 

	i = e;
	while (n > 0) 
	{
		unsigned char tmp = (unsigned char)(n % base);
		buf[--i] = tmp < 10 ? '0' + tmp : 'A' + tmp - 10;
		n /= base;
	}
	return m_Channel->Write(buf + i, e - i, TIMEOUT_INFINITE);
}

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
}

LinxDevice::~LinxDevice()
{
	if (m_Debug)
		m_Debug->Release();

	// Close Uart channels
	ClearChannels(IID_LinxUartChannel);

	// Close I2C channels
	ClearChannels(IID_LinxI2cChannel);

	// Close Spi channels
	ClearChannels(IID_LinxSpiChannel);

}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxDevice::EnableDebug(LinxCommChannel *channel)
{	
	if (m_Debug)
		m_Debug->Release();
	m_Debug = new LinxFmtChannel(channel);
	if (m_Debug)
		return m_Debug->AddRef();
	return LERR_MEMORY;
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
	unsigned char dataBitsRemaining = AiResolution;
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
			*(values + responseByteOffset) |= ( (aiVal >> (AiResolution - dataBitsRemaining)) << (8 - responseBitsRemaining));

			if (responseBitsRemaining > dataBitsRemaining)
			{
				// Current Byte Still Has Empty Bits
				responseBitsRemaining -= dataBitsRemaining;
				dataBitsRemaining = 0;
			}
			else
			{
				// Current Byte Full
				dataBitsRemaining = dataBitsRemaining - responseBitsRemaining;
				responseByteOffset++;
				responseBitsRemaining = 8;
				values[responseByteOffset] = 0x00;    //Clear Next Response Byte
			}
		}
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
	}
	return status;
}

int LinxDevice::AnalogWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	int status = L_OK;
	for (int i = 0; !status && i < numChans; i++)
	{
		LinxAoChannel *chan = (LinxAoChannel*)LookupChannel(IID_LinxAoChannel, channels[i]);
		if (!chan)
			return LERR_BADCHAN;
		
		status = chan->Write(values[i]);
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
	for (int i = 0; i < numChans; i++)
	{
		LinxDioChannel *chan = (LinxDioChannel*)LookupChannel(IID_LinxDioChannel, channels[i]);
		if (chan)
		{
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
			if (status)
				return status;

			retVal |= (diVal << bitOffset);	//Read Pin And Insert Value Into retVal
		}
		else
		{
			return LERR_BADCHAN;
		}
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
	}
	return status;
}

// ---------------- SPI Functions ------------------ 
int LinxDevice::SpiOpenMaster(unsigned char channel)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		return chan->Open();
	}
	return LERR_BADPARAM;

}

int LinxDevice::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		return chan->SetBitOrder(bitOrder);
	}
	return LERR_BADPARAM;
}

int LinxDevice::SpiSetMode(unsigned char channel, unsigned char mode)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		return chan->SetMode(mode);
	}
	return LERR_BADPARAM;
}

int LinxDevice::SpiSetSpeed(unsigned char channel, unsigned int speed, unsigned int* actualSpeed)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		return chan->SetSpeed(speed, actualSpeed);
	}
	return LERR_BADPARAM;
}

int LinxDevice::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		return chan->WriteRead(frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
	}
	return LERR_BADPARAM;
}

int LinxDevice::SpiCloseMaster(unsigned char channel)
{
	LinxSpiChannel *chan = (LinxSpiChannel*)LookupChannel(IID_LinxSpiChannel, channel);
	if (chan)
	{
		return chan->Close();
	}
	return LERR_BADPARAM;
}

// ---------------- UART Functions ------------------ 
int LinxDevice::UartOpen(unsigned char channel, LinxUartChannel **chan)
{
	LinxUartChannel *temp = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (temp && chan)
	{
		*chan = temp;
		return L_OK;
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartOpen(const char *deviceName, unsigned char nameLength, unsigned char *channel, LinxUartChannel **chan)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::UartSetBaudRate(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		return chan->SetSpeed(baudRate, actualBaud);
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartSetBitSizes(unsigned char channel, unsigned char dataBits, unsigned char stopBits)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		return chan->SetBitSizes(dataBits, stopBits);
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartSetParity(unsigned char channel, LinxUartParity parity)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		return chan->SetParity(parity);
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		return chan->Read(NULL, 0, 0, (int*)numBytes);
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, int timeout, unsigned char* numBytesRead)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		return chan->Read(recBuffer, numBytes, timeout, (int*)numBytesRead);
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer, int timeout)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		return chan->Write(sendBuffer, numBytes, timeout);
	}
	return LERR_BADPARAM;
}

int LinxDevice::UartClose(unsigned char channel)
{
	LinxUartChannel *chan = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channel);
	if (chan)
	{
		return chan->Close();
	}
	return LERR_BADPARAM;
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

/****************************************************************************************
**  Public Channel Registry Functions
****************************************************************************************/
unsigned char LinxDevice::EnumerateChannels(int type, unsigned char *buffer, unsigned char length)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

/****************************************************************************************
**  Protected Channel Registry Functions
****************************************************************************************/
unsigned char LinxDevice::RegisterChannel(int type, LinxChannel *chan)
{
	unsigned char channel = 0;
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin(); it != m.end(); it++)
	{
		if (it->first == channel)
		{
			channel = it->first + 1;
		}
		else
		{
			break;
		}
	}
	m.insert(std::pair<unsigned char, LinxChannel*>(channel, chan));
	return channel;
}

void LinxDevice::RegisterChannel(int type, unsigned char channel, LinxChannel *chan)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::pair<std::map<unsigned char, LinxChannel*>::iterator, bool> result = m.insert(std::pair<unsigned char, LinxChannel*>(channel, chan));
	if (!result.second)
	{
		result.first->second->Release();
		result.first->second = chan;
		result.first->second->AddRef();
	}
}

LinxChannel* LinxDevice::LookupChannel(int type, unsigned char channel)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::map<unsigned char, LinxChannel*>::iterator it = m.find(channel);
	return it != m.end() ? it->second->QueryInterface(type) : NULL;
}

void LinxDevice::RemoveChannel(int type, unsigned char channel)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::map<unsigned char, LinxChannel*>::iterator it = m.find(channel);
	if (it != m.end())
	{
		it->second->Release();
		m.erase(it);
	}
}

void LinxDevice::ClearChannels(int type)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin(); it != m.end(); it++)
	{
		int count = it->second->Release();
		if (count)
		{
			m_Debug->Write("Channel not released! Bad refcount");
		}
	}
	m.clear();
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
