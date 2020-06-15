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
LinxSerialListener::LinxSerialListener()
{
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxSerialListener::Start(LinxDevice* debug, LinxDevice* device, unsigned char uartChan, unsigned int baudRate)
{
	unsigned char buffer[255];

	m_LinxDev = device;
	m_LinxDebug = debug;
	m_UartChannel = uartChan;

	m_LinxDev->GetDeviceName(buffer, 255);
	m_LinxDebug->DebugPrint("Starting Listener on ");
	m_LinxDebug->DebugPrint((char *)buffer);
	m_LinxDebug->DebugPrint(", Serial Port: ");
	m_LinxDebug->DebugPrintln((int)uartChan);

	unsigned int actualBaud = 0;
	int status = m_LinxDev->UartOpen(uartChan, baudRate, &actualBaud);
	if (!status)
		return LinxListener::Start();
	return status;
}

int LinxSerialListener::Start(LinxDevice* device, unsigned char uartChan, unsigned int baudRate)
{
	return Start(device, device, uartChan, baudRate);
}

int LinxSerialListener::WaitForConnection()
{
	return L_OK;
}

int LinxSerialListener::Close()
{
	m_LinxDev->UartClose(m_UartChannel);
	return 0;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int LinxSerialListener::ReadData(unsigned char *buffer, int bytesToRead, int *numBytesRead)
{
	return m_LinxDev->UartRead(m_UartChannel, bytesToRead, buffer, (unsigned char*)numBytesRead);
}

int LinxSerialListener::WriteData(unsigned char *buffer, int bytesToWrite)
{
	return m_LinxDev->UartWrite(m_UartChannel, bytesToWrite, buffer);
}

int LinxSerialListener::FlushData()
{
	unsigned char bytesToRead;
	int status = m_LinxDev->UartGetBytesAvailable(m_UartChannel, &bytesToRead);
	if (!status &&  bytesToRead)
	{
		unsigned char *buffer = (unsigned char *)alloca(bytesToRead);
		status = m_LinxDev->UartRead(m_UartChannel, bytesToRead, buffer, &bytesToRead);
	}
	return status;
}

// Pre Instantiate Object
LinxSerialListener LinxSerialConnection = LinxSerialListener();
