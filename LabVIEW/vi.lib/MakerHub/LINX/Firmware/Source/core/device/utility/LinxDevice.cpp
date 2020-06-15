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
#include <stdio.h>
#include <string.h>
#ifndef _MSC_VER
#include <alloca.h>
#else
#include <malloc.h>
#endif
#include "LinxDevice.h"

static unsigned char m_DeviceName[] = "Common Device Parent";

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

	//DIO

	//AI
	AiResolution = 0;
	AiRefDefault = 0;
	AiRefSet = 0;

	//AO
	AoResolution = 0;
	AoRefDefault = 0;
	AoRefSet = 0;

	//PWM

	//QE

	//UART
	UartMaxBaud = 0;

	//I2C

	//SPI

	//CAN

	//Servo

	//Debug
	m_DebugChannnel = -1;
}

LinxDevice::~LinxDevice()
{

}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
unsigned char LinxDevice::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	unsigned char slen = (unsigned char)strlen((const char*)m_DeviceName);
	if (buffer)
	{
		if (length < slen)
			slen = length - 1;

		memcpy(buffer, m_DeviceName, slen);
		if (length > slen)
			buffer[slen] = 0;
	}
	return 12;
}

int LinxDevice::EnableDebug(unsigned char channel)
{	
	return EnableDebug(channel, 115200); 
}

int LinxDevice::EnableDebug(unsigned char channel, unsigned int baudRate)
{	
	unsigned int actualBaud = 0;
	int status = UartOpen(channel, baudRate, &actualBaud);
	if (!status)
	{
		m_DebugChannnel = channel;
		DebugPrint("Debugging Enabled on port ");
		DebugPrintln((int)channel);
	}
	return status;
}

void LinxDevice::DelayMs(unsigned int ms)
{

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
int LinxDevice::AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}


//--------------------------------------------------------Digital-------------------------------------------------------
int LinxDevice::DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Bit Packed Data Array
	int numBytes = ((numChans + 7) >> 3);
	unsigned char *packValues = (unsigned char *)alloca(numBytes);
	memset(packValues, 0, numBytes);
	for (int i = 0; i < numChans; i++)
	{
		packValues[i / 8] |= ((values[i] & 0x01) << (i % 8));
	}
	return DigitalWrite(numChans, channels, packValues);
}

int LinxDevice::DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Bit Packed Data Array
	int numBytes = ((numChans + 7) >> 3);
	unsigned char *packValues = (unsigned char *)alloca(numBytes);
	memset(packValues, 0, numBytes);
	int ret = DigitalRead(numChans, channels, packValues);
	if (!ret)
	{
		for (int i = 0; i < numChans; i++)
		{
			values[i] = (packValues[i / 8] >> (i % 8)) && 0x01;
		}
	}
	return ret;
}

// ---------------- PWM Functions ------------------ 
int LinxDevice::PwmSetFrequency(unsigned char numChans, unsigned char* channels, unsigned int* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

// ---------------- SPI Functions ------------------ 
int LinxDevice::SpiCloseMaster(unsigned char channel)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

// ---------------- UART Functions ------------------ 

int LinxDevice::UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::UartWrite(unsigned char channel, unsigned char b)
{
	return UartWrite(channel, 1, &b);
}

int LinxDevice::UartWrite(unsigned char channel, const char *s)
{
	int status = L_OK;
	while (!status && *s)
	{
		status = UartWrite(channel, 1, (unsigned char*)s++);
	}
	return status;
}
 
int LinxDevice::UartWrite(unsigned char channel, char c)
{
	return UartWrite(channel, 1, (unsigned char*)&c);
}

int LinxDevice::UartWrite(unsigned char channel, int n)
{
	return UartWrite(channel, (long)n);
}

int LinxDevice::UartWrite(unsigned char channel, unsigned int n)
{
	return UartWrite(channel, (unsigned long)n);
}

int LinxDevice::UartWrite(unsigned char channel, long n)
{
	int status;
	if (n < 0) 
	{
		status = UartWrite(channel, '-');
		n = -n;
	}
	if (!status)
		status = UartWriteNumber(channel, n, 10);
	return status;
}

int LinxDevice::UartWrite(unsigned char channel, unsigned long n)
{
	return UartWriteNumber(channel, n, 10);
}

int LinxDevice::UartWrite(unsigned char channel, long n, int base)
{
	if (base == 0)
	{
		return UartWrite(channel, (char)n);
	}
	else if (base == 10)
	{
		return UartWrite(channel, n);
	}
	return UartWriteNumber(channel, n, base);
}

int LinxDevice::UartWriteln(unsigned char channel)
{
	UartWrite(channel, '\r');
	return UartWrite(channel, '\n');  
}

int LinxDevice::UartWriteln(unsigned char channel, char c)
{
	UartWrite(channel, c);
	return UartWriteln(channel);  
}

int LinxDevice::UartWriteln(unsigned char channel, const char c[])
{
	UartWrite(channel, c);
	return UartWriteln(channel);
}

int LinxDevice::UartWriteln(unsigned char channel, unsigned char b)
{
	UartWrite(channel, b);
	return UartWriteln(channel);
}

int LinxDevice::UartWriteln(unsigned char channel, int n)
{
	UartWrite(channel, n);
	return UartWriteln(channel);
}

int LinxDevice::UartWriteln(unsigned char channel, long n)
{
	UartWrite(channel, n);
	return UartWriteln(channel);  
}

int LinxDevice::UartWriteln(unsigned char channel, unsigned long n)
{
	UartWrite(channel, n);
	return UartWriteln(channel);  
}

int LinxDevice::UartWriteln(unsigned char channel, long n, int base)
{
	UartWrite(channel, n, base);
	return UartWriteln(channel);
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

//----------------- DEBUG Functions -----------------------------
void  LinxDevice::DebugPrint(char c)
{
	if (m_DebugChannnel >= 0)
		UartWrite(m_DebugChannnel, c);
}

void  LinxDevice::DebugPrint(const char s[])
{
	if (m_DebugChannnel >= 0)
		UartWrite(m_DebugChannnel, s);
}

void  LinxDevice::DebugPrint(unsigned char c)
{
	if (m_DebugChannnel >= 0)
		UartWrite(m_DebugChannnel, c);
}

void  LinxDevice::DebugPrint(int n)
{
	if (m_DebugChannnel >= 0)
		UartWrite(m_DebugChannnel, n);
}

void  LinxDevice::DebugPrint(unsigned int n)
{
	if (m_DebugChannnel >= 0)
		UartWrite(m_DebugChannnel, n);
}

void  LinxDevice::DebugPrint(long n)
{
	if (m_DebugChannnel >= 0)
		UartWrite(m_DebugChannnel, n);
}

void  LinxDevice::DebugPrint(unsigned long n)
{
	if (m_DebugChannnel >= 0)
		UartWrite(m_DebugChannnel, n);
}

void  LinxDevice::DebugPrint(long n, int base)
{
	if (m_DebugChannnel >= 0)
		UartWrite(m_DebugChannnel, n, base);
}

void  LinxDevice::DebugPrintln()
{
	if (m_DebugChannnel >= 0)
		UartWriteln(m_DebugChannnel);
}

void  LinxDevice::DebugPrintln(char c)
{
	if (m_DebugChannnel >= 0)
		UartWriteln(m_DebugChannnel, c);
}

void  LinxDevice::DebugPrintln(const char s[])
{
	if (m_DebugChannnel >= 0)
		UartWriteln(m_DebugChannnel, s);
}

void  LinxDevice::DebugPrintln(unsigned char c)
{
	if (m_DebugChannnel >= 0)
		UartWriteln(m_DebugChannnel, c);
}

void  LinxDevice::DebugPrintln(int n)
{	
	if (m_DebugChannnel >= 0)
		UartWriteln(m_DebugChannnel, n);
}

void  LinxDevice::DebugPrintln(long n)
{
	if (m_DebugChannnel >= 0)
		UartWriteln(m_DebugChannnel, n);
}

void  LinxDevice::DebugPrintln(unsigned long n)
{
	if (m_DebugChannnel >= 0)
		UartWriteln(m_DebugChannnel, n);
}

void  LinxDevice::DebugPrintln(long n, int base)
{
	if (m_DebugChannnel >= 0)
		UartWriteln(m_DebugChannnel, n, base);
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
**  Protected Functions
****************************************************************************************/

/****************************************************************************************
**  Private Functions
****************************************************************************************/
int LinxDevice::UartWriteNumber(unsigned char channel, unsigned long n, unsigned char base)
{
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long i = 0;

	if (n == 0) 
	{
		return UartWrite(channel, '0');
	} 

	while (n > 0) 
	{
		buf[i++] = (unsigned char)(n % base);
		n /= base;
	}

	int status = L_OK;
	for (; i > 0 && !status; i--)
	{
		status = UartWrite(channel, (char) (buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
	}
	return status;
}
