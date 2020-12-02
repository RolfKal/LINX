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
		LinxWindowsCommChannel(LinxFmtChannel *debug, const char *channelName, OSSocket socket);
		LinxWindowsCommChannel(LinxFmtChannel *debug, const char *address, unsigned short port);
		~LinxWindowsCommChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead);
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout);
		virtual int Close();

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		OSSocket m_Socket;
};

class LinxWindowsUartChannel : public LinxUartChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWindowsUartChannel(LinxFmtChannel *debug, const char *deviceName);
		~LinxWindowsUartChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SetSpeed(unsigned int speed, unsigned int* actualSpeed);
		virtual int SetBitSizes(unsigned char dataBits, unsigned char stopBits);
		virtual int SetParity(LinxUartParity parity);
		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead);
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout);
		virtual int Close();

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SmartOpen();

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		HANDLE m_Handle;
};

class LinxWindowsSocketChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWindowsSocketChannel(LinxFmtChannel *debug, const char *name, OSSocket socket);
		LinxWindowsSocketChannel(LinxFmtChannel *debug, const char *address, unsigned short port);
		~LinxWindowsSocketChannel();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead);
		virtual int Write(unsigned char* sendBuffer, int numBytes, int timeout);
		virtual int Close();

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		OSSocket m_Socket;
};
#endif //LINX_WINDOWSCHANNEL_H