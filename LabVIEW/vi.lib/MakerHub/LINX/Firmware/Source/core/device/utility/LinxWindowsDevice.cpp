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

LinxWinUartChannel::LinxWinUartChannel(const char *channelName, LinxFmtChannel *debug) : LinxUartChannel(channelName, debug)
{
	m_Handle = INVALID_HANDLE_VALUE;
}

LinxWinUartChannel::~LinxWinUartChannel()
{
	if (m_Handle != INVALID_HANDLE_VALUE)
		CloseHandle(m_Handle);
}

/****************************************************************************************
**  Functions
****************************************************************************************/
LinxChannel *LinxWinUartChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxWinUartChannel)
	{
		AddRef();
		return this;
	}
	return LinxUartChannel::QueryInterface(interfaceId);
}

int LinxWinUartChannel::SmartOpen()
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

int LinxWinUartChannel::SetSpeed(unsigned int speed, unsigned int* actualSpeed)
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

int LinxWinUartChannel::SetBitSizes(unsigned char dataBits, unsigned char stopBits)
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

int LinxWinUartChannel::SetParity(LinxUartParity parity)
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

int LinxWinUartChannel::GetBytesAvail(int* numBytesAvailable)
{
	COMSTAT stat;
	DWORD error;
	int status = ClearCommError(m_Handle, (LPDWORD)&error, &stat);
	if (status)
		*numBytesAvailable = stat.cbInQue;
	return status ? L_OK : LERR_IO;
}

int LinxWinUartChannel::Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead)
{
	if (recBuffer && numBytes)
	{
		COMMTIMEOUTS timeouts = { 0 };
		int status = GetCommTimeouts(m_Handle, &timeouts);
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
		return status ? L_OK : LERR_IO;
	}
	return GetBytesAvail(numBytesRead);
}

int LinxWinUartChannel::Write(unsigned char* sendBuffer, int numBytes, int timeout)
{
	COMMTIMEOUTS timeouts = { 0 };
	int status = GetCommTimeouts(m_Handle, &timeouts);
	timeouts.ReadTotalTimeoutConstant = timeout >= 0 ? timeout : MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	status = SetCommTimeouts(m_Handle, &timeouts);

	status = WriteFile(m_Handle, sendBuffer, numBytes, (LPDWORD)&numBytes, NULL);
	return status ? L_OK : LERR_IO;
}

int LinxWinUartChannel::Close()
{
	if (m_Handle != INVALID_HANDLE_VALUE)
		CloseHandle(m_Handle);
	return L_OK;
}

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxWindowsDevice::LinxWindowsDevice()
{
}

LinxWindowsDevice::~LinxWindowsDevice()
{
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxWindowsDevice::UartOpen(const char *deviceName, unsigned char *channel, LinxUartChannel **chan)
{
	return 0;
}


/****************************************************************************************
**  Public Channel Registry Functions
****************************************************************************************/
unsigned char LinxWindowsDevice::EnumerateChannels(int type, unsigned char *buffer, unsigned char length)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

/****************************************************************************************
**  Protected Channel Registry Functions
****************************************************************************************/
unsigned char LinxWindowsDevice::RegisterChannel(int type, LinxChannel *chan)
{
	unsigned char channel = 0;
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin(); it != m.end(); it++)
	{
		if (it->first == channel)
		{
			channel = it->first + 1;
		}
		else
		{
			break;
		}
	}
	m.insert(std::pair<unsigned char, LinxChannel*>(channel, chan));
	return channel;
}

void LinxWindowsDevice::RegisterChannel(int type, unsigned char channel, LinxChannel *chan)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::pair<std::map<unsigned char, LinxChannel*>::iterator, bool> result = m.insert(std::pair<unsigned char, LinxChannel*>(channel, chan));
	if (!result.second)
	{
		result.first->second->Release();
		result.first->second = chan;
		result.first->second->AddRef();
	}
}

LinxChannel* LinxWindowsDevice::LookupChannel(int type, unsigned char channel)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::map<unsigned char, LinxChannel*>::iterator it = m.find(channel);
	return it != m.end() ? it->second->QueryInterface(type) : NULL;
}

void LinxWindowsDevice::RemoveChannel(int type, unsigned char channel)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::map<unsigned char, LinxChannel*>::iterator it = m.find(channel);
	if (it != m.end())
	{
		it->second->Release();
		m.erase(it);
	}
}

void LinxWindowsDevice::ClearChannels(int type)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin(); it != m.end(); it++)
	{
		int count = it->second->Release();
		if (count)
		{
			m_Debug->Write("Channel not released! Bad refcount");
		}
	}
	m.clear();
}
