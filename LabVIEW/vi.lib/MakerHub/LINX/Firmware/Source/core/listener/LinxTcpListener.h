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
#endif
#include "utility\LinxListener.h"
#include "LinxDevice.h"

/****************************************************************************************
**  Classes
****************************************************************************************/
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
	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		unsigned int m_TcpUpdateTime;
		struct timeval m_TcpTimeout;

		OSSocket m_ServerSocket;

		struct sockaddr_in m_TcpServer;
		struct sockaddr_in m_TcpClient;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_TCP_LISTENER_H