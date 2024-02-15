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
		LinxWindowsCommChannel(LinxFmtChannel *debug, const unsigned char *address, uint16_t port);
		~LinxWindowsCommChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Read(unsigned char* recBuffer, uint32_t numBytes, uint32_t start, int32_t timeout, uint32_t* numBytesRead);
		virtual int32_t Write(const unsigned char* sendBuffer, uint32_t numBytes, uint32_t start, int32_t timeout);
		virtual int32_t Close(void);

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
		~LinxWindowsUartChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SetSpeed(uint32_t speed, uint32_t* actualSpeed);
		virtual int32_t SetParameters(uint8_t dataBits, uint8_t stopBits, LinxUartParity parity);
		virtual int32_t Read(unsigned char* recBuffer, uint32_t numBytes, uint32_t start, int32_t timeout, uint32_t* numBytesRead);
		virtual int32_t Write(const unsigned char* sendBuffer, uint32_t numBytes, uint32_t start, int32_t timeout);
		virtual int32_t Close(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SmartOpen(void);

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
		LinxWindowsSocketChannel(LinxFmtChannel *debug, const char *address, uint16_t port);
		~LinxWindowsSocketChannel(void);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Read(unsigned char* recBuffer, int32_t numBytes, int32_t timeout, int32_t* numBytesRead);
		virtual int32_t Write(unsigned char* sendBuffer, int32_t numBytes, int32_t timeout);
		virtual int32_t Close(void);

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