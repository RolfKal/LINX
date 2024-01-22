/****************************************************************************************
**  LINX Windows host Code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written by Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinxWindowsDevice.h"
#include "LinxWindowsChannel.h"
#include <setupapi.h>

/****************************************************************************************
**  Constructors/Destructor
****************************************************************************************/
LinxWindowsDevice::LinxWindowsDevice(LinxFmtChannel *debug) : LinxDevice(debug)
{
	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;

	//-------------------------------- Device Detection ------------------------------
	DeviceFamily = 0x04;	// Windows Family Code
#if Win64
	DeviceId = 0x1;		// Windows 64-bit
#elif Win32
	DeviceId = 0x2;		// Windows 32-bit
#endif

	//------------------------------------- AI ---------------------------------------
	
	//------------------------------------- AO ---------------------------------------

	//------------------------------------- DIO --------------------------------------

	//------------------------------------- PWM --------------------------------------

	//------------------------------------- QE ---------------------------------------

	//------------------------------------- UART -------------------------------------
	// Enumerate all currently known Uart channels and store them in the registry map
	// EnumerateCommPorts(&GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, DIGCF_PRESENT);
	EnumerateCommPorts(&GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	//------------------------------------- I2C -------------------------------------

	//------------------------------------- SPI -------------------------------------

	//------------------------------------- CAN -------------------------------------

	//------------------------------------ SERVO ------------------------------------
}

LinxWindowsDevice::~LinxWindowsDevice()
{
}

unsigned char LinxWindowsDevice::EnumerateCommPorts(const GUID *guid, DWORD dwFlags)
{
	HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, NULL, NULL, dwFlags);
	if (INVALID_HANDLE_VALUE != hDevInfo)
	{
		DWORD i, buffersize = 0;
		SP_DEVINFO_DATA devInfoData = {0};
		LPSTR buffer = NULL;

		// Enumerate through all devices in Set.
		devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
		{
			HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
			if (hKey != INVALID_HANDLE_VALUE)
			{
				DWORD error, dwSize, dwType = 0;
				ULONG length = 0;
//				WCHAR nameBuf[MAX_PATH];
//				error = RegQueryKeyPath(hKey, nameBuf, MAX_PATH);

				error = RegGetValueW(hKey, NULL, L"PortName", RRF_RT_REG_SZ, &dwType, NULL, &length);
				if (error == ERROR_SUCCESS || error == ERROR_MORE_DATA)
				{
					LPWSTR sPortName = (LPWSTR)malloc(length);
					if (sPortName)
					{
						error = RegGetValueW(hKey, NULL, L"PortName", RRF_RT_REG_SZ, &dwType, sPortName, &length);
						if (error == ERROR_SUCCESS)
						{
							//If it looks like "COMx" then add it to the array which will be returned
							if (length / sizeof(WCHAR) > 3 && !_wcsnicmp(sPortName, L"COM", 3) && iswdigit(sPortName[3]))
							{
								//Work out the port number
								int nPort = _wtoi(sPortName + 3);
								
								//Query initially to get the buffer size required
								if (!SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, &dwType, NULL, 0, &dwSize))
								{
									if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
										continue;
								}
								unsigned char *sFriendlyName = (unsigned char*)malloc(dwSize);

								if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, &dwType, (PBYTE)sFriendlyName, dwSize, &dwSize) && dwType == REG_SZ)
								{
									RegisterChannel(IID_LinxUartChannel, (unsigned char)nPort, (LinxUartChannel*)new LinxWindowsUartChannel(m_Debug, nPort, sFriendlyName));
								}
								free(sFriendlyName);
							}
						}
						free(sPortName);
					}
				}
			}
		}
		SetupDiDestroyDeviceInfoList(hDevInfo);
	}
	return L_OK;}

unsigned char LinxWindowsDevice::GetDeviceName(unsigned char *buffer, unsigned char length)
{
	DWORD len = length;
	if (!GetComputerNameExA(ComputerNamePhysicalDnsFullyQualified, (LPSTR)buffer, &len))
		return L_UNKNOWN_ERROR;
	return L_OK;
}
