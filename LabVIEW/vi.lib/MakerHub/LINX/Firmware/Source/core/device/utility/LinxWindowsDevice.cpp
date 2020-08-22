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
