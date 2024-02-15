/****************************************************************************************
**  LINX - Generic LINX channel implementation.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "LinxDefines.h"
#include "LinxChannel.h" 
#include "LinxUtilities.h"

/************************************ Linx Channel *************************************/
/****************************************************************************************
**  Constructor/Destructors
****************************************************************************************/
LinxChannel::LinxChannel(const unsigned char *channelName)
{
	m_Debug = new LinxFmtChannel();
	m_ChannelName = NULL;
	size_t len = channelName ? strlen((char*)channelName) : 0;
	if (len)
	{
		m_ChannelName = (char*)malloc(len + 1);
		strcpy(m_ChannelName, (char*)channelName);
	}
}

LinxChannel::LinxChannel(LinxFmtChannel *debug, const unsigned char *channelName)
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
	m_ChannelName = NULL;
	size_t len = channelName ? strlen((char*)channelName) : 0;
	if (len)
	{
		m_ChannelName = (char*)malloc(len + 1);
		strcpy(m_ChannelName, (char*)channelName);
	}
}

LinxChannel::~LinxChannel(void)
{
	free(m_ChannelName);
	m_Debug->Release();
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
uint32_t LinxChannel::GetName(unsigned char* buffer, uint8_t numBytes)
{
	uint32_t len = m_ChannelName ? (uint32_t)strlen(m_ChannelName) : 0;
	if (len && buffer)
	{
		strncpy((char*)buffer, m_ChannelName, numBytes);
	}
	return len;
}

uint32_t LinxChannel::PrintName(void)
{
	if (m_ChannelName)
		return m_Debug->Write(m_ChannelName);
	return 0;
}

int32_t LinxChannel::EnableDebug(LinxCommChannel *channel)
{	
	return m_Debug->SetDebugChannel(channel);
}

int32_t LinxChannel::DisableDebug(void)
{	
	return m_Debug->SetDebugChannel(NULL);
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
/*
uint32_t LinxChannel::SetName(unsigned char* channelName)
{
	size_t len = channelName ? strlen((char*)channelName) : 0;
	if (len)
	{
		m_ChannelName = (char*)realloc(m_ChannelName, len + 1);
		strcpy(m_ChannelName, (char*)channelName);
	}
	return (uint32_t)len;
}
*/
/************************************ Linx Analog Channels *****************************/
/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxAnalogChannel::LinxAnalogChannel(LinxFmtChannel *debug, const unsigned char *channelName, uint8_t resolution) : LinxChannel(debug, channelName)
{
	m_Resolution = resolution;
	m_ResOffset = (int8_t)resolution - 8; 
}

/****************************************************************************************
**  Functions
****************************************************************************************/
int32_t LinxAnalogChannel::SetResolution(uint8_t resolution)
{
	m_Resolution = resolution;
	m_ResOffset = (int8_t)resolution - 8; 
	return L_OK;
}

static const uint8_t lookup[] = { 128, 140, 153, 165, 177, 188, 199, 209, 218, 226, 234, 240, 245, 250, 253, 254, 255, 254, 253, 250, 245, 240, 234, 226, 218, 209, 199, 188, 177, 165, 153, 140, 128, 116, 103, 91, 79, 68, 57, 47,38, 30, 22, 16, 11, 6, 3, 2, 1, 2, 3, 6, 11, 16, 22, 30, 38, 47, 57, 68, 79, 91, 103, 116 };

int32_t LinxAnalogChannel::GetResolution(uint8_t *resolution)
{
	*resolution = m_Resolution;
	return L_OK;
}

int32_t LinxAiChannel::Read(uint32_t *value)
{
	uint32_t tick = getMsTicks();
	uint32_t size = sizeof(lookup);
	uint32_t sinVal = lookup[tick % size];
	*value = (m_ResOffset >= 0) ? sinVal << m_ResOffset : sinVal >> -m_ResOffset;
	return L_OK;
}

int32_t LinxAoChannel::Write(int32_t value)
{
	return L_OK;
}

/************************************ Linx Digital IO Channel **************************/
/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxDioChannel::LinxDioChannel(LinxFmtChannel *debug, uint16_t linxPin, uint16_t gpioPin) : LinxChannel(debug, (unsigned char*)"LinxDioPin")
{
	m_GpioChan = gpioPin;
	m_LinxChan = linxPin;
	m_State = GPIO_DIRDIRTY | GPIO_PULLDIRTY;
	m_Value = 0;
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int32_t LinxDioChannel::SetState(uint8_t state)
{
	int32_t status = setDirection(state);
	if (!status || status == LDIGITAL_PIN_NOCHANGE)
	{
		status = setPull(state);
		if (status == LDIGITAL_PIN_NOCHANGE)
			return L_OK;
	}
	return status;
}

int32_t LinxDioChannel::Read(uint8_t *value)
{
	int32_t status = setDirection(GPIO_INPUT);
	if (!status || status == LDIGITAL_PIN_NOCHANGE)
	{
		return getValue(value);
	}
	return status;
}

int32_t LinxDioChannel::Write(uint8_t value)
{
	int32_t status = setDirection(GPIO_OUTPUT);
	if (!status || status == LDIGITAL_PIN_NOCHANGE)
	{
		return setValue(value);
	}
	return status;
}

int32_t LinxDioChannel::WriteSquareWave(uint32_t freq, uint32_t duration)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int32_t LinxDioChannel::ReadPulseWidth(uint8_t stimType, uint8_t respChan, uint8_t respType, uint32_t timeout, uint32_t* width)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int32_t LinxDioChannel::setDirection(uint8_t state)	// direction
{
	char direction = state & GPIO_DIRMASK;
	if ((m_State & (GPIO_DIRMASK | GPIO_DIRDIRTY)) != direction)
	{
		m_State = (m_State & ~GPIO_DIRMASK) | direction;
		return L_OK;
	}
	return LDIGITAL_PIN_NOCHANGE;
}

int32_t LinxDioChannel::setPull(uint8_t state)		// pull-up/down
{
	char pud = state & GPIO_PULLMASK;
	if ((m_State & (GPIO_PULLDIRTY | GPIO_PULLMASK)) != pud)
	{
		m_State = (m_State & ~GPIO_PULLMASK) | pud;
		return L_OK;
	}
	return LDIGITAL_PIN_NOCHANGE;
}

int32_t LinxDioChannel::setValue(uint8_t value)
{
	m_Value = value;
	return L_OK;
}

int32_t LinxDioChannel::getValue(uint8_t *value)
{
	*value = (++m_Value & GPIO_DIRMASK); 
	return L_OK;
}

/************************************ Linx Communication Channel ***********************/
/****************************************************************************************
**  Public Functions
****************************************************************************************/
int32_t LinxCommChannel::Read(unsigned char* recBuffer, uint32_t numBytes, int32_t timeout, uint32_t* numBytesRead)
{
	return Read(recBuffer, numBytes, timeout > 0 ? getMsTicks() : 0, timeout, numBytesRead);
}

int32_t LinxCommChannel::Write(const unsigned char* sendBuffer, uint32_t numBytes, int32_t timeout)
{
	return Write(sendBuffer, numBytes, timeout > 0 ? getMsTicks() : 0, timeout);
}

/************************************ Linx Format Channel ******************************/
/****************************************************************************************
**  Constructor/Destructors
****************************************************************************************/
LinxFmtChannel::LinxFmtChannel(int32_t timeout)
{
	m_Timeout = timeout;
	m_Channel = NULL;
}

LinxFmtChannel::LinxFmtChannel(LinxCommChannel *channel, int32_t timeout)
{
	m_Timeout = timeout;
	if (channel)
		channel->AddRef();
	m_Channel = channel;
}

LinxFmtChannel::~LinxFmtChannel(void)
{
	if (m_Channel)
	{
		m_Channel->Close();
		m_Channel->Release();
	}
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int32_t LinxFmtChannel::Write(const char s[], int32_t len)
{
	if (m_Channel)
		return m_Channel->Write((const unsigned char*)s, len, m_Timeout);
	return L_OK;
}

int32_t LinxFmtChannel::Write(const char s[])
{
	return Write(s, (int32_t)strlen(s));
}

int32_t LinxFmtChannel::Write(int8_t c)
{
	if (m_Channel)
		return m_Channel->Write((const unsigned char*)&c, 1, m_Timeout);
	return L_OK;
}

int32_t LinxFmtChannel::Write(uint8_t c)
{
	if (m_Channel)
		return m_Channel->Write((const unsigned char*)&c, 1, m_Timeout);
	return L_OK;
}

int32_t LinxFmtChannel::Write(int32_t n)
{
	return Write((int64_t)n);
}

int32_t LinxFmtChannel::Write(uint32_t n)
{
	return WriteNumber((uint64_t)n, 10);
}

int32_t LinxFmtChannel::Write(int64_t n)
{
	int32_t status = L_OK;
	if (n < 0) 
	{
		status = Write('-');
		n = -n;
	}
	if (!status)
		status = WriteNumber(n, 10);
	return status;
}

int32_t LinxFmtChannel::Write(uint64_t n)
{
	return  WriteNumber(n, 10);
}

int32_t LinxFmtChannel::Write(int64_t n, int8_t base)
{
	if (base == 0)
	{
		return Write((char)n);
	}
	else if (base == 10)
	{
		return Write(n);
	}
	int32_t status = L_OK;
	if (n < 0) 
	{
		status = Write('-');
		n = -n;
	}
	if (!status)
		return WriteNumber(n, base);
	return status;
}

int32_t LinxFmtChannel::Writeln(void)
{
	if (m_Channel)
		return m_Channel->Write((const unsigned char*)"\r\n", 2, m_Timeout);
	return L_OK;
}

int32_t LinxFmtChannel::Writeln(const char s[])
{
	int32_t status = Write(s);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Writeln(const char s[], int32_t len)
{
	int32_t status = Write(s, len);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Writeln(int8_t c)
{
	int32_t status = Write(c);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Writeln(uint8_t c)
{
	int32_t status = Write(c);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Writeln(int32_t n)
{
	int32_t status = Write(n);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Writeln(uint32_t n)
{
	int32_t status = Write(n);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Writeln(int64_t n)
{
	int32_t status = Write(n);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Writeln(uint64_t n)
{
	int32_t status = Write(n);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Writeln(int64_t n, int8_t base)
{
	int32_t status = Write(n, base);
	if (!status)
		status = Writeln();
	return status;
}

int32_t LinxFmtChannel::Close(void)
{
	if (m_Channel)
		return m_Channel->Close();
	return L_OK;
}

int32_t LinxFmtChannel::SetTimeout(int32_t timeout)
{
	m_Timeout = timeout;
	return L_OK;
}

int32_t LinxFmtChannel::SetDebugChannel(LinxCommChannel *channel)
{
	if (m_Channel)
		m_Channel->Release();
	if (channel)
		channel->AddRef();
	m_Channel = channel;
	return L_OK;
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
int32_t LinxFmtChannel::WriteNumber(uint64_t n, uint8_t base)
{
	uint32_t i, e = 8 * sizeof(uint64_t);
	unsigned char *buf = (unsigned char*)alloca(e); // Assumes 8-bit chars. 

	if (n == 0) 
	{
		return Write('0');
	} 

	i = e;
	while (n > 0) 
	{
		uint8_t tmp = (uint8_t)(n % base);
		buf[--i] = tmp < 10 ? '0' + tmp : 'A' + tmp - 10;
		n /= base;
	}
	return Write((const char*)buf + i, e - i);
}
