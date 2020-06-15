/****************************************************************************************
**  LINX Windows host Code
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
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "LinxDevice.h"
#include "LinxWindowsDevice.h"

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxWindowsDevice::LinxWindowsDevice()
{
	if (!QueryPerformanceFrequency(&Frequency))
		Frequency.QuadPart = 0;
}

LinxWindowsDevice::~LinxWindowsDevice()
{
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
unsigned char LinxWindowsDevice::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetAiChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetAoChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetDioChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetQeChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetPwmChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetSpiChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetI2cChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetUartChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetCanChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

unsigned char LinxWindowsDevice::GetServoChans(unsigned char *buffer, unsigned char length)
{
	return 0;
}

int LinxWindowsDevice::UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return UartOpen(channel, baudRate, actualBaud, 8, 1, None);
}

int LinxWindowsDevice::UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud, unsigned char dataBits, unsigned char stopBits, LinxUartParity parity)
{
	return 0;
}

//------------------------------------- General -------------------------------------
unsigned int LinxWindowsDevice::GetMilliSeconds()
{
	if (Frequency.QuadPart)
	{
		LARGE_INTEGER counter;
		if (QueryPerformanceCounter(&counter))
		{
			counter.QuadPart *= 1000;
			counter.QuadPart /= Frequency.QuadPart;
			return (int)counter.LowPart;
		}
	}
	return (int)GetTickCount();
}

unsigned int LinxWindowsDevice::GetSeconds()
{
	if (Frequency.QuadPart)
	{
		LARGE_INTEGER counter;
		if (QueryPerformanceCounter(&counter))
		{
			counter.QuadPart /= Frequency.QuadPart;
			return (int)counter.LowPart;
		}
	}
	return (int)GetTickCount() / 1000;
}

void LinxWindowsDevice::DelayMs(unsigned int ms)
{
	Sleep(ms);
}

void LinxWindowsDevice::NonVolatileWrite(int address, unsigned char data)
{

}

unsigned char LinxWindowsDevice::NonVolatileRead(int address)
{
	return L_FUNCTION_NOT_SUPPORTED;
}
