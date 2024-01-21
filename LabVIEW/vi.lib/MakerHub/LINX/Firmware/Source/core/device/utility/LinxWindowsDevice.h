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

#ifndef LINX_WINDOWSDEVICE_H
#define LINX_WINDOWSDEVICE_H

/****************************************************************************************
**  Defines
****************************************************************************************/

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stdio.h>
#include <map>
#include <string>
#include "LinxDefines.h"
#include "LinxDevice.h"

/****************************************************************************************
**  Variables
****************************************************************************************/

class LinxWindowsDevice : public LinxDevice
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWindowsDevice(LinxFmtChannel *debug = NULL);
		~LinxWindowsDevice();

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		unsigned char EnumerateCommPorts(const GUID *guid, DWORD dwFlags);

};
#endif //LINX_WINDOWSDEVICE_H