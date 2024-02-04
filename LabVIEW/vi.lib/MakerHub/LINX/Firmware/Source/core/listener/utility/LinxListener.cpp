/****************************************************************************************
**  Generic LINX Listener code.  This file contains the parent class for LINX listeners.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written by Sam Kristoff
**  Modifications by Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/

/****************************************************************************************
** Includes
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "LinxCommand.h"
#include "LinxUtilities.h"
#include "LinxDevice.h"
#include "LinxListener.h"

/****************************************************************************************
** Defines
****************************************************************************************/
#define CUSTOM_CMD_PREFIX 0xFC00

/****************************************************************************************
**  Constructors/Destructors
****************************************************************************************/
LinxListener::LinxListener(LinxDevice *device, bool autoLaunch) : LinxBase()
{
	unsigned char buffer[255];

	m_Channel = NULL;
	m_PeriodicTask = NULL;
	for (int32_t i = 0; i < MAX_CUSTOM_CMDS; i++)
	{
		m_CustomCommands[i] = NULL;
	}
	m_ListenerBufferSize = 0;
	m_DataBuffer = NULL;
	m_ProtocolVersion = 0;

	m_LaunchThread = autoLaunch;
	m_Thread = 0;
	m_Run = FALSE;

	m_LinxDev = device;
	m_LinxDev->AddRef();

	m_LinxDev->GetDeviceName(buffer, 255);
	m_Debug = m_LinxDev->GetDebug();
	m_Debug->Write("Initializing Listener on ");
	m_Debug->Writeln((char *)buffer);
#if Win32
	InitializeCriticalSection(&m_Mutex);
#elif Linux
	m_Mutex = PTHREAD_MUTEX_INITIALIZER;
#else
#else
#endif
}

LinxListener::~LinxListener(void)
{
	Close();
	ControlMutex(true);
	free(m_DataBuffer);
	ControlMutex(false);
#if Win32
	DeleteCriticalSection(&m_Mutex);
#elif Linux
#else
#endif
}

#if Win32
static DWORD WINAPI ThreadFunction(LPVOID lpParam) 
{
    ((LinxListener *)lpParam)->ProcessLoop(TRUE);
	return 0;
}
#elif Linux
static void *ThreadFunction(void *lpParam)
{
    ((LinxListener *)lpParam)->ProcessLoop(TRUE);
	pthread_exit(NULL); 
	return NULL;
}
#else
#endif

/****************************************************************************************
** Public Functions
****************************************************************************************/
int32_t LinxListener::AttachCustomCommand(uint16_t commandNumber, CustomCommand callback)
{
	if (commandNumber < MAX_CUSTOM_CMDS)
	{
		ControlMutex(true);
		m_CustomCommands[commandNumber] = callback;
		ControlMutex(false);
		return L_OK;
	}
	return LERR_BADPARAM;
}

int32_t LinxListener::AttachPeriodicTask(PeriodicTask task)
{
	ControlMutex(true);
	m_PeriodicTask = task;
	ControlMutex(false);
	return L_OK;
}

int32_t LinxListener::ProcessLoop(bool loop)
{
	int32_t status;
	do
	{
		status = WaitForConnection();
		if (!status)
		{
			status = CheckForCommand();
		}
	} while (!status && loop);
	return status;
}

/****************************************************************************************
** Protected Functions
****************************************************************************************/
// Start Listener with the device to relay commands to
int32_t LinxListener::Run(LinxCommChannel *channel, int32_t timeout, int32_t bufferSize)
{
	int32_t status = L_OK;
	ControlMutex(true);
	if (m_ListenerBufferSize != bufferSize)
	{
		m_ListenerBufferSize = bufferSize;
		m_DataBuffer = (uint8_t *)realloc(m_DataBuffer, bufferSize);
	}
	m_Timeout = timeout;
	if (m_Channel)
		m_Channel->Release();
	if (channel)
		channel->AddRef();
	m_Channel = channel;
	m_Run = TRUE;
	if (m_LaunchThread)
	{
#if Win32
		DWORD dwThreadID = 0;
		m_Thread = CreateThread(NULL, 0, ThreadFunction, this, 0, &dwThreadID);
		if (!m_Thread)
		{
			status = L_DISCONNECT;
		}
#elif Linux
		int err = pthread_create(&m_Thread, NULL, ThreadFunction, this);
		if (err)
		{
			m_Thread = 0;
			status = L_DISCONNECT;
		}
#else
#endif
	}
	ControlMutex(false);
	return status;
}

int32_t LinxListener::WaitForConnection(void)
{
	return m_Run ? L_OK : L_DISCONNECT;
}

int32_t LinxListener::Close(void)
{
	ControlMutex(true);
	if (m_Thread)
	{
		m_Run = FALSE;
		ControlMutex(false);
#if Win32
		DWORD dwStatus = WaitForSingleObject(m_Thread, 2000);
		ControlMutex(true);
		if (dwStatus == WAIT_TIMEOUT)
		{
			TerminateThread(m_Thread, 1);
		}
		CloseHandle(m_Thread);
#elif Linux
		int status;
		//status = pthread_cancel(m_Thread);
		// Waiting for the created thread to terminate 
		status = pthread_join(ptid, NULL);
		ControlMutex(true);
#else

#endif
	}

	if (m_Channel)
	{
		m_Channel->Close();
		m_Channel->Release();
		m_Channel = NULL;
	}
	ControlMutex(false);
	return L_OK;
}

int32_t LinxListener::ControlMutex(bool lock)
{
#if Win32
	if (lock)
		EnterCriticalSection(&m_Mutex);
	else
		LeaveCriticalSection(&m_Mutex);
#elif Linux
	if (lock)
		pthread_mutex_lock(&m_Mutex);
	else
		pthread_mutex_unlock(&m_Mutex);
#endif
	return L_OK;
}

/****************************************************************************************
** Private Functions
****************************************************************************************/
// wire protocol
//  offset value
//   0     0xFF   start of frame
//   1     len    length of entire package including header
//   2            Hi(packetNum)
//   3			  Lo(packetNum)
//   4			  Hi(command)
//   5			  Lo(command)
//   6            len - 7 bytes data
//   n - 1        checksum

//  offset value
//   0     0xFF   start of frame
//   1     len    length of entire package including header
//   2            Hi(packetNum)
//   3			  Lo(packetNum)
//   4			  status
//   5            len - 6 bytes data
//   n - 1        checksum

// wire protocol
//  offset value
//   0     0xFE   start of frame
//   1     len2   
//   2     len1   length of entire package including header
//   3     len0   
//   4            Hi(packetNum)
//   5			  Lo(packetNum)
//   6			  Hi(command)
//   7			  Lo(command)
//   8            len - 9 bytes data
//   n - 1        checksum

//  offset value
//   0     0xFE   start of frame
//   1     len2   
//   2     len1   length of entire package including header
//   3     len0   
//   4            Hi(packetNum)
//   5			  Lo(packetNum)
//   6			  status
//   7            len - 8 bytes data
//   n - 1        checksum

int32_t LinxListener::CheckForCommand(void)
{
	int32_t status = L_DISCONNECT;
	if (m_Channel)
	{
		uint64_t start = getMsTicks();
		uint32_t timeout = m_Timeout, dataRead = 0;

		// Try to read first 4 bytes
		status = m_Channel->Read(m_DataBuffer, 4, timeout, &dataRead);
		if (!status)
		{
			int32_t offset, msgLength, length = dataRead;
			// Decode length in package
			if (m_DataBuffer[0] == 0xFF && dataRead >= 2)
			{
				msgLength = m_DataBuffer[1];
				offset = 4;
			}
			else if (m_DataBuffer[0] == 0xFE && dataRead >= 4)
			{
				msgLength = GetU32FromBuff(m_DataBuffer, 0) & 0xFFFFFF;
				offset = 6;
			}
			else
			{
				// invalid data frame, flush buffer and return
				m_Channel->Read(m_DataBuffer, m_ListenerBufferSize, 0, NULL);
				return LERR_INVALID_FRAME;
			}
		
			// if expected msgLength is greater than the data already received then read the remainder
			while (msgLength > length)
			{
				if (timeout >= 0)
				{
					timeout = m_Timeout - (int32_t)(getMsTicks() - start);
					if (timeout < 0)
						timeout = 0;
				}
				status = m_Channel->Read(m_DataBuffer + dataRead, msgLength - dataRead, timeout, &dataRead);
				if (status)
					return status;

				length += dataRead;
			}

			if (ComputeChecksum(0, m_DataBuffer, length - 1) != m_DataBuffer[length - 1])
			{
				return LERR_CHECKSUM;
			}
			status = ProcessCommand(m_DataBuffer, offset, length - offset, m_ListenerBufferSize);
			if (m_PeriodicTask)
				status = m_PeriodicTask(m_DataBuffer, m_DataBuffer);
		}
	}
	return status;
}

int32_t LinxListener::PacketizeAndSend(uint8_t* packetBuffer, uint32_t dataSize, int32_t status)
{
	int32_t offset = 0;

	// Fill in packet size
	if (packetBuffer[0] == 0xFE)
	{
		offset = WriteU32ToBuff(packetBuffer, 0, dataSize + 8);
		packetBuffer[0] = 0xFE;
	}
	else if (packetBuffer[0] == 0xFF)
	{
		offset = WriteU8ToBuff(packetBuffer, 1, dataSize + 6);
	}

	// Skip packet number
	offset += 2;

	// Make Sure Status Is Valid
	if (status >= 0 && status <= 255)
	{
		packetBuffer[offset++] = (unsigned char)status;	//Status
	}
	else
	{
		packetBuffer[offset++] = L_UNKNOWN_ERROR;		//Status
	}

	// Compute and append checksum
	packetBuffer[offset + dataSize] = ComputeChecksum(0, packetBuffer, offset + dataSize);

	// Send it off
	return m_Channel->Write(packetBuffer, offset + dataSize + 1, m_Timeout);
}

int32_t LinxListener::EnumerateChannels(int32_t type, uint8_t protVersion, uint8_t *packetBuffer, uint32_t offset, uint32_t length)
{
	// Command parameters
	//  None
	// Response parameters
	//  uint8[] : array of UART channel identifiers
	// or when protVersion >= 1
	//  Response parameters
	//  uint8 : numChans
	//  uint8[] : array of num Chans UART channel identifiers
	//  uint8[] : numChans of Pascal strings

	if (protVersion)
	{
		packetBuffer[offset - 1] = m_LinxDev->EnumerateChannels(type, packetBuffer + offset, length - offset, &length); 
		length++;
	}
	else
	{
		offset--;
		length = m_LinxDev->EnumerateChannels(type, packetBuffer + offset, length - offset, NULL); 
	}
	return length;
}

int32_t LinxListener::ProcessCommand(uint8_t* packetBuffer, uint32_t offset, uint32_t dataLength, uint32_t bufferLength)
{
	//Store Some Local Values For Convenience
	int32_t status = LERR_BADPARAM;
	uint16_t command;
	
	offset = ReadU16FromBuff(packetBuffer, offset, &command);
//	bool extension = (command & LCMD_EXTENDED_FLAG) == EXTENDED_CMD_FLAG;
	
	if ((command & LCMD_EXTENDED_FLAG) == CUSTOM_CMD_PREFIX)
	{
		command &= ~LCMD_EXTENDED_FLAG;

		/****************************************************************************************
		** User Commands
		****************************************************************************************/
		if (((command) < MAX_CUSTOM_CMDS) && (m_CustomCommands[command]))
		{
			bufferLength -= offset + 1;
			status = m_CustomCommands[command](packetBuffer + offset, dataLength, packetBuffer + offset - 1, &bufferLength);
			dataLength = bufferLength;
		}
		else
		{
			dataLength = 0;
		}
	}
	else		
	{
		//Standard Commands
		switch (command & ~EXTENDED_CMD_FLAG)
		{
		/************************************************************************************
		* SYSTEM COMMANDS
		************************************************************************************/
		case LCMD_SYNC: // Sync Packet
			// Command parameters
			//  None
			// or
			//  uint8 : Protocol Version
			// Response parameters
			//  None
			// or
			//  uint8 : Protocol Version
			if (dataLength)
			{
				m_ProtocolVersion = Min(packetBuffer[offset], PROTOCOL_VERSION);
				packetBuffer[offset - 1] = m_ProtocolVersion; 
			}
			else
			{
				m_ProtocolVersion = 0;
			}
			status = L_OK;
			break;

		//case 0x0001: //TODO Flush Linx Connection Buffer
		//case 0x0002: //TODO Reset

		case LCMD_GET_DEV_ID: // Get Device ID
			// Command parameters
			// None
			// Response parameters
			// uint8 : device family
			// uint8 : device id
			if (bufferLength >= offset + 1)
			{
				offset = WriteU8ToBuff(packetBuffer, offset - 1, m_LinxDev->DeviceFamily);
				offset = WriteU8ToBuff(packetBuffer, offset, m_LinxDev->DeviceId);
				status = L_OK;
				dataLength = 2;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_GET_API_VER: //Get LINX API Version
			// Command parameters
			// None
			// Response parameters
			// uint8 : api major version
			// uint8 : api minor version
			// uint8 : api subminor version
			if (bufferLength >= offset + 2)
			{
				offset = WriteU8ToBuff(packetBuffer, offset - 1, m_LinxDev->LinxApiMajor);
				offset = WriteU8ToBuff(packetBuffer, offset, m_LinxDev->LinxApiMinor);
				offset = WriteU8ToBuff(packetBuffer, offset, m_LinxDev->LinxApiSubminor);
				status = L_OK;
				dataLength = 3;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_GET_UART_MAX_BAUD: //Get UART Listener Interface Max Baud
			// Command parameters
			// None
			// Response parameters
			// uint32 : max uart baudrate
			if (bufferLength >= offset + 3)
			{
				WriteU32ToBuff(packetBuffer, offset - 1, m_LinxDev->UartMaxBaud);
				status = L_OK;
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		// case LCMD_SET_UART_MAX_BAUD: //Set UART Listener Interface Max Baud
			// Command parameters
			// uint32 : max uart baudrate
			// Response parameters
			// uint32 : actual used uart baudrate

		// case LCMD_GET_MAX_PACK_SIZE: // Get Max Packet Size
			// Command parameters
			// None
			// Response parameters
			// uint32 : listenere buffer size

		case LCMD_GET_DIO_CHANS: // Get DIO Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of digital channel identifiers
			dataLength = EnumerateChannels(IID_LinxDioChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_GET_AI_CHANS: // Get AI Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of analog input channel identifiers
			dataLength = EnumerateChannels(IID_LinxAiChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_GET_AO_CHANS: // Get AO Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of analog output channel identifiers
			dataLength = EnumerateChannels(IID_LinxAoChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_GET_PWM_CHANS: // Get PWM Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of pwm channel identifiers
			dataLength = EnumerateChannels(IID_LinxPwmChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_GET_QE_CHANS: // Get QE Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of quadrature encoder channel identifiers
			dataLength = EnumerateChannels(IID_LinxPwmChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_GET_UART_CHANS: // Get UART Channels
			// Command parameters
			//  None
			//  Response parameters
			//  uint8[] : array of UART channel identifiers
			// or
			//  Response parameters
			//  uint8 : numChans
			//  uint8[] : array of num Chans UART channel identifiers
			//  uint8[] : numChans of Pascal strings
			dataLength = EnumerateChannels(IID_LinxUartChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_GET_I2C_CHANS: // Get I2C Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of I2C channel identifiers
			dataLength = EnumerateChannels(IID_LinxI2cChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_GET_SPI_CHANS: // Get SPI Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of SPI channel identifiers
			dataLength = EnumerateChannels(IID_LinxSpiChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_GET_CAN_CHANS: // Get CAN Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of CAN channel identifiers
			dataLength = EnumerateChannels(IID_LinxCanChannel, m_ProtocolVersion, packetBuffer, offset, bufferLength); 
			status = L_OK;
			break;

		case LCMD_DISCONNECT: // Disconnect
			// Command parameters
			// None
			// Response parameters
			// None
			m_Debug->Writeln("Remote Close Command");
			status = L_DISCONNECT;
			dataLength = 0;
			break;

		case LCMD_SET_DEVICE_USER_ID: //Set Device User Id
			// Command parameters
			// uint16 : device user id
			// Response parameters
			// None
			if (dataLength >= 2)
			{
				m_LinxDev->userId = GetU16FromBuff(packetBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_USERID, packetBuffer[offset]);
				m_LinxDev->NonVolatileWrite(NVS_USERID + 1, packetBuffer[offset + 1]);
				status = L_OK;
			}
			dataLength = 0;
			break;

		case LCMD_GET_DEVICE_USER_ID: //Get Device User Id
			// Command parameters
			// None
			// Response parameters
			// uint16 : device user id
			if (bufferLength >= offset + 1)
			{
				WriteU16ToBuff(packetBuffer, offset - 1, m_LinxDev->userId);
				status = L_OK;
				dataLength = 2;
				break;
			}
			dataLength = 0;
			break;


		case LCMD_SET_ETH_ADDR: //Set Device Ethernet IP
			// Command parameters
			// uint32 : ethernet IP
			// Response parameters
			// None
			if (dataLength >= 4)
			{
				m_LinxDev->ethernetIp = GetU32FromBuff(packetBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 0, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 1, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 2, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 3, packetBuffer[offset++]);
				status = L_OK;
			}
			dataLength = 0;
			break;

		case LCMD_GET_ETH_ADDR: //Get Device Ethernet IP
			// Command parameters
			// None
			// Response parameters
			// uint32 : ethernet IP
			if (bufferLength >= offset + 3)
			{
				WriteU32ToBuff(packetBuffer, offset - 1, m_LinxDev->ethernetIp);
				status = L_OK;
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_SET_ETH_PORT: //Set Device Ethernet Port
			// Command parameters
			// uint16 : ethernet port
			// Response parameters
			// None
			if (dataLength >= 2)
			{
				m_LinxDev->ethernetPort = GetU16FromBuff(packetBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_PORT + 0, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_PORT + 1, packetBuffer[offset++]);
				status = L_OK;
			}
			dataLength = 0;
			break;

		case LCMD_GET_ETH_PORT: //Get Device Ethernet Port
			// Command parameters
			// None
			// Response parameters
			// uint16 : ethernet port
			if (bufferLength >= offset + 1)
			{
				WriteU16ToBuff(packetBuffer, offset - 1, m_LinxDev->ethernetPort);
				status = L_OK;
				dataLength = 2;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_SET_WIFI_ADDR: //Set Device WIFI IP
			// Command parameters
			// uint32 : WIFI IP
			// Response parameters
			// None
			if (dataLength >= 4)
			{
				m_LinxDev->WifiIp = GetU32FromBuff(packetBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_IP + 0, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_IP + 1, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_IP + 2, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_IP + 3, packetBuffer[offset++]);
				status = L_OK;
			}
			dataLength = 0;
			break;

		case LCMD_GET_WIFI_ADDR: //Get Device WIFI IP
			// Command parameters
			// None
			// Response parameters
			// uint32 : WIFI IP
			if (bufferLength >= offset + 3)
			{
				WriteU32ToBuff(packetBuffer, offset - 1, m_LinxDev->WifiIp);
				status = L_OK;
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_SET_WIFI_PORT: //Set Device WIFI Port
			// Command parameters
			// uint16 : WIFI port
			// Response parameters
			// None
			if (dataLength >= 2)
			{
				m_LinxDev->WifiPort = GetU16FromBuff(packetBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_PORT + 0, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_PORT + 1, packetBuffer[offset++]);
				status = L_OK;
			}
			dataLength = 0;
			break;

		case LCMD_GET_WIFI_PORT: //Get Device WIFI Port
			// Command parameters
			// None
			// Response parameters
			// uint16 : WIFI port
			if (bufferLength >= offset + 1)
			{
				WriteU16ToBuff(packetBuffer, offset - 1, m_LinxDev->WifiPort);
				status = L_OK;
				dataLength = 2;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_SET_WIFI_SSID: //Set Device WIFI SSID
			// Command parameters
			// uint8 : len
			// uint8[len] : WIFI SSID
			// Response parameters
			// None
			if (dataLength > packetBuffer[offset])
			{
				uint8_t length = packetBuffer[offset++];
				//Update Ssid Size In RAM And NVS
				if (length > 32)
				{
					length = 32;
				}
				m_LinxDev->WifiSsidSize = length;
				m_LinxDev->NonVolatileWrite(NVS_WIFI_SSID_SIZE, length);

				//Update SSID Value In RAM And NVS
				for (int32_t i = 0; i < length; i++, offset++)
				{
					m_LinxDev->WifiSsid[i] = packetBuffer[offset];
					m_LinxDev->NonVolatileWrite(NVS_WIFI_SSID + i, packetBuffer[offset]);
				}
				status = L_OK;
			}
			dataLength = 0;
			break;

		case LCMD_GET_WIFI_SSID: //Get Device WIFI SSID
			// Command parameters
			// None
			// Response parameters
			// uint8 : len
			// uint8[len] : WIFI SSID
			packetBuffer[offset - 1] = m_LinxDev->WifiSsidSize;	//SSID SIZE
			for (int32_t i = 0; i < m_LinxDev->WifiSsidSize; i++, offset++)
			{
				packetBuffer[offset] = m_LinxDev->WifiSsid[i];
			}
			status = L_OK;
			dataLength = m_LinxDev->WifiSsidSize;
			break;

		case LCMD_SET_WIFI_SEC: //Set Device WIFI Security Type
			// Command parameters
			// uint8 : WIFI Security Type
			// Response parameters
			// None
			if (dataLength)
			{
				m_LinxDev->WifiSecurity = packetBuffer[offset];
				m_LinxDev->NonVolatileWrite(NVS_WIFI_SECURITY_TYPE, packetBuffer[offset]);
				status = L_OK;
			}
			dataLength = 0;
			break;

		case LCMD_GET_WIFI_SEC: //Get Device WIFI Security Type
			// Command parameters
			// None
			// Response parameters
			// uint8 : WIFI Security Type
			if (bufferLength >= offset)
			{
				packetBuffer[offset - 1] = m_LinxDev->WifiSecurity;
				status = L_OK;
				dataLength = 1;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_SET_WIFI_PASS: //Set Device WIFI Password
			// Command parameters
			// uint8 : len
			// uint8[len] : WIFI SSID
			// Response parameters
			// None
			if (dataLength > packetBuffer[offset])
			{
				uint8_t length = packetBuffer[offset++];
				//Update PW Size In RAM And NVS
				if (length > 64)
				{
					length = 64;
				}
				m_LinxDev->WifiPwSize = length;
				m_LinxDev->NonVolatileWrite(NVS_WIFI_PW_SIZE, length);

				//Update PW Value In RAM And NVS
				for (int32_t i = 0; i < length; i++, offset++)
				{
					m_LinxDev->WifiPw[i] = packetBuffer[offset];
					m_LinxDev->NonVolatileWrite(NVS_WIFI_PW + i, packetBuffer[offset]);
				}
				status = L_OK;
			}
			dataLength = 0;
			break;

		//case 0x0021: //TODO Get Device WIFI Password - Intentionally Not Implemented For Security Reasons.

		case LCMD_SET_DEV_MAX_BAUD: //Set Device Max Baud
			// Command parameters
			// uint32 : Device Max Baud
			// Response parameters
			// None
			if (dataLength >= 4)
			{
				m_LinxDev->UartMaxBaud = GetU32FromBuff(packetBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 0, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 1, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 2, packetBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 3, packetBuffer[offset++]);
				status = L_OK;
			}
			dataLength = 0;
			break;

		case LCMD_GET_DEV_MAX_BAUD: //Get Device Max Baud
			// Command parameters
			// None
			// Response parameters
			// uint32 : Device Max Baud
			if (bufferLength >= offset + 3)
			{
				WriteU32ToBuff(packetBuffer, offset - 1, m_LinxDev->UartMaxBaud);
				status = L_OK;
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_GET_DEV_NAME: // Get Device Name
			// Command parameters
			// None
			// Response parameters
			// uint8[] : Device Name
			dataLength = m_LinxDev->GetDeviceName(packetBuffer + offset - 1, bufferLength - offset); 
			status = L_OK;
			break;

		case LCMD_GET_SERVO_CHANS: // Get Servo Channels
			// Command parameters
			// None
			// Response parameters
			// uint32 : Device Max Baud
			dataLength = m_LinxDev->EnumerateChannels(IID_LinxServoChannel, packetBuffer + offset - 1, bufferLength - offset); 
			status = L_OK;
			break;

		//---0x0026 to 0x003F Reserved---

		/****************************************************************************************
		**  Digital I/O
		****************************************************************************************/
		case LCMD_SET_PIN_MODE: // Set Pin Mode
			// Command parameters
			// uint8 : num channels
			// uint8[numChans] : channels
			// uint8[numChans] : states
			// Response parameters
			// None
			if (dataLength > 2 * (uint32_t)packetBuffer[offset])
			{
				uint8_t numChans = packetBuffer[offset++];
				status = m_LinxDev->DigitalSetState(numChans, packetBuffer + offset, packetBuffer + offset + numChans);
			}
			dataLength = 0;
			break;

		case LCMD_DIGITAL_WRITE: // Digital Write
			// Command parameters
			// uint8 : num channels
			// uint8[numChans] : channels
			// uint8[numChans + 7 / 8] : packed values
			// Response parameters
			// None
			if (dataLength > ((uint32_t)packetBuffer[offset] + ((packetBuffer[offset] + 7) >> 3)))
			{
				uint8_t numChans = packetBuffer[offset++];
				status = m_LinxDev->DigitalWrite(numChans, packetBuffer + offset, packetBuffer + offset + numChans);
			}
			dataLength = 0;
			break;

		case LCMD_DIGITAL_READ: // Digital Read
			// Command parameters
			// uint8[numChans] : channels
			// Response parameters
			// uint8[numChans + 7 / 8] : packed values
			if (dataLength >= 1)
			{
				uint8_t numRespBytes = (dataLength + 7) >> 3;
				status = m_LinxDev->DigitalRead(dataLength, packetBuffer + offset, packetBuffer + offset - 1);
				dataLength = numRespBytes;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_SET_SQUARE_WAVE: //Write Square Wave
			// Command parameters
			// uint8  : channel
			// uint32 : frequency
			// uint32 : duration
			// Response parameters
			// None
			if (dataLength >= 9)
			{
				uint32_t freq, duration;
				ReadU32FromBuff(packetBuffer, offset + 1, &freq);	
				ReadU32FromBuff(packetBuffer, offset + 5, &duration);	
				status = m_LinxDev->DigitalWriteSquareWave(packetBuffer[offset], freq, duration);
			}
			dataLength = 0;
			break;

		case LCMD_GET_PULSE_WIDTH: //Read Pulse Width
			// Command parameters
			// uint8 : response channel
			// uint8 : stimulation channel
			// uint8 : stimulation type
			// uint8 : response type
			// uint32 : timeout
			// Response parameters
			// uint32 : pulse width
			if (dataLength >= 8)
			{
				uint32_t timeout, width;
				ReadU32FromBuff(packetBuffer, offset + 4, &timeout);	

				//LinxDev->DebugPrint("Timeout = ");
				//LinxDev->DebugPrintln(timeout, DEC);

				status = m_LinxDev->DigitalReadPulseWidth(packetBuffer[offset + 1], packetBuffer[offset + 2], packetBuffer[offset], packetBuffer[offset + 3], timeout, &width);
				WriteU32ToBuff(packetBuffer, offset - 1, width);
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		//---0x0045 to 0x005F Reserved---

		/****************************************************************************************
		**  Analog I/O
		****************************************************************************************/
		case LCMD_SET_AI_REF_VOLT: //Set AI Ref Voltage
			// Command parameters
			// uint8 : mode
			// uint32 : reference voltage
			// Response parameters
			// None
			if (dataLength >= 5)
			{
				uint32_t voltage;
				ReadU32FromBuff(packetBuffer, offset + 1, &voltage);	
				status =  m_LinxDev->AnalogSetRef(packetBuffer[offset], voltage);
			}
			dataLength = 0;
			break;

		case LCMD_GET_AI_REF_VOLT: // Get AI Reference Voltage
			// Command parameters
			// None
			// Response parameters
			// uint32 : reference voltage
			if (bufferLength >= offset + 3)
			{
				WriteU32ToBuff(packetBuffer, offset - 1, m_LinxDev->AiRefSet);
				status = L_OK;
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		//case LCMD_SET_AI_RESOLUTION;	// Set AI Resolution

		case LCMD_GET_AI_RESOLUTION:	// Get AI Resolution
			// Command parameters
			// None
			// Response parameters
			// uint8 : analog input resolution
			if (bufferLength >= offset + 3)
			{
				packetBuffer[offset - 1] = m_LinxDev->AiResolution;
				status = L_OK;
				dataLength = 1;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_ANALOG_READ: // Analog Read
			// Command parameters
			// uint8[length] : channels
			// Response parameters
			// uint8 : analog input resolution
			// uint8[] : analog values, bit-packed with resolution bits per channel
			if (dataLength >= bufferLength)
			{
				status = m_LinxDev->AnalogRead(dataLength, packetBuffer + offset, packetBuffer + offset);
				packetBuffer[offset - 1] = m_LinxDev->AiResolution;
				dataLength = ((dataLength * m_LinxDev->AiResolution + 7) / 8);
				break;
			}
			dataLength = 0;
			break;

		//case LCMD_GET_AO_RESOLUTION: // Get AO Resolution

		case LCMD_GET_AO_RESOLUTION: // Get AO Resolution
			// Command parameters
			// None
			// Response parameters
			// uint8 : analog output resolution
			if (bufferLength >= offset)
			{
				packetBuffer[offset - 1] = m_LinxDev->AoResolution;
				status = L_OK;
				dataLength = 1;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_ANALOG_WRITE: // Analog Write
			// Command parameters
			// uint8 : number of channels
			// uint8 : analog output resolution
			// uint8[numChans] : channels
			// uint8[] : analog values, bit-packed with resolution bits per channel
			// Response parameters
			// None
			if (dataLength >= (2 + (uint32_t)packetBuffer[offset] + (((uint32_t)packetBuffer[offset] * m_LinxDev->AoResolution + 7) / 8)) &&
				packetBuffer[offset + 1] == m_LinxDev->AoResolution)
			{
				uint8_t numChans = packetBuffer[offset];
				status = m_LinxDev->AnalogWrite(numChans, packetBuffer + offset + 2, packetBuffer + offset + 2 + numChans);
			}
			dataLength = 0;
			break;

		//---0x0066 to 0x007F Reserved---

		/****************************************************************************************
		** PWM
		****************************************************************************************/

		//case LCMD_PWM_OPEN: //TODO PWM Open
		//case LCMD_SET_PWM_MODE: //TODO PWM Set Mode
		//case LCMD_SET_PWN_FREQ: //TODO PWM Set Frequency

		case LCMD_SET_PWM_DUTY_CYCLE: //PWM Set Duty Cycle
			// Command parameters
			// uint8 : number of channels
			// uint8[numChans] : channels
			// uint8[numChans] : pwm values
			// Response parameters
			// None
			if (dataLength > 2 * (uint32_t)packetBuffer[offset])
			{
				uint8_t numChans = packetBuffer[offset++];
				status = m_LinxDev->PwmSetDutyCycle(numChans, packetBuffer + offset, packetBuffer + offset + numChans);
			}
			dataLength = 0;
			break;

		//case LCMD_PWM_CLOSE: //TODO PWM Close

		/****************************************************************************************
		** QE
		****************************************************************************************/

		//---0x00A0 to 0x00BF Reserved---


		/****************************************************************************************
		** UART
		****************************************************************************************/
		case LCMD_UART_OPEN: // UART Open
			// Command parameters
			// uint8 : channel
			// uint32 : baudrate
			// Response parameters
			// uint32 : actual baudrate
			if (dataLength >= 5)
			{
				status = m_LinxDev->UartOpen(packetBuffer[offset]);
				if (!status)
				{
					uint32_t actualBaud = 0;
					status = m_LinxDev->UartSetBaudRate(packetBuffer[offset], GetU32FromBuff(packetBuffer, offset + 1), &actualBaud);
					WriteU32ToBuff(packetBuffer, offset - 1, actualBaud); 
					dataLength = 4;
					break;
				}
			}
			dataLength = 0;
			break;

		case LCMD_UART_OPEN_BY_NAME: // UART Open by Name
			// Command parameters
			// uint8[] : device name
			// Response parameters
			// uint8 : assigned channel
			if (dataLength >= bufferLength - offset)
			{
				packetBuffer[offset + dataLength] = 0;
				status = m_LinxDev->UartOpen(packetBuffer + offset, packetBuffer + offset - 1);
				dataLength = 1;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_UART_SET_BAUD: // UART Set Baud Rate
			// Command parameters
			// uint8 : channel
			// uint32 : baudrate
			// Response parameters
			// uint32 : actual baudrate
			if (dataLength >= 5)
			{
				uint32_t actualBaud = 0;
				status = m_LinxDev->UartSetBaudRate(packetBuffer[offset], GetU32FromBuff(packetBuffer, offset + 1), &actualBaud);
				WriteU32ToBuff(packetBuffer, offset - 1, actualBaud);
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_UART_SET_ATTRIBUTE: // UART Set Parameters
			// Command parameters
			// uint8 : channel
			// uint8 : data bits
			// uint8 : stop bits
			// uint8 : parity
			// Response parameters
			// None
			if (dataLength >= 4)
			{
				status = m_LinxDev->UartSetParameters(packetBuffer[offset], packetBuffer[offset + 1], packetBuffer[offset + 2], (LinxUartParity)packetBuffer[offset + 3]);
			}
			dataLength = 0;
			break;
		case LCMD_UART_GET_BYTES: // UART Get Bytes Available
			// Command parameters
			// uint8 : channel
			// Response parameters
			//  uint8 : available bytes
			// or
			//  uint32 : available bytes
			if (dataLength >= 1)
			{
				uint32_t numBytes;
				status = m_LinxDev->UartGetBytesAvailable(packetBuffer[offset], &numBytes);
				if (!status)
				{
					if (m_ProtocolVersion)
					{
						WriteU32ToBuff(packetBuffer, offset - 1, numBytes);
						dataLength = 4;
					}
					else
					{
						WriteU8ToBuff(packetBuffer, offset - 1, numBytes);
						dataLength = 1;
					}
				}
				break;
			}
			dataLength = 0;
			break;

		case LCMD_UART_READ: // UART Read
			// Command parameters
			// Either:
            //  uint8 : channel
			//  uint8 : bytes to read
			// or
            //  uint8 : 0
            //  uint8 : channel
			//  int32 : timeout
			//  int32 : bytes to read
			// end
			// Response parameters
			// uint8[] : read bytes
			if (dataLength >= 2)
			{
				uint8_t channel = packetBuffer[offset];
				int32_t timeout = TIMEOUT_INFINITE;
				uint32_t numBytes = packetBuffer[offset + 1];
				if (dataLength >= 10)
				{
					channel = packetBuffer[offset + 1];
					timeout = GetU32FromBuff(packetBuffer, offset + 2);
					numBytes = GetU32FromBuff(packetBuffer, offset + 6);
				}
				if (numBytes > (uint32_t)(bufferLength - offset))
					numBytes = bufferLength - offset;

				status = m_LinxDev->UartRead(channel, numBytes, packetBuffer + offset - 1, timeout, &numBytes);
				dataLength = numBytes;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_UART_WRITE: // UART Write
			// Command parameters
			// Either:
            //  uint8 : channel
			// or
			//  uint8 : 0
			//  uint8 : channel
			//  int32 : timeout
			//  int32 : number of bytes
			// end
			// uint8[] : bytes to write
			// Response parameters
			// None
			if (dataLength >= 1)
			{
				uint8_t channel = packetBuffer[offset++];
				uint32_t numBytes = dataLength - 1;
				int32_t timeout = TIMEOUT_INFINITE;
				if (!channel && dataLength > 10)
				{
					channel = packetBuffer[offset];
					timeout = GetU32FromBuff(packetBuffer, offset + 1);
					numBytes = dataLength - 10;
					numBytes = Min(numBytes, GetU32FromBuff(packetBuffer, offset + 5));
					offset += 9;
				}
				status = m_LinxDev->UartWrite(channel, numBytes, packetBuffer + offset, timeout);
			}
			dataLength = 0;
			break;

		case LCMD_UART_CLOSE: // UART Close
			// Command parameters
			// uint8 : channel
			// Response parameters
			// None
			if (dataLength >= 1)
			{
				status = m_LinxDev->UartClose(packetBuffer[offset]);
			}
			dataLength = 0;
			break;

		//---0x00C6 to 0x00DF Reserved---

		/****************************************************************************************
		** I2C
		****************************************************************************************/
		case LCMD_I2C_OPEN: // I2C Open Master
			// Command parameters
			// uint8 : channel
			// Response parameters
			// None
			if (dataLength >= 1)
			{
				status = m_LinxDev->I2cOpenMaster(packetBuffer[offset]);
			}
			dataLength = 0;
			break;

		case LCMD_I2C_SPEED: // I2C Set Speed
			// Command parameters
			// uint8 : channel
			// uint32 : speed
			// Response parameters
			// uint32 : actual speed
			if (dataLength >= 5)
			{
				uint32_t actualSpeed = 0;
				status = m_LinxDev->I2cSetSpeed(packetBuffer[offset], GetU32FromBuff(packetBuffer, offset + 1), &actualSpeed);

				//Build Response Packet
				WriteU32ToBuff(packetBuffer, offset - 1, actualSpeed);
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_I2C_WRITE: // I2C Write
			// Command parameters
			// uint8 : I2C channel
			// uint8 : I2C slave address
			// uint8 : I2C eof config
			// uint8[] : data bytes to write
			// Response parameters
			// None
			if (dataLength >= 3)
			{
				status = m_LinxDev->I2cWrite(packetBuffer[offset], packetBuffer[offset + 1], packetBuffer[offset + 2], (uint8_t)dataLength - 3, packetBuffer + offset + 3);
			}
			dataLength = 0;
			break;

		case LCMD_I2C_READ: // I2C Read
			// Command parameters
			// uint8 : I2C channel
			// uint8 : I2C slave address
			// uint8 : num bytes to read
			// uint16 : timeout
			// uint8 : I2C eof config
			// Response parameters
			// uint8[] : num bytes data
			if (dataLength >= 6)
			{
				status = m_LinxDev->I2cRead(packetBuffer[offset], packetBuffer[offset + 1], packetBuffer[offset + 5], packetBuffer[offset + 2], GetU16FromBuff(packetBuffer, offset + 3), packetBuffer + offset - 1);
				dataLength = packetBuffer[offset + 2];
				break;
			}
			dataLength = 0;
			break;

		case LCMD_I2C_CLOSE: // I2C Close
			// Command parameters
			// uint8 : I2C channel
			// Response parameters
			// None
			if (dataLength >= 1)
			{
				status = m_LinxDev->I2cClose((packetBuffer[offset]));
			}
			dataLength = 0;
			break;

		//---0x00E5 to 0x00FF Reserved---

		/****************************************************************************************
		** SPI
		****************************************************************************************/
		case LCMD_SPI_OPEN: // SPI Open Master
			// Command parameters
			// uint8 : SPI channel
			// Response parameters
			// None
			if (dataLength >= 1)
			{
				status = m_LinxDev->SpiOpenMaster(packetBuffer[offset]);
			}
			dataLength = 0;
			break;

		case LCMD_SPI_SET_ORDER: // SPI Set Bit Order
			// Command parameters
			// uint8 : SPI channel
			// uint8 : SPI bit order
			// Response parameters
			// None
			if (dataLength >= 2)
			{
				status = m_LinxDev->SpiSetBitOrder(packetBuffer[offset], packetBuffer[offset + 1]);
			}
			dataLength = 0;
			break;

		case LCMD_SPI_SET_FREQ: // SPI Set Clock Frequency
			// Command parameters
			// uint8 : SPI channel
			// uint32 : clock frequency
			// Response parameters
			// None
			if (dataLength >= 5)
			{
				uint32_t actualSpeed = 0;
				status = m_LinxDev->SpiSetSpeed(packetBuffer[offset], GetU32FromBuff(packetBuffer, offset + 1), &actualSpeed);

				//Build Response Packet
				WriteU32ToBuff(packetBuffer, offset - 1, actualSpeed);
				dataLength = 4;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_SPI_SET_MODE: // SPI Set Mode
			// Command parameters
			// uint8 : SPI channel
			// uint8 : SPI mode
			// Response parameters
			// None
			if (dataLength >= 2)
			{
				status = m_LinxDev->SpiSetMode(packetBuffer[offset], packetBuffer[offset + 1]);
			}
			dataLength = 0;
			break;

		//case 0x0104: //LEGACY - SPI Set Frame Size
		//case 0x0105: //LEGACY - SPI Set CS Logic Level
		//case 0x0106: //LEGACY - SPI Set CS Channel

		case LCMD_SPI_TRANSFER: // SPI Write Read
			// Command parameters
			// uint8 : SPI channel
			// uint8 : frame size
			// uint8 : CS digital channel
			// uint8 : CS Logic Level
			// uint8[] : num frames * frame size data bytes 
			// Response parameters
			// uint8[] : num frames * frame size data bytes
			if (dataLength > 4)
			{
				uint8_t frameSize = packetBuffer[offset + 1], 
						numFrames = (dataLength - 4) / frameSize,

				status = m_LinxDev->SpiWriteRead(packetBuffer[offset], frameSize, numFrames, packetBuffer[offset + 2], packetBuffer[offset + 3], packetBuffer + offset + 4, packetBuffer + offset - 1);
				dataLength -= 5;
				break;
			}
			dataLength = 0;
			break;

		case LCMD_SPI_CLOSE: // SPI Close
			if (dataLength >= 1)
			{
				status = m_LinxDev->SpiCloseMaster(packetBuffer[offset]);
			}
			dataLength = 0;
			break;

		//---0x0085 to 0x009F Reserved---

		/****************************************************************************************
		** CAN
		****************************************************************************************/

		//---0x0120 to 0x013F Reserved---

		/****************************************************************************************
		** SERVO
		****************************************************************************************/
		case LCMD_SERVO_INIT: // Servo Init
			//LinxDev->DebugPrintln("Opening Servo");
			status = m_LinxDev->ServoOpen((uint8_t)dataLength, packetBuffer + offset);
			dataLength = 0;
			//LinxDev->DebugPrintln("Done Creating Servos...");
			break;

		case LCMD_SERVO_SET_PULSE: // Servo Set Pulse Width
			if (dataLength >= 1)
			{
				uint8_t numChans = packetBuffer[offset];
				// Convert Big Endian packet to platform endianess (uC)
				// Temporary in place array pointer to store endianess corrected value
				uint8_t *tempPtr = packetBuffer + offset + 1 + numChans;
				uint16_t *tempVals = (uint16_t *)tempPtr;

				for (int32_t i = 0; i < numChans; i++)
				{
					// Create uint16_t from big endian byte array
					tempVals[i] = GetU16FromBuff(tempPtr, 2 * i);			
				}

				//TODO REMOVE DEBUG PRINT
				m_Debug->Writeln("::tempVals::");
				for (int32_t i = 0; i < numChans; i++)
				{
					m_Debug->Writeln(tempVals[i], DEC);
				}
				status = m_LinxDev->ServoSetPulseWidth(numChans, packetBuffer + offset + 1, tempVals);
			}
			dataLength = 0;
			break;

		case LCMD_SERVE_CLOSE: // Servo Close
			status = m_LinxDev->ServoClose((uint8_t)dataLength, packetBuffer + offset);
			dataLength = 0;
			break;

		/****************************************************************************************
		** WS2812
		****************************************************************************************/
		case 0x0160: // WS2812 Open
			status = m_LinxDev->Ws2812Open(GetU16FromBuff(packetBuffer, offset), packetBuffer[offset + 2]);
			dataLength = 0;
			break;
		case 0x0161: // WS2812 Write One Pixel
			status = m_LinxDev->Ws2812WriteOnePixel(GetU16FromBuff(packetBuffer, offset), packetBuffer[offset + 2], packetBuffer[offset + 3], packetBuffer[offset + 4], packetBuffer[offset + 5]);
			dataLength = 0;
			break;
		case 0x0162: // WS2812 Write N Pixels
			status = m_LinxDev->Ws2812WriteNPixels(GetU16FromBuff(packetBuffer, offset), GetU16FromBuff(packetBuffer, offset), packetBuffer + offset + 5, packetBuffer[offset + 4]);
			dataLength = 0;
			break;
		case 0x0163: // WS2812 Refresh
			status = m_LinxDev->Ws2812Refresh();
			dataLength = 0;
			break;
		case 0x0164: // WS2812 Close
			status = m_LinxDev->Ws2812Close();
			dataLength = 0;
			break;

		/****************************************************************************************
		** Default
		****************************************************************************************/
		default: //Default Case
			dataLength = 0;
			status = L_FUNCTION_NOT_SUPPORTED;
			break;
		}
	}
	return PacketizeAndSend(packetBuffer, dataLength, status);
}
