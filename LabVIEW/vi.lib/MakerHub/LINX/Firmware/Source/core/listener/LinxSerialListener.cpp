/****************************************************************************************
**  LINX serial listener code.
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

#include "LinxDevice.h"
#include "utility/LinxListener.h"
#include "LinxSerialListener.h"

/****************************************************************************************
**  Constructors
****************************************************************************************/

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxSerialListener::Start(unsigned char uartChannel, unsigned int baudRate)
{
	LinxUartChannel *uartChan = NULL;
	unsigned int actualBaud = 0;
	int status = m_LinxDev->UartOpen(uartChannel, &uartChan);
	if (!status)
		status = uartChan->SetSpeed(baudRate, &actualBaud);
	if (!status)
		status = LinxListener::Start(uartChan);
	return status;
}

int LinxSerialListener::WaitForConnection()
{
	return L_OK;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
