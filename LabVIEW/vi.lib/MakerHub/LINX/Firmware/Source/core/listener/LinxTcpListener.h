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
		LinxTcpListener(LinxDevice* device, bool autoLaunch = false);
		virtual ~LinxTcpListener(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Start(const unsigned char *interfaceAddress, uint16_t port = 44300);
		virtual int32_t Start(const unsigned char *interfaceAddress, const char *servName);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t WaitForConnection(int32_t timeout);
		virtual int32_t Terminate(void);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		uint32_t m_TcpUpdateTime;

		NetObject m_ServerSocket;
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_TCP_LISTENER_H