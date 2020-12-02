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

#include "LinxWindowsChannel.h"
#include "LinxUtilities.h"

/***************************************** Socket ****************************************/

/****************************************************************************************
**  Constructor/Destructors
****************************************************************************************/
LinxWindowsCommChannel::LinxWindowsCommChannel(LinxFmtChannel *debug, const char *channelName, OSSocket socket) : LinxCommChannel(debug, channelName)
{
	u_long nonBlocking = 1;
	m_Socket = socket;
	ioctlsocket(m_Socket, FIONBIO, &nonBlocking);
}

LinxWindowsCommChannel::LinxWindowsCommChannel(LinxFmtChannel *debug, const char *address, unsigned short port) : LinxCommChannel(debug, address)
{
	struct addrinfo hints, *result, *rp;
	char str[10];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICSERV;

	sprintf(str, "%hu", port);
	if (!getaddrinfo(address,  str, &hints, &result))
	{
		for (rp = result; rp != NULL; rp = rp->ai_next)
		{
			m_Socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (m_Socket < 0)
				continue;

			switch (rp->ai_addr->sa_family)
			{
				case AF_INET:
					((sockaddr_in*)rp->ai_addr)->sin_port = port;
					break;
				case AF_INET6:
					((sockaddr_in6*)rp->ai_addr)->sin6_port = port;
					break;
				default:
					continue;
			}
            if (connect(m_Socket, rp->ai_addr, (socklen_t)rp->ai_addrlen) != -1)
			{
				u_long nonBlocking = 1;
				if (ioctlsocket(m_Socket, FIONBIO, &nonBlocking) >= 0)
					break;
			}
			closesocket(m_Socket);
		}
		freeaddrinfo(result);
		if (rp == NULL)
		{       
			m_Debug->Write("Could not connect to TCP/IP address: ");
			m_Debug->Writeln(address);
		}
	}
}

LinxWindowsCommChannel::~LinxWindowsCommChannel()
{
	if (m_Socket != INVALID_SOCKET)
		closesocket(m_Socket);
}

#define kRetryLimit 25

int LinxWindowsCommChannel::Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead)
{
	*numBytesRead = 0;

	if (recBuffer && numBytes)
	{
		unsigned long long start = getMsTicks();
		struct timeval tout, *pto = timeout < 0 ? NULL : &tout;
 		int retval, syserr;
        fd_set readfds;     /* read sockets */

		FD_ZERO(&readfds);
		FD_SET(m_Socket, &readfds);

		while (*numBytesRead < numBytes)
		{
			int i = 0;
			do
			{
				if (pto)
				{
					int elapsed = (int)(getMsTicks() - start);
					if (elapsed < timeout)
					{
						elapsed = timeout - elapsed;
						pto->tv_sec = elapsed / 1000;
						pto->tv_usec = (elapsed % 1000) * 1000;
					}
					else
					{
						pto->tv_sec = 0;
						pto->tv_usec = 0;
					}
				}
			
				retval = select(0, &readfds, NULL, NULL, pto);
				if (retval < 0)
				{
					syserr = WSAGetLastError();
					if ((syserr & 0xFFFF) != WSAEINTR || ++i >= kRetryLimit)
						return LUART_READ_FAIL;
				}
			}
			while (retval < 0);

			if (!retval)
				return LUART_TIMEOUT;

			// some socket event was triggered, check which one
			if (FD_ISSET(m_Socket, &readfds))
			{
				// Read bytes from input buffer
				retval = recv(m_Socket, (char*)recBuffer + *numBytesRead, numBytes - *numBytesRead, 0);
				if (retval < 0)
					return LUART_READ_FAIL;
				else if (!retval)
					return LERR_CLOSED_BY_PEER;

				*numBytesRead += retval;
			}
		}
	}
	else
	{
		// Check how many bytes are available
		if (ioctlsocket(m_Socket, FIONREAD, (u_long*)numBytesRead) < 0)
			return LUART_READ_FAIL;
	}
	return L_OK;
}

int LinxWindowsCommChannel::Write(unsigned char* sendBuffer, int numBytes, int timeout)
{
	if (sendBuffer && numBytes)
	{
		unsigned long long start = getMsTicks();
		struct timeval tout, *pto = timeout < 0 ? NULL : &tout;
 		int bytesSent = 0, retval, syserr;
        fd_set writefds;     /* write sockets */

		FD_ZERO(&writefds);
		FD_SET(m_Socket, &writefds);

		while (bytesSent < numBytes)
		{
			int i = 0;
			do
			{
				if (pto)
				{
					int elapsed = (int)(getMsTicks() - start);
					if (elapsed < timeout)
					{
						elapsed = timeout - elapsed;
						pto->tv_sec = elapsed / 1000;
						pto->tv_usec = (elapsed % 1000) * 1000;
					}
					else
					{
						pto->tv_sec = 0;
						pto->tv_usec = 0;
					}
				}
			
				retval = select(0, NULL, &writefds, NULL, pto);
				if (retval < 0)
				{
					syserr = WSAGetLastError();
					if (!pto || (syserr & 0xFFFF) != WSAEINTR || ++i >= kRetryLimit)
						return LUART_READ_FAIL;
				}
			}
			while (retval < 0);

			if (!retval)
				return LUART_TIMEOUT;

			// some socket event was triggered, check which one
			if (FD_ISSET(m_Socket, &writefds))
			{
				retval = send(m_Socket, (char*)sendBuffer + bytesSent, numBytes - bytesSent, 0);
				if (retval < 0)
					return LUART_WRITE_FAIL;

				bytesSent += retval;
			}
		}
	}
	return L_OK;
}

int LinxWindowsCommChannel::Close()
{
	if (m_Socket != INVALID_SOCKET)
		closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;
	return L_OK;
}

/***************************************** Uart ****************************************/

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