/****************************************************************************************
**  LINX Linux TCP listener code.
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
#include "LinxDefines.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifndef _MSC_VER
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define closesocket(s) close(s)
#else
#include <io.h>
#include <winsock2.h>
#endif

#include "LinxDevice.h"
#include "utility/LinxListener.h"
#include "LinxTcpListener.h"

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxTcpListener::LinxTcpListener(LinxDevice* device, LinxFmtChannel* debug) : LinxListener(device, debug)
{
	m_TcpTimeout.tv_sec = 10;		//Set Socket Time-out To Default Value
	m_TcpTimeout.tv_usec = 0;
	m_ServerSocket = INVALID_SOCKET;
	m_ClientSocket = INVALID_SOCKET;
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
	memset(&m_TcpServer, 0, sizeof(struct sockaddr_in));	//Clear Struct
	m_TcpServer.sin_family = AF_INET;						//Internet/IP
	m_TcpServer.sin_addr.s_addr = htonl(interfaceAddress); //Incoming Addr
	m_TcpServer.sin_port = htons(port);						//Server Port

	m_Debug->Write("Starting Listener on TCP/IP Address: ");
	m_Debug->Write(inet_ntoa(m_TcpServer.sin_addr));
	m_Debug->Write(":");
	m_Debug->Writeln((int)port);

	//Create the TCP socket
	if ((m_ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		m_Debug->Writeln("Failed To Create Socket");
		return -1;
	}
	m_Debug->Writeln("Successfully Created Socket");

	//Bind the server socket
	if (bind(m_ServerSocket, (struct sockaddr *)&m_TcpServer, sizeof(struct sockaddr_in)) < 0)
	{
		m_Debug->Writeln("Failed To Bind Sever Socket");
		return -1;
	}
	m_Debug->Writeln("Successfully Bound Sever Socket");

	//Listen on the server socket
	if (listen(m_ServerSocket, MAX_PENDING_CONS) < 0)
	{
		m_Debug->Writeln("Failed To Start Listening On Sever Socket");
		return -1;
	}
	m_Debug->Writeln("Successfully Started Listening On Sever Socket");
	return LinxListener::Start(NULL, 4095);
}

int LinxTcpListener::WaitForConnection()
{
	m_Debug->Writeln("Waiting For Client Connection\n");

	int clientlen = sizeof(m_TcpClient);
	if  ((m_ClientSocket = accept(m_ServerSocket, (struct sockaddr *)&m_TcpClient, &clientlen)) < 0)
	{
		m_Debug->Writeln("Failed To Accept Client Connection\n");
		return -1;
	}

	if (setsockopt(m_ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_TcpTimeout, sizeof(m_TcpTimeout)) < 0)
	{
		m_Debug->Writeln("Failed To Set Socket Receive Time-out\n");
		return -1;
	}
	m_TcpUpdateTime = m_LinxDev->GetSeconds();
	m_Debug->Writeln(inet_ntoa(m_TcpClient.sin_addr));
	m_Debug->Writeln("Successfully Connected\n");

	return 0;
}

int LinxTcpListener::Close()
{
	if (m_ServerSocket != INVALID_SOCKET)
	{
		closesocket(m_ServerSocket);
		m_ServerSocket = INVALID_SOCKET;
	}
	if (m_ClientSocket != INVALID_SOCKET)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;
	}
	return 0;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int LinxTcpListener::ReadData(unsigned char *buffer, int bytesToRead, int *numBytesRead)
{
	int retval = recv(m_ClientSocket, (char*)buffer, bytesToRead, MSG_WAITALL);
	if (retval > 0)
	{
		*numBytesRead = retval;
		return L_OK;
	}
	if (!retval)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;
		return LERR_CLOSED_BY_PEER;
	}
	return LERR_IO;
}

int LinxTcpListener::WriteData(unsigned char *buffer, int bytesToWrite)
{
	int retval = send(m_ClientSocket, (char*)buffer, bytesToWrite, 0);
	if (retval > 0)
	{
		return L_OK;
	}
	if (!retval)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;
		return LERR_CLOSED_BY_PEER;
	}
	return LERR_IO;
}

int LinxTcpListener::FlushData()
{
	return L_OK;
}
