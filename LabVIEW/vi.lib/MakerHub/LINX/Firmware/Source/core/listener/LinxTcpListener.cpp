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
LinxTcpListener::LinxTcpListener(LinxDevice* device) : LinxListener(device)
{
	m_ServerSocket = INVALID_SOCKET;
#if Win32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0)
	{
		m_Debug->Write("WSAStartup failed: ");
		m_Debug->Writeln(iResult);
	}
#endif
}

LinxTcpListener::~LinxTcpListener()
{
	Close();
#if Win32
	int iResult = WSACleanup();
#endif
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxTcpListener::Start(unsigned int interfaceAddress, unsigned short port)
{
	//Construct the server sockaddr_in structure
	memset(&m_TcpServer, 0, sizeof(struct sockaddr_in));	// Clear Struct
	m_TcpServer.sin_family = AF_INET;						// Internet/IP
	m_TcpServer.sin_addr.s_addr = htonl(interfaceAddress);  // Incoming Addr
	m_TcpServer.sin_port = htons(port);						// Server Port

	m_Debug->Write("Starting listener on TCP/IP address: ");
	m_Debug->Write(inet_ntoa(m_TcpServer.sin_addr));
	m_Debug->Write(":");
	m_Debug->Writeln((int)port);

	//Create the TCP socket
	if ((m_ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		m_Debug->Writeln("Failed to create server socket");
		return LERR_MEMORY;
	}
	m_Debug->Writeln("Successfully created server socket");

	//Bind the server socket
	if (bind(m_ServerSocket, (struct sockaddr *)&m_TcpServer, sizeof(struct sockaddr_in)) < 0)
	{
		m_Debug->Writeln("Failed to bind server socket");
		return LERR_MEMORY;
	}
	m_Debug->Writeln("Successfully bound server socket");

	//Listen on the server socket
	if (listen(m_ServerSocket, MAX_PENDING_CONS) < 0)
	{
		m_Debug->Writeln("Failed to start listening on sever socket");
		return LERR_MEMORY;
	}
	m_Debug->Writeln("Successfully started listening on sever socket");
	return L_OK;
}

int LinxTcpListener::WaitForConnection()
{
	int status = LERR_MEMORY;
	OSSocket clientSocket = INVALID_SOCKET;
	
	m_Debug->Writeln("Waiting For Client Connection\n");

	int clientlen = sizeof(m_TcpClient);
	if  ((clientSocket = accept(m_ServerSocket, (struct sockaddr *)&m_TcpClient, &clientlen)) < 0)
	{
		m_Debug->Writeln("Failed to accept client connection\n");
		return -1;
	}

	if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_TcpTimeout, sizeof(m_TcpTimeout)) < 0)
	{
		m_Debug->Writeln("Failed to set socket receive time-out\n");
		closesocket(clientSocket);
		return -1;
	}

	char *buf;
	unsigned short port;
	switch (m_TcpClient.sin_family)
	{
		case AF_INET:
			{
				struct sockaddr_in *addr_in = (struct sockaddr_in *)&m_TcpClient;
				port = addr_in->sin_port;
				buf = (char*)malloc(INET_ADDRSTRLEN + 6);
				inet_ntop(AF_INET, &(addr_in->sin_addr), buf, INET_ADDRSTRLEN);
			}
			break;
		case AF_INET6:
			{
				struct sockaddr_in6 *addr_in = (struct sockaddr_in6 *)&m_TcpClient;
				port = addr_in->sin6_port;
				buf = (char*)malloc(INET6_ADDRSTRLEN + 6);
				inet_ntop(AF_INET6, &(addr_in->sin6_addr), buf, INET6_ADDRSTRLEN);
			}
			break;
		default:
			return -1;
	}
	sprintf(buf + strlen(buf), ":%hd", port); 
	LinxCommChannel *clientChannel =
#if Unix
		new LinxUnixCommChannel(m_Debug, buf, clientSocket);
#elif Win32
		new LinxWindowsCommChannel(m_Debug, buf, clientSocket);
#endif
	free(buf);
	if (clientChannel)
	{
		m_TcpUpdateTime = m_LinxDev->GetSeconds();
		m_Debug->Write(inet_ntoa(m_TcpClient.sin_addr));
		m_Debug->Writeln(" successfully connected\n");
		status = LinxListener::Start(clientChannel, 4095);
	}
	return status;
}

int LinxTcpListener::Close()
{
	if (m_ServerSocket != INVALID_SOCKET)
	{
		closesocket(m_ServerSocket);
		m_ServerSocket = INVALID_SOCKET;
	}
	return LinxListener::Close();
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
