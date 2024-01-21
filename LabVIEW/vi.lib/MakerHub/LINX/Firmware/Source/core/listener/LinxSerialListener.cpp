/****************************************************************************************
**  LINX serial listener code.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinxDevice.h"
#include "utility/LinxListener.h"
#include "LinxSerialListener.h"

/****************************************************************************************
**  Constructors
****************************************************************************************/

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxSerialListener::Start(unsigned char uartChannel, unsigned int baudRate,
			                  unsigned char dataBits, unsigned char stopBits,
						      LinxUartParity parity, int timeout)
{
	LinxUartChannel *uartChan = NULL;
	int status = m_LinxDev->UartOpen(uartChannel, &uartChan);
	if (!status)
	{
		status = uartChan->SetSpeed(baudRate, NULL);
		if (!status)
			status = uartChan->SetParameters(dataBits, stopBits, parity);
		if (!status)
		{
			status = Run(uartChan);
		}
		uartChan->Release();
	}
	return status;
}

int LinxSerialListener::Start(const unsigned char *deviceName, unsigned int baudRate,
			                  unsigned char dataBits, unsigned char stopBits,
						      LinxUartParity parity, int timeout)
{
	LinxUartChannel *uartChan = NULL;
	int status = m_LinxDev->UartOpen(deviceName,  NULL, &uartChan);
	if (!status)
	{
		status = uartChan->SetSpeed(baudRate, NULL);
		if (!status)
			status = uartChan->SetParameters(dataBits, stopBits, parity);
		if (!status)
		{
			status = Run(uartChan);
		}
		uartChan->Release();
	}
	return status;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/

/****************************************************************************************
**  Private Functions
****************************************************************************************/
