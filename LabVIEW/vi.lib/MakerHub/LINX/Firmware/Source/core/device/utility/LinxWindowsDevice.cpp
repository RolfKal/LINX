/****************************************************************************************
**  LINX Windows host Code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written by Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinxWindowsDevice.h"
#include "LinxWindowsChannel.h"

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxWindowsDevice::LinxWindowsDevice()
{
	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;

	//-------------------------------- Device Detection ------------------------------
	DeviceFamily = 0x04;	// Windows Family Code
#if Win64
	DeviceId = 0x1;		// Windows 64-bit
#elif Win32
	DeviceId = 0x2;		// Windows 32-bit
#endif

	//------------------------------------- AI ---------------------------------------
	
	//------------------------------------- AO ---------------------------------------

	//------------------------------------- DIO --------------------------------------

	//------------------------------------- PWM --------------------------------------

	//------------------------------------- QE ---------------------------------------

	//------------------------------------- UART -------------------------------------
	// Enumerate all currently known Uart channels and store them in the registry map
	char v_UartChan = 0;
	char *v_UartName = NULL;
	for (int i = 0; i < 0; i++)
	{
		RegisterChannel(IID_LinxUartChannel, v_UartChan, (LinxUartChannel*)new LinxWindowsUartChannel(m_Debug, v_UartName));
	}

	//------------------------------------- I2C -------------------------------------

	//------------------------------------- SPI -------------------------------------

	//------------------------------------- CAN -------------------------------------

	//------------------------------------ SERVO ------------------------------------
}

LinxWindowsDevice::~LinxWindowsDevice()
{
}

unsigned char LinxWindowsDevice::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	DWORD len = length;
	if (!GetComputerNameExA(ComputerNamePhysicalDnsFullyQualified, (LPSTR)buffer, &len))
		return L_UNKNOWN_ERROR;
	return L_OK;
}
