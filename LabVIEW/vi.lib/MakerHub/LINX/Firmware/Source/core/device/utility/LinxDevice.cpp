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
#include <alloca.h>
#include "LinxDevice.h"

static unsigned char m_DeviceName[] = "Unknown Device";

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


//----Peripherals----

//DIO
unsigned char LinxDevice::GetDioChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//AI
unsigned char LinxDevice::GetAiChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//AO
unsigned char LinxDevice::GetAoChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//PWM
unsigned char LinxDevice::GetPwmChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//QE
unsigned char LinxDevice::GetQeChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//UART
unsigned char LinxDevice::GetUartChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//I2C
unsigned char LinxDevice::GetI2cChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//SPI
unsigned char LinxDevice::GetSpiChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//CAN
unsigned char LinxDevice::GetCanChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//Servo
unsigned char LinxDevice::GetServoChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

//Reverse The Order Of Bits In A Byte.  This Is Useful For SPI Hardware That Does Not Support Bit Order
unsigned char LinxDevice::ReverseBits(unsigned char b) 
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void LinxDevice::EnableDebug(unsigned char channel)
{	
	unsigned int actualBaud = 0;
	
	UartOpen(channel, 115200, &actualBaud);
	DebugPrintln("Debugging Enabled");
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
		
		for(int i=0; i<packetBuffer[1]; i++)
		{
			DebugPrint("[");
			DebugPrint(packetBuffer[i], HEX);
			DebugPrint("]");
		}
		DebugPrintln();
		
		if(direction == TX)
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

void LinxDevice::UartWrite(unsigned char channel, unsigned char b)
{
	UartWrite(channel, 1, (unsigned char*)&b);
}

void LinxDevice::UartWrite(unsigned char channel, const char *s)
{
	while (*s)
	{
		UartWrite(channel, *s++);
	}
}

void LinxDevice::UartWrite(unsigned char channel, char c)
{
	UartWrite(channel, (unsigned char) c);
}

void LinxDevice::UartWrite(unsigned char channel, int n)
{
	UartWrite(channel, (long) n);
}

void LinxDevice::UartWrite(unsigned char channel, unsigned int n)
{
	UartWrite(channel, (unsigned long) n);
}

void LinxDevice::UartWrite(unsigned char channel, long n)
{
	if (n < 0) 
	{
		UartWrite(channel, '-');
		n = -n;
	}
	UartWriteNumber(channel, n, 10);
}

void LinxDevice::UartWrite(unsigned char channel, unsigned long n)
{
	UartWriteNumber(channel , n, 10);
}

void LinxDevice::UartWrite(unsigned char channel, long n, int base)
{
	if (base == 0)
	{
		UartWrite(channel, (char) n);
	}
	else if (base == 10)
	{
		UartWrite(channel, n);
	}
	else
	{
		UartWriteNumber(channel , n, base);
	}
}

void LinxDevice::UartWriteln(unsigned char channel)
{
	UartWrite(channel, '\r');
	UartWrite(channel, '\n');  
}

void LinxDevice::UartWriteln(unsigned char channel, char c)
{
	UartWrite(channel, c);
	UartWriteln(channel);  
}

void LinxDevice::UartWriteln(unsigned char channel, const char c[])
{
	UartWrite(channel, c);
	UartWriteln(channel);
}

void LinxDevice::UartWriteln(unsigned char channel, unsigned char b)
{
	UartWrite(channel, b);
	UartWriteln(channel);
}

void LinxDevice::UartWriteln(unsigned char channel, int n)
{
	UartWrite(channel, n);
	UartWriteln(channel);
}

void LinxDevice::UartWriteln(unsigned char channel, long n)
{
	UartWrite(channel, n);
	UartWriteln(channel);  
}

void LinxDevice::UartWriteln(unsigned char channel, unsigned long n)
{
	UartWrite(channel, n);
	UartWriteln(channel);  
}

void LinxDevice::UartWriteln(unsigned char channel, long n, int base)
{
	UartWrite(channel, n, base);
	UartWriteln(channel);
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
	#if DEBUG_ENABLED >= 0
		UartWrite(DEBUG_ENABLED, c);
	#endif
}

void  LinxDevice::DebugPrint(const char s[])
{
	#if DEBUG_ENABLED >= 0
		UartWrite(DEBUG_ENABLED, s);
	#endif
}

void  LinxDevice::DebugPrint(unsigned char c)
{
	#if DEBUG_ENABLED >= 0
		UartWrite(DEBUG_ENABLED, c);
	#endif
}

void  LinxDevice::DebugPrint(int n)
{
	#if DEBUG_ENABLED >= 0
		UartWrite(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrint(unsigned int n)
{
	#if DEBUG_ENABLED >= 0
		UartWrite(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrint(long n)
{
	#if DEBUG_ENABLED >= 0
		UartWrite(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrint(unsigned long n)
{
	#if DEBUG_ENABLED >= 0
		UartWrite(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrint(long n, int base)
{
	#if DEBUG_ENABLED >= 0
		UartWrite(DEBUG_ENABLED, n, base);
	#endif
}

void  LinxDevice::DebugPrintln()
{
	#if DEBUG_ENABLED >= 0
		UartWriteln(DEBUG_ENABLED);
	#endif
}

void  LinxDevice::DebugPrintln(char c)
{
	#if DEBUG_ENABLED >= 0
		UartWriteln(DEBUG_ENABLED, c);
	#endif
}

void  LinxDevice::DebugPrintln(const char s[])
{
	#if DEBUG_ENABLED >= 0
		UartWriteln(DEBUG_ENABLED, s);
	#endif
}

void  LinxDevice::DebugPrintln(unsigned char c)
{
	#if DEBUG_ENABLED >= 0
		UartWriteln(DEBUG_ENABLED, c);
	#endif
}

void  LinxDevice::DebugPrintln(int n)
{	
	#if DEBUG_ENABLED >= 0
		UartWriteln(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrintln(long n)
{
	#if DEBUG_ENABLED >= 0
		UartWriteln(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrintln(unsigned long n)
{
	#if DEBUG_ENABLED >= 0
		UartWriteln(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrintln(long n, int base)
{
	#if DEBUG_ENABLED >= 0
		UartWriteln(DEBUG_ENABLED, n, base);
	#endif
}


/****************************************************************************************
**  Private Functions
****************************************************************************************/

void LinxDevice::UartWriteNumber(unsigned char channel, unsigned long n, unsigned char base)
{
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long i = 0;

	if (n == 0) 
	{
		UartWrite(channel, '0');
		return;
	} 

	while (n > 0) 
	{
		buf[i++] = (unsigned char)(n % base);
		n /= base;
	}

	for (; i > 0; i--)
	{
		UartWrite(channel, (char) (buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
	}
}
