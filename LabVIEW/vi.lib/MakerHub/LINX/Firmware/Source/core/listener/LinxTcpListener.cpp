/****************************************************************************************
**  LINX Linux TCP listener code.
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
#include "LinxDefines.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#if Unix
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <LinxUnixChannel.h>
#elif Win32
#include <io.h>
#include <LinxWindowsChannel.h>
#endif

#include "LinxDevice.h"
#include "utility/LinxListener.h"
#include "LinxTcpListener.h"

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxTcpListener::LinxTcpListener(LinxDevice* device, bool autoLaunch) : LinxListener(device, autoLaunch)
{
	m_ServerSocket = kInvalNetObject;
#if Win32
	WSADATA wsaData;
	int32_t iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0)
	{
		m_Debug->Write("WSAStartup failed: ");
		m_Debug->Writeln(iResult);
	}
#endif
}

LinxTcpListener::~LinxTcpListener(void)
{
	closesocket(m_ServerSocket);
#if Win32
	int32_t iResult = WSACleanup();
#endif
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int32_t LinxTcpListener::Start(const unsigned char *interfaceAddress, uint16_t port)
{
	char servName[10];
	sprintf(servName, "%hu", port);
	return Start(interfaceAddress, servName);
}

int32_t LinxTcpListener::Start(const unsigned char *interfaceAddress, const char *servName)
{
	const char *address = (interfaceAddress && interfaceAddress[0]) ? ((const char*)interfaceAddress) : NULL;
	struct addrinfo hints = {0};
	struct addrinfo *addrinfo = NULL;
	int32_t retval;

	m_Debug->Write("Starting listener on TCP/IP address: ");
	m_Debug->Write((char*)interfaceAddress);
	m_Debug->Write(":");
	m_Debug->Writeln(servName);

	//Create the TCP socket
	NetObject s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!IsANetObject(s))
	{
		m_Debug->Writeln("Failed to create server socket");
		return LERR_MEMORY;
	}
	m_Debug->Writeln("Successfully created server socket");

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
	retval = getaddrinfo(address, servName, &hints, &addrinfo);
	if (retval)
	{
		closesocket(s);
		m_Debug->Writeln("Failed to retrieve socket address");
		return LERR_MEMORY;
	}
	u_long nonBlocking = TRUE;
	retval = ioctlsocket(s, FIONBIO, &nonBlocking);
	if (retval < 0)
	{
		closesocket(s);
		m_Debug->Writeln("Failed to make server socket non-blocking");
		return L_DISCONNECT;
	}
	
	//Bind the server socket
	struct addrinfo *next = addrinfo;
	do
	{
		retval = bind(s, next->ai_addr, (int)next->ai_addrlen);
	} while (retval < 0 && (next = addrinfo->ai_next) != NULL);
	freeaddrinfo(addrinfo);
	if (retval < 0)
	{
		closesocket(s);
		m_Debug->Writeln("Failed to bind server socket");
		return L_DISCONNECT;
	}
	m_Debug->Writeln("Successfully bound server socket");

	//Listen on the server socket
	retval = listen(s, MAX_PENDING_CONS);
	if (retval < 0)
	{
		closesocket(s);
		m_Debug->Writeln("Failed to start listening on sever socket");
		return L_DISCONNECT;
	}
	ControlMutex(true);
	m_ServerSocket = s;
	m_Debug->Writeln("Successfully started listening on sever socket");
	ControlMutex(false);
	return L_OK;
}

int32_t LinxTcpListener::Terminate(void)
{
	ControlMutex(true);
	if (IsANetObject(m_ServerSocket))
	{
		closesocket(m_ServerSocket);
		m_ServerSocket = kInvalNetObject;
	}
	ControlMutex(false);
	return LinxListener::Terminate();
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int32_t LinxTcpListener::WaitForConnection(int32_t timeout)
{
	int32_t status = LinxListener::WaitForConnection(timeout);
	if (status == L_WAITING)
	{
		struct sockaddr_storage addr;
		struct timeval timeout_val;
		int32_t retval, clientlen = sizeof(addr);
		NetObject clientSocket = kInvalNetObject;
		LinxCommChannel *clientChannel;

		m_Debug->Writeln("Waiting For Client Connection\n");

		ControlMutex(true);
		clientSocket = accept(m_ServerSocket, (struct sockaddr *)&addr, &clientlen);
		ControlMutex(false);
		if  (!IsANetObject(clientSocket))
		{
			m_Debug->Writeln("Failed to accept client connection\n");
			return L_WAITING;
		}

		retval = setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout_val, sizeof(timeout_val));
		if (retval < 0)
		{
			m_Debug->Writeln("Failed to set socket receive time-out\n");
			closesocket(clientSocket);
			return LERR_IO;
		}

		unsigned char *buf;
		uint16_t port;
		switch (addr.ss_family)
		{
			case AF_INET:
				{
					struct sockaddr_in *addr_in = ((struct sockaddr_in *)&addr);
					port = addr_in->sin_port;
					buf = (unsigned char*)malloc(INET_ADDRSTRLEN + 6);
					inet_ntop(AF_INET, &(addr_in->sin_addr), (char*)buf, INET_ADDRSTRLEN);
				}
				break;
			case AF_INET6:
				{
					struct sockaddr_in6 *addr_in = (struct sockaddr_in6 *)&addr;
					port = addr_in->sin6_port;
					buf = (unsigned char*)malloc(INET6_ADDRSTRLEN + 6);
					inet_ntop(AF_INET6, &(addr_in->sin6_addr), (char*)buf, INET6_ADDRSTRLEN);
				}
				break;
			default:
				m_Debug->Writeln("Invalid socket family\n");
				closesocket(clientSocket);
				return LERR_IO;
		}
		sprintf((char*)buf + strlen((char*)buf), ":%hd", port); 

#if Unix
		clientChannel = new LinxUnixCommChannel(m_Debug, buf, clientSocket);
#elif Win32
		clientChannel =	new LinxWindowsCommChannel(m_Debug, buf, clientSocket);
#endif
		if (clientChannel)
		{
			m_TcpUpdateTime = m_LinxDev->GetSeconds();
			m_Debug->Write((char*)buf);
			m_Debug->Writeln(" successfully connected\n");
			status = LinxListener::Run(clientChannel, 4095);
		}
		free(buf);
	}
	return status;
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/

