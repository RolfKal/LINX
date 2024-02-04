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
int32_t LinxSerialListener::Start(uint8_t uartChannel, uint32_t baudRate,
			                      uint8_t dataBits, uint8_t stopBits,
						          LinxUartParity parity, int32_t timeout)
{
	LinxUartChannel *channelObj;
	int32_t status = m_LinxDev->UartOpen(uartChannel, &channelObj);
	if (!status)
	{
		status = channelObj->SetSpeed(baudRate, NULL);
		if (!status)
			status = channelObj->SetParameters(dataBits, stopBits, parity);
		if (!status)
		{
			status = Run(channelObj, timeout);
		}
	}
	return status;
}

int32_t LinxSerialListener::Start(const unsigned char *deviceName, uint32_t baudRate,
			                      uint8_t dataBits, uint8_t stopBits,
						          LinxUartParity parity, int32_t timeout)
{
	LinxUartChannel *channelObj;
	uint8_t uartChannel;
	int32_t status = m_LinxDev->UartOpen(deviceName, &uartChannel, &channelObj);
	if (!status)
	{
		status = channelObj->SetSpeed(baudRate, NULL);
		if (!status)
			status = channelObj->SetParameters(dataBits, stopBits, parity);
		if (!status)
		{
			status = Run(channelObj, timeout);
		}
	}
	return status;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/

/****************************************************************************************
**  Private Functions
****************************************************************************************/
