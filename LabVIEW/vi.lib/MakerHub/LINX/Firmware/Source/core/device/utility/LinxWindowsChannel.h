/****************************************************************************************
**  LINX header for Windows support functions used LinxDeviceLib on host
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermater based on code from Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_WINDOWSCHANNEL_H
#define LINX_WINDOWSCHANNEL_H

/****************************************************************************************
**  Defines
****************************************************************************************/

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stdio.h>
#include "LinxDefines.h"
#include "LinxDevice.h"

using namespace std;

class LinxWindowsCommChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWindowsCommChannel(LinxFmtChannel *debug, const unsigned char *channelName, NetObject socket);
		LinxWindowsCommChannel(LinxFmtChannel *debug, const unsigned char *address, unsigned short port);
		~LinxWindowsCommChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Read(unsigned char* recBuffer, unsigned int numBytes, int timeout, unsigned int* numBytesRead);
		virtual int Write(const unsigned char* sendBuffer, unsigned int numBytes, int timeout);
		virtual int Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		NetObject m_Socket;
};

class LinxWindowsUartChannel : public LinxUartChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWindowsUartChannel(LinxFmtChannel *debug, const unsigned char *deviceName);
		LinxWindowsUartChannel(LinxFmtChannel *debug, unsigned char channel, const unsigned char *deviceName);
		~LinxWindowsUartChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed);
		virtual int SetParameters(unsigned char dataBits, unsigned char stopBits, LinxUartParity parity);
		virtual int Read(unsigned char* recBuffer, unsigned int numBytes, int timeout, unsigned int* numBytesRead);
		virtual int Write(const unsigned char* sendBuffer, unsigned int numBytes, int timeout);
		virtual int Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SmartOpen(void);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		HANDLE m_Handle;
		CHAR m_DeviceName[10];
};

class LinxWindowsSocketChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWindowsSocketChannel(LinxFmtChannel *debug, const char *name, NetObject socket);
		LinxWindowsSocketChannel(LinxFmtChannel *debug, const char *address, unsigned short port);
		~LinxWindowsSocketChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead);
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout);
		virtual int Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		NetObject m_Socket;
};
#endif //LINX_WINDOWSCHANNEL_H