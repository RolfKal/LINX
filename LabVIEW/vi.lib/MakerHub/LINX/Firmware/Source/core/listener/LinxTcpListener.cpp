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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifndef _MSC_VER
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
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
LinxLinuxTcpListener::LinxLinuxTcpListener()
{
	m_TcpTimeout.tv_sec = 10;		//Set Socket Time-out To Default Value
	m_TcpTimeout.tv_usec = 0;
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxLinuxTcpListener::Start(LinxDevice* debug, LinxDevice* device, unsigned int interfaceAaddress, unsigned short port)
{
	unsigned char buffer[255];
	m_LinxDebug = debug;
	m_LinxDev = device;

	m_LinxDev->GetDeviceName(buffer, 255);
	m_LinxDebug->DebugPrint("Starting Listener on ");
	m_LinxDebug->DebugPrint((char *)buffer);
	m_LinxDebug->DebugPrint(", Serial Port: ");

	//Create the TCP socket
	if ((m_ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		m_LinxDebug->DebugPrintln("Failed To Create Socket");
		return -1;
	}
	m_LinxDebug->DebugPrintln("Successfully Created Socket");

	//Construct the server sockaddr_in structure
	memset(&m_TcpServer, 0, sizeof(struct sockaddr_in));	//Clear Struct
	m_TcpServer.sin_family = AF_INET;						//Internet/IP
	m_TcpServer.sin_addr.s_addr = htonl(interfaceAaddress); //Incoming Addr
	m_TcpServer.sin_port = htons(port);						//Server Port

	//Bind the server socket
	if (bind(m_ServerSocket, (struct sockaddr *)&m_TcpServer, sizeof(struct sockaddr_in)) < 0)
	{
		m_LinxDebug->DebugPrintln("Failed To Bind Sever Socket");
		return -1;
	}
	m_LinxDebug->DebugPrintln("Successfully Bound Sever Socket");

	//Listen on the server socket
	if (listen(m_ServerSocket, MAX_PENDING_CONS) < 0)
	{
		m_LinxDebug->DebugPrintln("Failed To Start Listening On Sever Socket");
		return -1;
	}
	m_LinxDebug->DebugPrintln("Successfully Started Listening On Sever Socket");
	return LinxListener::Start(4095);
}

int LinxLinuxTcpListener::Start(LinxDevice* device, unsigned int interfaceAddress, unsigned short port)
{
	return Start(device, device, interfaceAddress, port);
}

int LinxLinuxTcpListener::WaitForConnection()
{
	m_LinxDebug->DebugPrintln("Waiting For Client Connection\n");

	int clientlen = sizeof(m_TcpClient);
	if  ((m_ClientSocket = accept(m_ServerSocket, (struct sockaddr *)&m_TcpClient, &clientlen)) < 0)
	{
		m_LinxDebug->DebugPrintln("Failed To Accept Client Connection\n");
		return -1;
	}

	if (setsockopt(m_ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_TcpTimeout, sizeof(m_TcpTimeout)) < 0)
	{
		m_LinxDebug->DebugPrintln("Failed To Set Socket Receive Time-out\n");
		return -1;
	}
	m_TcpUpdateTime = m_LinxDev->GetSeconds();
	m_LinxDebug->DebugPrintln(inet_ntoa(m_TcpClient.sin_addr));
	m_LinxDebug->DebugPrintln("Successfully Connected\n");

	return 0;
}

int LinxLinuxTcpListener::Close()
{
	closesocket(m_ServerSocket);
	closesocket(m_ClientSocket);

	return 0;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/
int LinxLinuxTcpListener::ReadData(unsigned char *buffer, int bytesToRead, int *numBytesRead)
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
		m_ClientSocket = 0;
		return LERR_CLOSED_BY_PEER;
	}
	return LERR_IO;
}

int LinxLinuxTcpListener::WriteData(unsigned char *buffer, int bytesToWrite)
{
	int retval = send(m_ClientSocket, (char*)buffer, bytesToWrite, 0);
	if (retval > 0)
	{
		return L_OK;
	}
	if (!retval)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket = 0;
		return LERR_CLOSED_BY_PEER;
	}
	return LERR_IO;
}

int LinxLinuxTcpListener::FlushData()
{
	return L_OK;
}

// Pre Instantiate Object
LinxLinuxTcpListener LinxTcpConnection = LinxLinuxTcpListener();
