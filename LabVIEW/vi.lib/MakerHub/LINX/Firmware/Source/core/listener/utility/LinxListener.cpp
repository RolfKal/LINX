/****************************************************************************************
**  Generic LINX Listener code.  This file contains the parent class for LINX listeners.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Sam Kristoff
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
LinxListener::LinxListener()
{
	m_LinxDev = NULL;
	m_LinxDebug = NULL;
	m_PeriodicTask = NULL;
	for (int i = 0; i < MAX_CUSTOM_CMDS; i++)
	{
		m_CustomCommands[i] = NULL;
	}
	m_ListenerBufferSize = 0;
	m_SendBuffer = NULL;
	m_RecBuffer = NULL;
}

LinxListener::~LinxListener()
{
	free(m_SendBuffer);
	free(m_RecBuffer);
}

/****************************************************************************************
** Public Functions
****************************************************************************************/
int LinxListener::Start(int bufferSize)	// Start Listener with the device to relay commands to and a debug channel
{
	int status = L_OK;
	if (m_ListenerBufferSize != bufferSize)
	{
		m_ListenerBufferSize = bufferSize;
		m_SendBuffer = (unsigned char*)realloc(m_SendBuffer, bufferSize);
		m_RecBuffer = (unsigned char*)realloc(m_RecBuffer, bufferSize);
	}
	return status;
}

int LinxListener::CheckForCommand()
{
	int dataRead = 0;
	// Try to read first 4 bytes
	int status = ReadData(m_SendBuffer, 4, &dataRead);
	if (!status)
	{
		unsigned short command;
		int offset, msgLength, length = dataRead;
		// Decode length in package
		if (m_SendBuffer[0] == 0xFF && dataRead >= 2)
		{
			offset = 4;
			msgLength = m_SendBuffer[1];
		}
		else if (m_SendBuffer[0] == 0xFE && dataRead >= 4)
		{
			offset = 6;
			msgLength = GetU32FromBuff(m_SendBuffer, 0) & 0xFFFFFF;
		}
		else
		{
			// invalid data frame, flush buffer and return
			return FlushData();
		}
		
		// if expected msgLength is greater than the data already received then read the remainder
		while (msgLength > length)
		{
			dataRead = 0;
			status = ReadData(m_SendBuffer + dataRead, msgLength - dataRead, &dataRead);
			if (status)
				return status;

			length += dataRead;
		}

		if (m_LinxDev->ChecksumPassed(m_SendBuffer, length - 1))
		{
			return LERR_CHECKSUM;
		}
		offset = ReadU16FromBuff(m_SendBuffer, offset, &command);
		length -= offset; // Remove header
		status = ProcessCommand(m_SendBuffer, offset, length, command, m_RecBuffer);
		if (m_PeriodicTask)
			status = m_PeriodicTask(m_SendBuffer, m_RecBuffer);
	}
	return status;
}

int LinxListener::AttachCustomCommand(unsigned short commandNumber, CustomCommand callback)
{
	if (commandNumber < MAX_CUSTOM_CMDS)
	{
		m_CustomCommands[commandNumber] = callback;
		return L_OK;
	}
	return LERR_BADPARAM;
}

int LinxListener::AttachPeriodicTask(PeriodicTask task)
{
	m_PeriodicTask = task;
	return L_OK;
}

/****************************************************************************************
** Private Functions
****************************************************************************************/
int LinxListener::PacketizeAndSend(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, int dataSize,  int status)
{
	int offset = 2;

	// Fill in packet size
	if (commandPacketBuffer[0] == 0xFE)
	{
		offset = WriteU32ToBuff(responsePacketBuffer, 0, dataSize + 8);
	}
	else if (commandPacketBuffer[0] == 0xFE)
	{
		responsePacketBuffer[1] = dataSize + 6;					
	}

	// Set first byte to Start of Frame character
	responsePacketBuffer[0] = commandPacketBuffer[0];

	// Copy packet number from the command into our response
	offset = WriteU16ToBuff(responsePacketBuffer, offset, GetU16FromBuff(commandPacketBuffer, offset));

	// Make Sure Status Is Valid
	if (status >= 0 && status <= 255)
	{
		responsePacketBuffer[offset++] = (unsigned char)status;	//Status
	}
	else
	{
		responsePacketBuffer[offset++] = L_UNKNOWN_ERROR;		//Status
	}
	// Compute and append checksum
	offset += dataSize;
	responsePacketBuffer[offset] = m_LinxDev->ComputeChecksum(responsePacketBuffer, offset);

	// Send it off
	return WriteData(responsePacketBuffer, offset + 1);
}

int LinxListener::ProcessCommand(unsigned char* commandPacketBuffer, int offset, int length, unsigned short command, unsigned char* responsePacketBuffer)
{
	//Store Some Local Values For Convenience
	int status = LERR_BADPARAM;

	if ((command & 0xFF00) == CUSTOM_CMD_PREFIX)
	{
		command &= 0xFF;

		/****************************************************************************************
		** User Commands
		****************************************************************************************/
		if (((command) < MAX_CUSTOM_CMDS) && (m_CustomCommands[command]))
		{
			int responseLength = m_ListenerBufferSize - offset;
			status = m_CustomCommands[command - CUSTOM_CMD_PREFIX](commandPacketBuffer + offset, length - offset, responsePacketBuffer + offset - 1, &responseLength);
			length = responseLength;
		}
		else
		{
			length = 0;
		}
	}
	else		
	{
		//Standard Commands
		switch (command)
		{
		/************************************************************************************
		* SYSTEM COMMANDS
		************************************************************************************/
		case LCMD_SYNC: // Sync Packet
			// Command parameters
			// None
			// Response parameters
			// None
			status = L_OK;
			length = 0;
			break;

		//case 0x0001: //TODO Flush Linx Connection Buffer
		//case 0x0002: //TODO Reset

		case LCMD_GET_DEV_ID: // Get Device ID
			// Command parameters
			// None
			// Response parameters
			// uint8 : device family
			// uint8 : device id
			offset = WriteU8ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->DeviceFamily);
			offset = WriteU8ToBuff(responsePacketBuffer, offset, m_LinxDev->DeviceId);
			status = L_OK;
			length = 2;
			break;

		case LCMD_GET_API_VER: //Get LINX API Version
			// Command parameters
			// None
			// Response parameters
			// uint8 : api major version
			// uint8 : api minor version
			// uint8 : api subminor version
			offset = WriteU8ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->LinxApiMajor);
			offset = WriteU8ToBuff(responsePacketBuffer, offset, m_LinxDev->LinxApiMinor);
			offset = WriteU8ToBuff(responsePacketBuffer, offset, m_LinxDev->LinxApiSubminor);
			status = L_OK;
			length = 3;
			break;

		case LCMD_GET_UART_MAX_BAUD: //Get UART Listener Interface Max Baud
			// Command parameters
			// None
			// Response parameters
			// uint32 : max uart baudrate
			WriteU32ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->UartMaxBaud);
			status = L_OK;
			length = 4;
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
			length = m_LinxDev->GetDioChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_AI_CHANS: // Get AI Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of analog input channel identifiers
			length = m_LinxDev->GetAiChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_AO_CHANS: // Get AO Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of analog output channel identifiers
			length = m_LinxDev->GetAoChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_PWM_CHANS: // Get PWM Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of pwm channel identifiers
			length = m_LinxDev->GetPwmChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_QE_CHANS: // Get QE Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of quadrature encoder channel identifiers
			length = m_LinxDev->GetQeChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_UART_CHANS: // Get UART Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of UART channel identifiers
			length = m_LinxDev->GetUartChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_I2C_CHANS: // Get I2C Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of I2C channel identifiers
			length = m_LinxDev->GetI2cChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_SPI_CHANS: // Get SPI Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of SPI channel identifiers
			length = m_LinxDev->GetSpiChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_CAN_CHANS: // Get CAN Channels
			// Command parameters
			// None
			// Response parameters
			// uint8[] : array of CAN channel identifiers
			length = m_LinxDev->GetCanChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_DISCONNECT: // Disconnect
			// Command parameters
			// None
			// Response parameters
			// None
			m_LinxDev->DebugPrintln("Remote Close Command");
			status = L_DISCONNECT;
			length = 0;
			break;

		case LCMD_SET_DEVICE_USER_ID: //Set Device User Id
			// Command parameters
			// uint16 : device user id
			// Response parameters
			// None
			if (length >= 2)
			{
				m_LinxDev->userId = GetU16FromBuff(commandPacketBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_USERID, commandPacketBuffer[offset]);
				m_LinxDev->NonVolatileWrite(NVS_USERID + 1, commandPacketBuffer[offset + 1]);
				status = L_OK;
			}
			length = 0;
			break;

		case LCMD_GET_DEVICE_USER_ID: //Get Device User Id
			// Command parameters
			// None
			// Response parameters
			// uint16 : device user id
			WriteU16ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->userId);
			status = L_OK;
			length = 2;
			break;


		case LCMD_SET_ETH_ADDR: //Set Device Ethernet IP
			// Command parameters
			// uint32 : ethernet IP
			// Response parameters
			// None
			if (length >= 4)
			{
				m_LinxDev->ethernetIp = GetU32FromBuff(commandPacketBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 0, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 1, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 2, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 3, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			length = 0;
			break;

		case LCMD_GET_ETH_ADDR: //Get Device Ethernet IP
			// Command parameters
			// None
			// Response parameters
			// uint32 : ethernet IP
			WriteU32ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->ethernetIp);
			status = L_OK;
			length = 4;
			break;

		case LCMD_SET_ETH_PORT: //Set Device Ethernet Port
			// Command parameters
			// uint16 : ethernet port
			// Response parameters
			// None
			if (length >= 2)
			{
				m_LinxDev->ethernetPort = GetU16FromBuff(commandPacketBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_PORT + 0, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_ETHERNET_PORT + 1, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			length = 0;
			break;

		case LCMD_GET_ETH_PORT: //Get Device Ethernet Port
			// Command parameters
			// None
			// Response parameters
			// uint16 : ethernet port
			WriteU16ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->ethernetPort);
			status = L_OK;
			length = 2;
			break;

		case LCMD_SET_WIFI_ADDR: //Set Device WIFI IP
			// Command parameters
			// uint32 : WIFI IP
			// Response parameters
			// None
			if (length >= 4)
			{
				m_LinxDev->WifiIp = GetU32FromBuff(commandPacketBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_IP + 0, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_IP + 1, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_IP + 2, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_IP + 3, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			length = 0;
			break;

		case LCMD_GET_WIFI_ADDR: //Get Device WIFI IP
			// Command parameters
			// None
			// Response parameters
			// uint32 : WIFI IP
			WriteU32ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->WifiIp);
			status = L_OK;
			length = 4;
			break;

		case LCMD_SET_WIFI_PORT: //Set Device WIFI Port
			// Command parameters
			// uint16 : WIFI port
			// Response parameters
			// None
			if (length >= 2)
			{
				m_LinxDev->WifiPort = GetU16FromBuff(commandPacketBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_PORT + 0, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_WIFI_PORT + 1, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			length = 0;
			break;

		case LCMD_GET_WIFI_PORT: //Get Device WIFI Port
			// Command parameters
			// None
			// Response parameters
			// uint16 : WIFI port
			WriteU16ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->WifiPort);
			status = L_OK;
			length = 2;
			break;

		case LCMD_SET_WIFI_SSID: //Set Device WIFI SSID
			// Command parameters
			// uint8 : len
			// uint8[len] : WIFI SSID
			// Response parameters
			// None
			if (length > commandPacketBuffer[offset])
			{
				unsigned char len = commandPacketBuffer[offset++];
				//Update Ssid Size In RAM And NVS
				if (len > 32)
				{
					len = 32;
				}
				m_LinxDev->WifiSsidSize = len;
				m_LinxDev->NonVolatileWrite(NVS_WIFI_SSID_SIZE, len);

				//Update SSID Value In RAM And NVS
				for (int i = 0; i < len; i++, offset++)
				{
					m_LinxDev->WifiSsid[i] = commandPacketBuffer[offset];
					m_LinxDev->NonVolatileWrite(NVS_WIFI_SSID + i, commandPacketBuffer[offset]);
				}
				status = L_OK;
			}
			length = 0;
			break;

		case LCMD_GET_WIFI_SSID: //Get Device WIFI SSID
			// Command parameters
			// None
			// Response parameters
			// uint8 : len
			// uint8[len] : WIFI SSID
			responsePacketBuffer[offset - 1] = m_LinxDev->WifiSsidSize;	//SSID SIZE
			for (int i = 0; i < m_LinxDev->WifiSsidSize; i++)
			{
				responsePacketBuffer[offset + i] = m_LinxDev->WifiSsid[i];
			}
			status = L_OK;
			length = m_LinxDev->WifiSsidSize;
			break;

		case LCMD_SET_WIFI_SEC: //Set Device WIFI Security Type
			// Command parameters
			// uint8 : WIFI Security Type
			// Response parameters
			// None
			if (length)
			{
				m_LinxDev->WifiSecurity = commandPacketBuffer[offset];
				m_LinxDev->NonVolatileWrite(NVS_WIFI_SECURITY_TYPE, commandPacketBuffer[offset]);
				status = L_OK;
			}
			length = 0;
			break;

		case LCMD_GET_WIFI_SEC: //Get Device WIFI Security Type
			// Command parameters
			// None
			// Response parameters
			// uint8 : WIFI Security Type
			responsePacketBuffer[offset - 1] = m_LinxDev->WifiSecurity;
			status = L_OK;
			length = 1;
			break;

		case LCMD_SET_WIFI_PASS: //Set Device WIFI Password
			// Command parameters
			// uint8 : len
			// uint8[len] : WIFI SSID
			// Response parameters
			// None
			if (length > commandPacketBuffer[offset])
			{
				unsigned char len = commandPacketBuffer[offset++];
				//Update PW Size In RAM And NVS
				if (len > 64)
				{
					len = 64;
				}
				m_LinxDev->WifiPwSize = len;
				m_LinxDev->NonVolatileWrite(NVS_WIFI_PW_SIZE, len);

				//Update PW Value In RAM And NVS
				for (int i = 0; i < len; i++, offset++)
				{
					m_LinxDev->WifiPw[i] = commandPacketBuffer[offset];
					m_LinxDev->NonVolatileWrite(NVS_WIFI_PW + i, commandPacketBuffer[offset]);
				}
				status = L_OK;
			}
			length = 0;
			break;

		//case 0x0021: //TODO Get Device WIFI Password - Intentionally Not Implemented For Security Reasons.

		case LCMD_SET_DEV_MAX_BAUD: //Set Device Max Baud
			// Command parameters
			// uint32 : Device Max Baud
			// Response parameters
			// None
			if (length >= 4)
			{
				m_LinxDev->serialInterfaceMaxBaud = GetU32FromBuff(commandPacketBuffer, offset);
				m_LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 0, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 1, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 2, commandPacketBuffer[offset++]);
				m_LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 3, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			length = 0;
			break;

		case LCMD_GET_DEV_MAX_BAUD: //Get Device Max Baud
			// Command parameters
			// None
			// Response parameters
			// uint32 : Device Max Baud
			WriteU32ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->serialInterfaceMaxBaud);
			status = L_OK;
			break;

		case LCMD_GET_DEV_NAME: // Get Device Name
			// Command parameters
			// None
			// Response parameters
			// uint8[] : Device Name
			length = m_LinxDev->GetDeviceName(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		case LCMD_GET_SERVO_CHANS: // Get Servo Channels
			// Command parameters
			// None
			// Response parameters
			// uint32 : Device Max Baud
			length = m_LinxDev->GetServoChans(responsePacketBuffer + offset - 1, m_ListenerBufferSize - offset); 
			status = L_OK;
			break;

		//---0x0026 to 0x003F Reserved---

		/****************************************************************************************
		**  Digital I/O
		****************************************************************************************/
		//case LCMD_SET_PIN_MODE: //TODO Set Pin Mode

		case LCMD_DIGITAL_WRITE: // Digital Write
			// Command parameters
			// uint8 : num channels
			// uint8[numChans] : channels
			// uint8[numChans + 7 / 8] : packed values
			// Response parameters
			// None
			if (length > (commandPacketBuffer[offset] + ((commandPacketBuffer[offset] + 7) >> 3)))
			{
				int numChans = commandPacketBuffer[offset];
				status = m_LinxDev->DigitalWrite(numChans, commandPacketBuffer + offset + 1, commandPacketBuffer + offset + 1 + numChans);
			}
			length = 0;
			break;

		case LCMD_DIGITAL_READ: // Digital Read
			// Command parameters
			// uint8[numChans] : channels
			// Response parameters
			// uint8[numChans + 7 / 8] : packed values
			if (length >= 1)
			{
				unsigned char numRespBytes = (length + 7) >> 3;
				status = m_LinxDev->DigitalRead(length, commandPacketBuffer + offset, responsePacketBuffer + offset - 1);
				length = numRespBytes;
			}
			else
			{
				length = 0;
			}
			break;

		case LCMD_SET_SQUARE_WAVE: //Write Square Wave
			// Command parameters
			// uint32 : frequency
			// uint32 : duration
			// Response parameters
			// None
			if (length >= 8)
			{
				unsigned int freq, duration;
				ReadU32FromBuff(commandPacketBuffer, offset + 1, &freq);	
				ReadU32FromBuff(commandPacketBuffer, offset + 5, &duration);	
				status = m_LinxDev->DigitalWriteSquareWave(commandPacketBuffer[offset], freq, duration);
			}
			length = 0;
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
			if (length >= 8)
			{
				unsigned int timeout, width;
				ReadU32FromBuff(commandPacketBuffer, offset + 4, &timeout);	

				//LinxDev->DebugPrint("Timeout = ");
				//LinxDev->DebugPrintln(timeout, DEC);

				status = m_LinxDev->DigitalReadPulseWidth(commandPacketBuffer[offset + 1], commandPacketBuffer[offset + 2], commandPacketBuffer[offset], commandPacketBuffer[offset + 3], timeout, &width);
				WriteU32ToBuff(responsePacketBuffer, offset - 1, width);
				length = 4;
			}
			else
			{
				length = 0;
			}
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
			if (length >= 5)
			{
				unsigned int voltage;
				ReadU32FromBuff(commandPacketBuffer, offset + 1, &voltage);	
				status =  m_LinxDev->AnalogSetRef(commandPacketBuffer[offset], voltage);
			}
			length = 0;
			break;

		case LCMD_GET_ÀI_REF_VOLT: // Get AI Reference Voltage
			// Command parameters
			// None
			// Response parameters
			// uint32 : reference voltage
			WriteU32ToBuff(responsePacketBuffer, offset - 1, m_LinxDev->AiRefSet);
			status = L_OK;
			length = 4;
			break;

		//case LCMD_SET_AI_RESOLUTION;	// Get AI Resolution

		case LCMD_GET_AI_RESOLUTION:	// Get AI Resolution
			// Command parameters
			// None
			// Response parameters
			// uint8 : analog input resolution
			responsePacketBuffer[offset - 1] = m_LinxDev->AiResolution;
			status = L_OK;
			length = 1;
			break;

		case LCMD_ANALOG_READ: // Analog Read
			// Command parameters
			// uint8[length] : channels
			// Response parameters
			// uint8 : analog input resolution
			// uint8[] : analog values, bit-packed with resolution bits per channel
			responsePacketBuffer[offset - 1] = m_LinxDev->AiResolution;
			status = m_LinxDev->AnalogRead(length, commandPacketBuffer + offset, responsePacketBuffer + offset);
			length = ((length * m_LinxDev->AiResolution + 7) / 8);
			break;

		//case LCMD_GET_AO_RESOLUTION: // Get AO Resolution

		case LCMD_GET_AO_RESOLUTION: // Get AO Resolution
			// Command parameters
			// None
			// Response parameters
			// uint8 : analog output resolution
			responsePacketBuffer[offset - 1] = m_LinxDev->AoResolution;
			status = L_OK;
			length = 1;
			break;

		case LCMD_ANALOG_WRITE: // Analog Write
			// Command parameters
			// uint8 : number of channels
			// uint8 : analog output resolution
			// uint8[numChans] : channels
			// uint8[] : analog values, bit-packed with resolution bits per channel
			// Response parameters
			// None
			if (length >= (2 + commandPacketBuffer[offset] + ((commandPacketBuffer[offset] * m_LinxDev->AoResolution + 7) / 8)) &&
				commandPacketBuffer[offset + 1] == m_LinxDev->AoResolution)
			{
				unsigned char numChans = commandPacketBuffer[offset];
				status = m_LinxDev->AnalogWrite(numChans, commandPacketBuffer + offset + 2, commandPacketBuffer + offset + 2 + numChans);
			}
			length = 0;
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
			if (length > 2 * (int)commandPacketBuffer[offset])
			{
				unsigned char numChans = commandPacketBuffer[offset];
				status = m_LinxDev->PwmSetDutyCycle(numChans, commandPacketBuffer + offset + 1, commandPacketBuffer + offset + 1 + numChans);
			}
			length = 0;
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
			// optional uint8 : dataBits
			// optional uint8 : stopBits
			// optional uint8 : parity
			// Response parameters
			// uint32 : actual baudrate
			if (length >= 5)
			{
				unsigned char channel = commandPacketBuffer[offset];
				unsigned int actualBaud = 0, baudrate = GetU32FromBuff(commandPacketBuffer, offset + 1);
				if (length >= 8)
				{
					unsigned char dataBits = commandPacketBuffer[offset + 5];
					unsigned char stopBits = commandPacketBuffer[offset + 6];
					LinxUartParity parity = (LinxUartParity)commandPacketBuffer[offset + 7];
					status = m_LinxDev->UartOpen(channel, baudrate, &actualBaud, dataBits, stopBits, parity);
				}
				else
				{
					status = m_LinxDev->UartOpen(channel, baudrate, &actualBaud);
				}
				WriteU32ToBuff(responsePacketBuffer, offset - 1, actualBaud); 
				length = 4;
			}
			else
			{
				length = 0;
			}
			break;

		case LCMD_UART_SET_BAUD: // UART Set Buad Rate
			// Command parameters
			// uint8 : channel
			// uint32 : baudrate
			// Response parameters
			// uint32 : actual baudrate
			if (length >= 5)
			{
				unsigned int actualBaud = 0;
				status = m_LinxDev->UartSetBaudRate(commandPacketBuffer[offset], GetU32FromBuff(commandPacketBuffer, offset + 1), &actualBaud);
				WriteU32ToBuff(responsePacketBuffer, 5, actualBaud);
				length = 4;
			}
			else
			{
				length = 0;
			}
			break;

		case LCMD_UART_GET_BYTES: // UART Get Bytes Available
			// Command parameters
			// uint8 : channel
			// Response parameters
			// uint32 : available bytes
			if (length >= 1)
			{
				status = m_LinxDev->UartGetBytesAvailable(commandPacketBuffer[offset], responsePacketBuffer + offset - 1);
				length = 1;
			}
			else
			{
				length = 0;
			}
			break;

		case LCMD_UART_READ: // UART Read
			// Command parameters
			// uint8 : channel
			// uint8 : bytes to read
			// Response parameters
			// uint8[] : read bytes
			if (length >= 2)
			{
				unsigned char numBytesRead = 0;
				status = m_LinxDev->UartRead(commandPacketBuffer[offset], commandPacketBuffer[offset + 1], responsePacketBuffer + offset - 1, &numBytesRead);
				length = numBytesRead;
			}
			else
			{
				length = 0;
			}
			break;

		case LCMD_UART_WRITE: // UART Write
			// Command parameters
			// uint8 : channel
			// uint8[] : byte to write
			// Response parameters
			// None
			if (length >= 1)
			{
				status = m_LinxDev->UartWrite(commandPacketBuffer[offset], (unsigned char)length - 1, commandPacketBuffer + offset + 1);
			}
			length = 0;
			break;

		case LCMD_UART_CLOSE: // UART Close
			// Command parameters
			// uint8 : channel
			// Response parameters
			// None
			if (length >= 1)
			{
				status = m_LinxDev->UartClose(commandPacketBuffer[offset]);
			}
			length = 0;
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
			if (length >= 1)
			{
				status = m_LinxDev->I2cOpenMaster(commandPacketBuffer[offset]);
			}
			length = 0;
			break;

		case LCMD_I2C_SPEED: // I2C Set Speed
			// Command parameters
			// uint8 : channel
			// uint32 : speed
			// Response parameters
			// uint32 : actual speed
			if (length >= 5)
			{
				unsigned int actualSpeed = 0;
				status = m_LinxDev->I2cSetSpeed(commandPacketBuffer[offset], GetU32FromBuff(commandPacketBuffer, offset + 1), &actualSpeed);

				//Build Response Packet
				WriteU32ToBuff(responsePacketBuffer, offset - 1, actualSpeed);
				length = 4;
			}
			else
			{
				length = 0;
			}
			break;

		case LCMD_I2C_WRITE: // I2C Write
			// Command parameters
			// uint8 : I2C channel
			// uint8 : I2C slave address
			// uint8 : I2C eof config
			// uint8[] : data bytes to write
			// Response parameters
			// None
			if (length >= 3)
			{
				status = m_LinxDev->I2cWrite(commandPacketBuffer[offset], commandPacketBuffer[offset + 1], commandPacketBuffer[offset + 2], (unsigned char)length - 3, commandPacketBuffer + offset + 3);
			}
			length = 0;
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
			if (length >= 6)
			{
				status = m_LinxDev->I2cRead(commandPacketBuffer[offset], commandPacketBuffer[offset + 1], commandPacketBuffer[offset + 5], commandPacketBuffer[offset + 2], GetU16FromBuff(commandPacketBuffer, offset + 3), responsePacketBuffer + offset - 1);
				length = commandPacketBuffer[offset + 2];
			}
			else
			{
				length = 0;
			}
			break;

		case 0x00E4: // I2C Close
			// Command parameters
			// uint8 : I2C channel
			// Response parameters
			// None
			if (length >= 1)
			{
				status = m_LinxDev->I2cClose((commandPacketBuffer[offset]));
			}
			length = 0;
			break;

		//---0x00E5 to 0x00FF Reserved---

		/****************************************************************************************
		** SPI
		****************************************************************************************/
		case 0x0100: // SPI Open Master
			// Command parameters
			// uint8 : SPI channel
			// Response parameters
			// None
			if (length >= 1)
			{
				status = m_LinxDev->SpiOpenMaster(commandPacketBuffer[offset]);
			}
			length = 0;
			break;
		case 0x0101: // SPI Set Bit Order
			// Command parameters
			// uint8 : SPI channel
			// uint8 : SPI bit order
			// Response parameters
			// None
			if (length >= 2)
			{
				status = m_LinxDev->SpiSetBitOrder(commandPacketBuffer[offset], commandPacketBuffer[offset + 1]);
			}
			length = 0;
			break;

		case 0x0102: // SPI Set Clock Frequency
			// Command parameters
			// uint8 : SPI channel
			// uint32 : clock frequency
			// Response parameters
			// None
			if (length >= 5)
			{
				unsigned int actualSpeed = 0;
				status = m_LinxDev->SpiSetSpeed(commandPacketBuffer[offset], GetU32FromBuff(commandPacketBuffer, offset + 1), &actualSpeed);

				//Build Response Packet
				WriteU32ToBuff(responsePacketBuffer, offset - 1, actualSpeed);
				length = 4;
			}
			else
			{
				length = 0;
			}
			break;

		case 0x0103: // SPI Set Mode
			// Command parameters
			// uint8 : SPI channel
			// uint8 : SPI mode
			// Response parameters
			// None
			if (length >= 2)
			{
				status = m_LinxDev->SpiSetMode(commandPacketBuffer[offset], commandPacketBuffer[offset + 1]);
			}
			length = 0;
			break;

		//case 0x0104: //LEGACY - SPI Set Frame Size
		//case 0x0105: //LEGACY - SPI Set CS Logic Level
		//case 0x0106: //LEGACY - SPI Set CS Channel

		case 0x0107: // SPI Write Read
			// Command parameters
			// uint8 : SPI channel
			// uint8 : frame size
			// uint8 : CS digital channel
			// uint8 : CS Logic Level
			// uint8[] : num frames * frame size data bytes 
			// Response parameters
			// uint8[] : num frames * frame size data bytes
			if (length > 4)
			{
				unsigned char frameSize = commandPacketBuffer[offset + 1], 
							  numFrames = (length - 4) / frameSize,

				status = m_LinxDev->SpiWriteRead(commandPacketBuffer[offset], frameSize, numFrames, commandPacketBuffer[offset + 2], commandPacketBuffer[offset + 3], commandPacketBuffer + offset + 4, responsePacketBuffer + offset - 1);
				length -= 5;
			}
			else
			{
				length = 0;
			}
			break;

		case 0x0108: // SPI Close
			if (length >= 1)
			{
				status = m_LinxDev->SpiCloseMaster(commandPacketBuffer[offset]);
			}
			length = 0;
			break;

		//---0x0085 to 0x009F Reserved---

		/****************************************************************************************
		** CAN
		****************************************************************************************/

		//---0x0120 to 0x013F Reserved---

		/****************************************************************************************
		** SERVO
		****************************************************************************************/
		case 0x0140: // Servo Init
			//LinxDev->DebugPrintln("Opening Servo");
			status = m_LinxDev->ServoOpen((unsigned char)length, commandPacketBuffer + offset);
			length = 0;
			//LinxDev->DebugPrintln("Done Creating Servos...");
			break;

		case 0x0141: // Servo Set Pulse Width
			if (length >= 1)
			{
				unsigned char numChans = commandPacketBuffer[offset];
				// Convert Big Endian packet to platform endianess (uC)
				// Temporary in place array pointer to store endianess corrected value
				unsigned char *tempPtr = commandPacketBuffer + offset + 1 + numChans;
				unsigned short *tempVals = (unsigned short *)tempPtr;

				for (int i = 0; i < numChans; i++)
				{
					// Create unsigned short from big endian byte array
					tempVals[i] = GetU16FromBuff(tempPtr, 2 * i);			
				}

				//TODO REMOVE DEBUG PRINT
				m_LinxDev->DebugPrintln("::tempVals::");
				for (int i = 0; i < numChans; i++)
				{
					m_LinxDev->DebugPrintln(tempVals[i], DEC);
				}
				status = m_LinxDev->ServoSetPulseWidth(numChans, commandPacketBuffer + offset + 1, tempVals);
			}
			length = 0;
			break;

		case 0x0142: // Servo Close
			status = m_LinxDev->ServoClose((unsigned char)length, commandPacketBuffer + offset);
			length = 0;
			break;

		/****************************************************************************************
		** WS2812
		****************************************************************************************/
		case 0x0160: // WS2812 Open
			status = m_LinxDev->Ws2812Open(GetU16FromBuff(commandPacketBuffer, offset), commandPacketBuffer[offset + 2]);
			length = 0;
			break;
		case 0x0161: // WS2812 Write One Pixel
			status = m_LinxDev->Ws2812WriteOnePixel(GetU16FromBuff(commandPacketBuffer, offset), commandPacketBuffer[offset + 2], commandPacketBuffer[offset + 3], commandPacketBuffer[offset + 4], commandPacketBuffer[offset + 5]);
			length = 0;
			break;
		case 0x0162: // WS2812 Write N Pixels
			status = m_LinxDev->Ws2812WriteNPixels(GetU16FromBuff(commandPacketBuffer, offset), GetU16FromBuff(commandPacketBuffer, offset), commandPacketBuffer + offset + 5, commandPacketBuffer[offset + 4]);
			length = 0;
			break;
		case 0x0163: // WS2812 Refresh
			status = m_LinxDev->Ws2812Refresh();
			length = 0;
			break;
		case 0x0164: // WS2812 Close
			status = m_LinxDev->Ws2812Close();
			length = 0;
			break;

		/****************************************************************************************
		** Default
		****************************************************************************************/
		default: //Default Case
			length = 0;
			status = L_FUNCTION_NOT_SUPPORTED;
			break;
		}
	}
	return PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, status);
}
