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
#include <fcntl.h>
#include <sys/stat.h>

#include "LinxDevice.h"
#include "LinxWindowsDevice.h"

LinxWindowsDevice::LinxWindowsDevice()
{
}

LinxWindowsDevice::~LinxWindowsDevice()
{
}

unsigned char LinxWindowsDevice::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	return 0;
}

int UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud)
{
	return 0;
}
