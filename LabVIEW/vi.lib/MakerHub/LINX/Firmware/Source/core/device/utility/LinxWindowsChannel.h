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
#include <windows.h>
#include "LinxDefines.h"
#include "LinxDevice.h"

using namespace std;

class LinxWindowsTcpChannel : public LinxCommChannel
{
	public:
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWindowsTcpChannel(LinxFmtChannel *debug, SOCKET fd);
		LinxWindowsTcpChannel(LinxFmtChannel *debug, const char *address, unsigned short port);
		~LinxWindowsTcpChannel();

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
		virtual int SmartOpen();

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		SOCKET m_Socket;
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
#endif //LINX_WINDOWSCHANNEL_H