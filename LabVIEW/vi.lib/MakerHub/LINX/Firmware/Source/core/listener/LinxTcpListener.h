/****************************************************************************************
**  LINX header for Linux TCP listener.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_TCP_LISTENER_H
#define LINX_TCP_LISTENER_H


#ifndef MAX_PENDING_CONS
	#define MAX_PENDING_CONS 2
#endif

/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LinxDefines.h"
#include <stdio.h>
#if Unix
#include <sys/time.h>
#include <netinet/in.h>
#define NetSocket int
#elif Win32
#include <winsock.h>
#define NetSocket SOCKET
#endif
#include "utility\LinxListener.h"
#include "LinxDevice.h"

/****************************************************************************************
**  Classes
****************************************************************************************/
class LinxTcpChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxTcpChannel(LinxFmtChannel *debug, NetSocket socket, int timeout);
		virtual ~LinxTcpChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead);
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout);
		virtual int Close();
};

class LinxTcpListener : public LinxListener
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors/Destructor
		****************************************************************************************/
		LinxTcpListener(LinxDevice* device);
		virtual ~LinxTcpListener();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Start(unsigned int interfaceAaddress = INADDR_ANY, unsigned short port = 44300);
		virtual int WaitForConnection();
		virtual int Close();

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int ReadData(unsigned char *buffer, int bytesToRead, int *numBytesRead);
		int WriteData(unsigned char *buffer, int bytesToWrite);
		int FlushData();

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		unsigned int m_TcpUpdateTime;
		struct timeval m_TcpTimeout;

		NetSocket m_ServerSocket;
		NetSocket m_ClientSocket;

		struct sockaddr_in m_TcpServer;
		struct sockaddr_in m_TcpClient;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_TCP_LISTENER_H