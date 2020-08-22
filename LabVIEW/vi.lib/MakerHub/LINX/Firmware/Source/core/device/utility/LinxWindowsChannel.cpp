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

#include "LinxDefines.h"
#include "LinxChannel.h"
#include "LinxWindowsChannel.h"
#include "LinxDevice.h"

/****************************************************************************************
**  Constructor/Destructors
****************************************************************************************/
LinxWindowsUartChannel::LinxWindowsUartChannel(LinxFmtChannel *debug, const char *deviceName) : LinxUartChannel(debug, deviceName)
{
	m_Handle = INVALID_HANDLE_VALUE;
}

LinxWindowsUartChannel::~LinxWindowsUartChannel()
{	
	if (m_Handle != INVALID_HANDLE_VALUE)
		CloseHandle(m_Handle);
}

/****************************************************************************************
**  Functions
****************************************************************************************/
int LinxWindowsUartChannel::SmartOpen()
{
	if (m_Handle == INVALID_HANDLE_VALUE)
	{
		m_Handle = CreateFileA(m_ChannelName,                  // Name of the Port to be Opened
		                        GENERIC_READ | GENERIC_WRITE, // Read/Write Access
								0,                            // No Sharing, ports cant be shared
								NULL,                         // No Security
							    OPEN_EXISTING,                // Open existing port only
		                        0,                            // Non Overlapped I/O
		                        NULL);                        // Null for Comm Devicesopen(m_ChannelName, O_RDWR);
		if (m_Handle == INVALID_HANDLE_VALUE)
		{
			m_Debug->Write("UART Fail - Failed To Open UART Handle - ");
			m_Debug->Writeln(m_ChannelName);
			return  LUART_OPEN_FAIL;
		}
	}
	return L_OK;
}

int LinxWindowsUartChannel::SetSpeed(unsigned int speed, unsigned int* actualSpeed)
{
	int status = SmartOpen();
	if (status)
		return status;

	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	status = GetCommState(m_Handle, &dcbSerialParams);      //retreives  the current settings
	if (status == FALSE)
		m_Debug->Writeln("   Error! in GetCommState()");

	dcbSerialParams.BaudRate = speed;      // Setting BaudRate = 9600
	status = SetCommState(m_Handle, &dcbSerialParams);  //Configuring the port according to settings in DCB 
	return status ? L_OK : LERR_IO;
}

int LinxWindowsUartChannel::SetBitSizes(unsigned char dataBits, unsigned char stopBits)
{
	int status = SmartOpen();
	if (status)
		return status;

	DCB dcbSerialParams = { 0 };					// Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	status = GetCommState(m_Handle, &dcbSerialParams);      //retreives  the current settings
	if (status == FALSE)
		m_Debug->Writeln("   Error! in GetCommState()");

	if (dataBits)
		dcbSerialParams.ByteSize = dataBits;	// Setting ByteSize
	if (stopBits == 1)
		dcbSerialParams.StopBits = ONESTOPBIT;	// Setting StopBits
	else if ((stopBits == 1))
		dcbSerialParams.StopBits = TWOSTOPBITS;	// Setting StopBits
	status = SetCommState(m_Handle, &dcbSerialParams);  //Configuring the port according to settings in DCB 
	return status ? L_OK : LERR_IO;
}

#define NUM_PARITY_SIZES	5

int LinxWindowsUartChannel::SetParity(LinxUartParity parity)
{
	int status = SmartOpen();
	if (status)
		return status;

	if (parity > NUM_PARITY_SIZES)
		return LERR_BADPARAM;

	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	status = GetCommState(m_Handle, &dcbSerialParams);      //retreives  the current settings
	if (status == FALSE)
		m_Debug->Writeln("   Error! in GetCommState()");

	if (parity == Ignore)
	{
		dcbSerialParams.Parity = NOPARITY;		// Setting parity = None 
		dcbSerialParams.fParity = TRUE;			// Ignore parity errors
	}
	else
	{
		dcbSerialParams.Parity = parity - 1;	// Setting parity to desired value
		dcbSerialParams.fParity = FALSE;		// Enable parity errors
	}
	status = SetCommState(m_Handle, &dcbSerialParams);  //Configuring the port according to settings in DCB 
	return status ? L_OK : LERR_IO;
}

int LinxWindowsUartChannel::Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead)
{
	int status;
	if (recBuffer && numBytes)
	{
		COMMTIMEOUTS timeouts = { 0 };
		status = GetCommTimeouts(m_Handle, &timeouts);
		if (status)
		{
			timeouts.ReadTotalTimeoutMultiplier = 0;
			if (!timeout)
			{
				// Return immediately even if there is no data
				timeouts.ReadIntervalTimeout = MAXDWORD; 
				timeouts.ReadTotalTimeoutConstant = 0;
			}
			else
			{
				timeouts.ReadIntervalTimeout = 0;
				timeouts.ReadTotalTimeoutConstant = timeout > 0 ? timeout : MAXDWORD;
			}
			status = SetCommTimeouts(m_Handle, &timeouts);
			if (status)
				status = ReadFile(m_Handle, recBuffer, numBytes, (LPDWORD)numBytesRead, NULL);
		}
	}
	else
	{
		COMSTAT stat;
		DWORD error;
		status = ClearCommError(m_Handle, (LPDWORD)&error, &stat);
		if (status)
			*numBytesRead = stat.cbInQue;
	}
	return status ? L_OK : LERR_IO;
}

int LinxWindowsUartChannel::Write(unsigned char* sendBuffer, int numBytes, int timeout)
{
	COMMTIMEOUTS timeouts = { 0 };
	int status = GetCommTimeouts(m_Handle, &timeouts);
	timeouts.ReadTotalTimeoutConstant = timeout >= 0 ? timeout : MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	status = SetCommTimeouts(m_Handle, &timeouts);

	status = WriteFile(m_Handle, sendBuffer, numBytes, (LPDWORD)&numBytes, NULL);
	return status ? L_OK : LERR_IO;
}

int LinxWindowsUartChannel::Close()
{
	if (m_Handle != INVALID_HANDLE_VALUE)
		CloseHandle(m_Handle);
	return L_OK;
}
