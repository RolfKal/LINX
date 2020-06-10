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
#include "LinxListener.h"
#include "LinxCommand.h"
#include "LinxDevice.h"
#include "LinxUtilities.h"

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxListener::LinxListener()
{
	State = START;
}

/****************************************************************************************
** Functions
****************************************************************************************/

int LinxListener::Start()
{
	return -1;
}

int LinxListener::Listen()
{
	return -1;
}

int LinxListener::Available()
{
	return -1;
}

int LinxListener::Accept()
{
	return -1;
}
int LinxListener::Connected()
{
	return -1;
}

int LinxListener::Close()
{
	return -1;
}

int LinxListener::Exit()
{
	return -1;
}

int LinxListener::CheckForCommands()
{
	switch (State)
	{
		case START:
			Start();
			break;
		case CONNECTED:
			Connected();
			break;
		case CLOSE:
			Close();
			break;
		case EXIT:
			Exit();
			break;
		default:
			return LUNKNOWN_STATE;
			break;
	}
	return L_OK;
}

unsigned char LinxListener::ComputeChecksum(unsigned char* packetBuffer)
{
	unsigned char checksum = 0;

	//Sum All Bytes In The Packet Except The Last (Checksum Byte)
	for (int i = 0; i < (packetBuffer[1] - 1); i++)
	{
		checksum += packetBuffer[i];
	}
	return checksum;
}


bool LinxListener::ChecksumPassed(unsigned char* packetBuffer)
{
	return (ComputeChecksum(packetBuffer) == packetBuffer[packetBuffer[1] - 1]);
}


void LinxListener::StatusResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, int status)
{
	PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 0, status);
}


int LinxListener::ProcessCommand(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
	//Store Some Local Values For Convenience
	int status = L_OK;
	unsigned short command;
	unsigned int length, offset;
	if (commandPacketBuffer[0] == 0xFF)
	{
		offset = 4;
		length = commandPacketBuffer[1];
	}
	else if (commandPacketBuffer[0] == 0xFE)
	{
		offset = 7;
		ReadU32FromBuff(commandPacketBuffer, 1, &length);
	}
	length -= 1;      // Remove Checksum byte
	offset = ReadU16FromBuff(commandPacketBuffer, offset, &command);
	length -= offset; // Remove header

	/****************************************************************************************
	** User Commands
	****************************************************************************************/
	if (command >= 0xFC00)
	{
		unsigned char numResponseBytes = 0;
		status = customCommands[command - 0xFC00]((unsigned char)length, commandPacketBuffer + offset, &numResponseBytes, responsePacketBuffer + offset - 1);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, numResponseBytes, status);
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
			StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
			break;

		//case 0x0001: //TODO Flush Linx Connection Buffer
		//case 0x0002: //TODO Reset

		case LCMD_GET_DEV_ID: // Get Device ID
			offset = WriteU8ToBuff(responsePacketBuffer, offset - 1, LinxDev->DeviceFamily);
			offset = WriteU8ToBuff(responsePacketBuffer, offset, LinxDev->DeviceId);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK);
			break;

		case LCMD_GET_API_VER: //Get LINX API Version
			offset = WriteU8ToBuff(responsePacketBuffer, offset - 1, LinxDev->LinxApiMajor);
			offset = WriteU8ToBuff(responsePacketBuffer, offset, LinxDev->LinxApiMinor);
			offset = WriteU8ToBuff(responsePacketBuffer, offset, LinxDev->LinxApiSubminor);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 3, L_OK);
			break;

		case LCMD_GET_UART_MAX_BAUD: //Get UART Listener Interface Max Baud
			WriteU32ToBuff(responsePacketBuffer, offset - 1, LinxDev->UartMaxBaud);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
			break;

		case LCMD_SET_UART_MAX_BAUD: //Set UART Listener Interface Max Baud
			if (length >= 4)
			{
				unsigned int actualBaud = 0;
				status = LinxDev->UartSetBaudRate(ListenerChan, GetU32FromBuff(commandPacketBuffer, offset), &actualBaud);
				LinxDev->DelayMs(1000);
				WriteU32ToBuff(responsePacketBuffer, offset - 1, actualBaud);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status);
			}
			else
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			break;

		case LCMD_GET_MAX_PACK_SIZE: // Get Max Packet Size
			WriteU32ToBuff(responsePacketBuffer, offset - 1, ListenerBufferSize);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
			break;

		case LCMD_GET_DIO_CHANS: // Get DIO Channels
		{
			length = LinxDev->GetDioChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;
		}
		case LCMD_GET_AI_CHANS: // Get AI Channels
			length = LinxDev->GetAiChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case LCMD_GET_AO_CHANS: // Get AO Channels
			length = LinxDev->GetAoChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case LCMD_GET_PWM_CHANS: // Get PWM Channels
			length = LinxDev->GetPwmChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case LCMD_GET_QE_CHANS: // Get QE Channels
			length = LinxDev->GetQeChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case LCMD_GET_UART_CHANS: // Get UART Channels
			length = LinxDev->GetUartChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case LCMD_GET_I2C_CHANS: // Get I2C Channels
			length = LinxDev->GetI2cChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case LCMD_GET_SPI_CHANS: // Get SPI Channels
			length = LinxDev->GetSpiChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case LCMD_GET_CAN_CHANS: // Get CAN Channels
			length = LinxDev->GetCanChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case LCMD_DISCONNECT: // Disconnect
			LinxDev->DebugPrintln("Close Command");
			status = L_DISCONNECT;
			StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
			break;

		case LCMD_SET_DEVICE_USER_ID: //Set Device User Id
			if (length >= 2)
			{
				LinxDev->userId = GetU16FromBuff(commandPacketBuffer, offset);
				LinxDev->NonVolatileWrite(NVS_USERID, commandPacketBuffer[offset]);
				LinxDev->NonVolatileWrite(NVS_USERID + 1, commandPacketBuffer[offset + 1]);
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case LCMD_GET_DEVICE_USER_ID: //Get Device User Id
			WriteU16ToBuff(responsePacketBuffer, offset - 1, LinxDev->userId);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK);
			break;


		case LCMD_SET_ETH_ADDR: //Set Device Ethernet IP
			if (length >= 4)
			{
				LinxDev->ethernetIp = GetU32FromBuff(commandPacketBuffer, offset);
				LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 0, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 1, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 2, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_ETHERNET_IP + 3, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case LCMD_GET_ETH_ADDR: //Get Device Ethernet IP
			WriteU32ToBuff(responsePacketBuffer, offset - 1, LinxDev->ethernetIp);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
			break;

		case LCMD_SET_ETH_PORT: //Set Device Ethernet Port
			if (length >= 2)
			{
				LinxDev->ethernetPort = GetU16FromBuff(commandPacketBuffer, offset);
				LinxDev->NonVolatileWrite(NVS_ETHERNET_PORT + 0, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_ETHERNET_PORT + 1, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case LCMD_GET_ETH_PORT: //Get Device Ethernet Port
			WriteU16ToBuff(responsePacketBuffer, offset - 1, LinxDev->ethernetPort);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK);
			break;

		case 0x0018: //Set Device WIFI IP
			if (length >= 4)
			{
				LinxDev->WifiIp = GetU32FromBuff(commandPacketBuffer, offset);
				LinxDev->NonVolatileWrite(NVS_WIFI_IP + 0, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_WIFI_IP + 1, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_WIFI_IP + 2, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_WIFI_IP + 3, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x0019: //Get Device WIFI IP
			WriteU32ToBuff(responsePacketBuffer, offset - 1, LinxDev->WifiIp);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
			break;

		case 0x001A: //Set Device WIFI Port
			if (length >= 2)
			{
				LinxDev->WifiPort = GetU16FromBuff(commandPacketBuffer, offset);
				LinxDev->NonVolatileWrite(NVS_WIFI_PORT + 0, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_WIFI_PORT + 1, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x001B: //Get Device WIFI Port
			WriteU16ToBuff(responsePacketBuffer, offset - 1, LinxDev->WifiPort);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK);
			break;

		case 0x001C: //Set Device WIFI SSID
			if (length > commandPacketBuffer[offset])
			{
				unsigned char len = commandPacketBuffer[offset],
				              *ptr = commandPacketBuffer + offset + 1;
				//Update Ssid Size In RAM And NVS
				if (len > 32)
				{
					LinxDev->WifiSsidSize = 32;
					LinxDev->NonVolatileWrite(NVS_WIFI_SSID_SIZE, 32);
				}
				else
				{
					LinxDev->WifiSsidSize = len;
					LinxDev->NonVolatileWrite(NVS_WIFI_SSID_SIZE, len);
				}

				//Update SSID Value In RAM And NVS
				for (int i = 0; i < LinxDev->WifiSsidSize; i++)
				{
					LinxDev->WifiSsid[i] = ptr[i];
					LinxDev->NonVolatileWrite(NVS_WIFI_SSID + i, ptr[i]);
				}
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x001D: //Get Device WIFI SSID
			responsePacketBuffer[offset - 1] = LinxDev->WifiSsidSize;	//SSID SIZE
			for (int i = 0; i < LinxDev->WifiSsidSize; i++)
			{
				responsePacketBuffer[offset + i] = LinxDev->WifiSsid[i];
			}
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, LinxDev->WifiSsidSize, L_OK);
			break;

		case 0x001E: //Set Device WIFI Security Type
			if (length)
			{
				LinxDev->WifiSecurity = commandPacketBuffer[offset];
				LinxDev->NonVolatileWrite(NVS_WIFI_SECURITY_TYPE, commandPacketBuffer[offset]);
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x001F: //Get Device WIFI Security Type
			responsePacketBuffer[offset - 1] = LinxDev->WifiSecurity;
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 1, L_OK);
			break;

		case 0x0020: //Set Device WIFI Password
			if (length > commandPacketBuffer[offset])
			{
				unsigned char len = commandPacketBuffer[offset],
				              *ptr = commandPacketBuffer + offset + 1;
				//Update PW Size In RAM And NVS
				if (len > 64)
				{
					LinxDev->WifiPwSize = 64;
					LinxDev->NonVolatileWrite(NVS_WIFI_PW_SIZE, 64);
				}
				else
				{
					LinxDev->WifiPwSize = len;
					LinxDev->NonVolatileWrite(NVS_WIFI_PW_SIZE, len);
				}

				//Update PW Value In RAM And NVS
				for (int i = 0; i < LinxDev->WifiPwSize; i++)
				{
					LinxDev->WifiPw[i] = ptr[i];
					LinxDev->NonVolatileWrite(NVS_WIFI_PW + i, ptr[i]);
				}
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		//case 0x0021: //TODO Get Device WIFI Password - Intentionally Not Implemented For Security Reasons.

		case 0x0022: //Set Device Max Baud
			if (length >= 4)
			{
				LinxDev->serialInterfaceMaxBaud = GetU32FromBuff(commandPacketBuffer, offset);
				LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 0, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 1, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 2, commandPacketBuffer[offset++]);
				LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD + 3, commandPacketBuffer[offset++]);
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;
		case 0x0023: //Get Device Max Baud
			WriteU32ToBuff(responsePacketBuffer, offset - 1, LinxDev->serialInterfaceMaxBaud);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
			break;

		case 0x0024: // Get Device Name
			length = LinxDev->GetDeviceName(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		case 0x0025: // Get Servo Channels
			length = LinxDev->GetServoChans(responsePacketBuffer + offset - 1, ListenerBufferSize - offset); 
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length, L_OK);
			break;

		//---0x0026 to 0x003F Reserved---

		/****************************************************************************************
		**  Digital I/O
		****************************************************************************************/
		//case 0x0040: //TODO Set Pin Mode

		case LCMD_DIGITAL_WRITE: // Digital Write
			if (length > 2 * (unsigned int)commandPacketBuffer[offset])
			{
				int numChans = commandPacketBuffer[offset];
				status = LinxDev->DigitalWrite(numChans, commandPacketBuffer + offset + 1, commandPacketBuffer + offset + 1 + numChans);
				status = L_OK;
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case LCMD_DIGITAL_READ: // Digital Read
			if (length >= 1)
			{
				unsigned char numRespBytes = ((length - 1) >> 3) + 1;
				status = LinxDev->DigitalRead(length, commandPacketBuffer + offset, responsePacketBuffer + offset - 1);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, numRespBytes, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
			}
			break;

		case LCMD_SET_SQUARE_WAVE: //Write Square Wave
			if (length >= 8)
			{
				unsigned int freq, duration;
				ReadU32FromBuff(commandPacketBuffer, offset + 1, &freq);	
				ReadU32FromBuff(commandPacketBuffer, offset + 5, &duration);	
				status = LinxDev->DigitalWriteSquareWave(commandPacketBuffer[offset], freq, duration);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case LCMD_GET_PULSE_WIDTH: //Read Pulse Width
			if (length >= 8)
			{
				unsigned int timeout, width;
				ReadU32FromBuff(commandPacketBuffer, offset + 4, &timeout);	

				//LinxDev->DebugPrint("Timeout = ");
				//LinxDev->DebugPrintln(timeout, DEC);

				status = LinxDev->DigitalReadPulseWidth(commandPacketBuffer[offset + 1], commandPacketBuffer[offset + 2], commandPacketBuffer[offset], commandPacketBuffer[offset + 3], timeout, &width);
				WriteU32ToBuff(responsePacketBuffer, offset - 1, width);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		//---0x0045 to 0x005F Reserved---

		/****************************************************************************************
		**  Analog I/O
		****************************************************************************************/
		case 0x0060: //Set AI Ref Voltage
			if (length >= 5)
			{
				unsigned int voltage;
				ReadU32FromBuff(commandPacketBuffer, offset + 1, &voltage);	
				status =  LinxDev->AnalogSetRef(commandPacketBuffer[offset], voltage);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x0061: // Get AI Reference Voltage
			WriteU32ToBuff(responsePacketBuffer, offset - 1, LinxDev->AiRefSet);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
			break;

		//case 0x0062: //TODO Set AI Resolution
		//case 0x0063: //TODO Get AI Resolution

		case 0x0064: // Analog Read
			responsePacketBuffer[offset - 1] = LinxDev->AiResolution;
			status = LinxDev->AnalogRead(length, commandPacketBuffer + offset, responsePacketBuffer + offset);
			PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, (((length * LinxDev->AiResolution) + 7) >> 3) + 1, status);
			break;

		//case 0x0065: //TODO Analog Write

		//---0x0066 to 0x007F Reserved---

		/****************************************************************************************
		** PWM
		****************************************************************************************/

		//case 0x0080: //TODO PWM Open
		//case 0x0081: //TODO PWM Set Mode
		//case 0x0082: //TODO PWM Set Frequency

		case 0x0083: //PWM Set Duty Cycle
			if (length > 2 * (unsigned int)commandPacketBuffer[offset])
			{
				unsigned char numChans = commandPacketBuffer[offset];
				status = LinxDev->PwmSetDutyCycle(numChans, commandPacketBuffer + offset + 1, commandPacketBuffer + offset + 1 + numChans);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		//case 0x0084: //TODO PWM Close

		/****************************************************************************************
		** QE
		****************************************************************************************/

		//---0x00A0 to 0x00BF Reserved---


		/****************************************************************************************
		** UART
		****************************************************************************************/
		case 0x00C0: // UART Open
			if (length >= 5)
			{
				unsigned char command = commandPacketBuffer[offset];
				unsigned int actualBaud = 0, baudrate = GetU32FromBuff(commandPacketBuffer, offset + 1);
				if (length >= 8)
				{
					unsigned char dataBits = commandPacketBuffer[offset + 5];
					unsigned char stopBits = commandPacketBuffer[offset + 6];
					LinxUartParity parity = (LinxUartParity)commandPacketBuffer[offset + 7];
					status = LinxDev->UartOpen(command, baudrate, &actualBaud, dataBits, stopBits, parity);
				}
				else
				{
					status = LinxDev->UartOpen(command, baudrate, &actualBaud);
				}
				WriteU32ToBuff(responsePacketBuffer, offset - 1, actualBaud); 
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		case 0x00C1: // UART Set Buad Rate
			if (length >= 5)
			{
				unsigned int actualBaud = 0;
				status = LinxDev->UartSetBaudRate(commandPacketBuffer[offset], GetU32FromBuff(commandPacketBuffer, offset + 1), &actualBaud);
				WriteU32ToBuff(responsePacketBuffer, 5, actualBaud);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		case 0x00C2: // UART Get Bytes Available
			if (length >= 1)
			{
				status = LinxDev->UartGetBytesAvailable(commandPacketBuffer[offset], responsePacketBuffer + offset - 1);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 1, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		case 0x00C3: // UART Read
			if (length >= 2)
			{
				unsigned char numBytesRead = 0;
				status = LinxDev->UartRead(commandPacketBuffer[offset], commandPacketBuffer[offset + 1], responsePacketBuffer + offset - 1, &numBytesRead);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, numBytesRead, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		case 0x00C4: // UART Write
			if (length >= 1)
			{
				status = LinxDev->UartWrite(commandPacketBuffer[offset], (unsigned char)length - 1, commandPacketBuffer + offset + 1);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x00C5: // UART Close
			if (length >= 1)
			{
				status = LinxDev->UartClose(commandPacketBuffer[offset]);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		//---0x00C6 to 0x00DF Reserved---

		/****************************************************************************************
		** I2C
		****************************************************************************************/
		case 0x00E0: // I2C Open Master
			if (length >= 1)
			{
				status = LinxDev->I2cOpenMaster(commandPacketBuffer[offset]);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x00E1: // I2C Set Speed
			if (length >= 5)
			{
				unsigned int actualSpeed = 0;
				status = LinxDev->I2cSetSpeed(commandPacketBuffer[offset], GetU32FromBuff(commandPacketBuffer, offset + 1), &actualSpeed);

				//Build Response Packet
				WriteU32ToBuff(responsePacketBuffer, offset - 1, actualSpeed);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		case 0x00E2: // I2C Write
			if (length >= 3)
			{
				status = LinxDev->I2cWrite(commandPacketBuffer[offset], commandPacketBuffer[offset + 1], commandPacketBuffer[offset + 2], (unsigned char)length - 3, commandPacketBuffer + offset + 3);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x00E3: // I2C Read
			if (length >= 6)
			{
				status = LinxDev->I2cRead(commandPacketBuffer[offset], commandPacketBuffer[offset + 1], commandPacketBuffer[offset + 5], commandPacketBuffer[offset + 2], GetU16FromBuff(commandPacketBuffer, offset + 3), responsePacketBuffer + offset - 1);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, commandPacketBuffer[offset + 2], status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		case 0x00E4: // I2C Close
			if (length >= 1)
			{
				status = LinxDev->I2cClose((commandPacketBuffer[offset]));
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		//---0x00E5 to 0x00FF Reserved---

		/****************************************************************************************
		** SPI
		****************************************************************************************/
		case 0x0100: // SPI Open Master
			if (length >= 1)
			{
				status = LinxDev->SpiOpenMaster(commandPacketBuffer[offset]);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;
		case 0x0101: // SPI Set Bit Order
			if (length >= 2)
			{
				status = LinxDev->SpiSetBitOrder(commandPacketBuffer[offset], commandPacketBuffer[offset + 1]);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x0102: // SPI Set Clock Frequency
			if (length >= 5)
			{
				unsigned int actualSpeed = 0;
				status = LinxDev->SpiSetSpeed(commandPacketBuffer[offset], GetU32FromBuff(commandPacketBuffer, offset + 1), &actualSpeed);

				//Build Response Packet
				WriteU32ToBuff(responsePacketBuffer, offset - 1, actualSpeed);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		case 0x0103: // SPI Set Mode
			if (length >= 2)
			{
				status = LinxDev->SpiSetMode(commandPacketBuffer[offset], commandPacketBuffer[offset + 1]);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		//case 0x0104: //LEGACY - SPI Set Frame Size
		//case 0x0105: //LEGACY - SPI Set CS Logic Level
		//case 0x0106: //LEGACY - SPI Set CS Channel

		case 0x0107: // SPI Write Read
			if (length >= 4)
			{
				unsigned char frameSize = commandPacketBuffer[offset + 1], 
							  numFrames = (length - 4) / frameSize,

				status = LinxDev->SpiWriteRead(commandPacketBuffer[offset], frameSize, numFrames, commandPacketBuffer[offset + 2], commandPacketBuffer[offset + 3], commandPacketBuffer + offset + 4, responsePacketBuffer + offset - 1);
				PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, length - 5, status);
			}
			else
			{
				StatusResponse(commandPacketBuffer, responsePacketBuffer, LERR_BADPARAM);
			}
			break;

		case 0x0108: // SPI Close
			if (length >= 1)
			{
				status = LinxDev->SpiCloseMaster(commandPacketBuffer[offset]);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
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
			status = LinxDev->ServoOpen((unsigned char)length, commandPacketBuffer + offset);
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
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
				LinxDev->DebugPrintln("::tempVals::");
				for (int i = 0; i < numChans; i++)
				{
					LinxDev->DebugPrintln(tempVals[i], DEC);
				}
				status = LinxDev->ServoSetPulseWidth(numChans, commandPacketBuffer + offset + 1, tempVals);
			}
			else
			{
				status = LERR_BADPARAM;
			}
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		case 0x0142: // Servo Close
			status = LinxDev->ServoClose((unsigned char)length, commandPacketBuffer + offset);
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		/****************************************************************************************
		** WS2812
		****************************************************************************************/
		case 0x0160: // WS2812 Open
			status = LinxDev->Ws2812Open(GetU16FromBuff(commandPacketBuffer, offset), commandPacketBuffer[offset + 2]);
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;
		case 0x0161: // WS2812 Write One Pixel
			status = LinxDev->Ws2812WriteOnePixel(GetU16FromBuff(commandPacketBuffer, offset), commandPacketBuffer[offset + 2], commandPacketBuffer[offset + 3], commandPacketBuffer[offset + 4], commandPacketBuffer[offset + 5]);
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;
		case 0x0162: // WS2812 Write N Pixels
			status = LinxDev->Ws2812WriteNPixels(GetU16FromBuff(commandPacketBuffer, offset), GetU16FromBuff(commandPacketBuffer, offset), commandPacketBuffer + offset + 5, commandPacketBuffer[offset + 4]);
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;
		case 0x0163: // WS2812 Refresh
			status = LinxDev->Ws2812Refresh();
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;
		case 0x0164: // WS2812 Close
			status = LinxDev->Ws2812Close();
			StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
			break;

		/****************************************************************************************
		** Default
		****************************************************************************************/
		default: //Default Case
			StatusResponse(commandPacketBuffer, responsePacketBuffer, (int)L_FUNCTION_NOT_SUPPORTED);
			break;
		}
	}

	return status;
}


void LinxListener::PacketizeAndSend(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, unsigned int dataSize,  int status)
{
	//Load Header
	responsePacketBuffer[0] = 0xFF;						//SoF
	responsePacketBuffer[1] = dataSize + 6;				//PACKET SIZE
	responsePacketBuffer[2] = commandPacketBuffer[2];	//PACKET NUM (MSB)
	responsePacketBuffer[3] = commandPacketBuffer[3];	//PACKET NUM (LSB)
	//Make Sure Status Is Valid
	if (status >= 0 && status <= 255)
	{
		responsePacketBuffer[4] = (unsigned char)status;	//Status
	}
	else
	{
		responsePacketBuffer[4] = (unsigned char)L_UNKNOWN_ERROR;	//Status
	}

	//Compute And Load Checksum
	responsePacketBuffer[dataSize + 5] = ComputeChecksum(responsePacketBuffer);
}

void LinxListener::DataBufferResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, const unsigned char* dataBuffer, unsigned char dataSize, int status)
{

	//Copy Data Into Response Buffer
	for (int i = 0; i < dataSize; i++)
	{
		responsePacketBuffer[i + 5] = dataBuffer[i];
	}
	PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, dataSize, status);
}

void LinxListener::AttachCustomCommand(unsigned short commandNumber, int (*function)(unsigned char, unsigned char*, unsigned char*, unsigned char*) )
{
	customCommands[commandNumber] = function;
}

void LinxListener::AttachPeriodicTask(int (*function)(unsigned char*, unsigned char*))
{
	periodicTasks[0] = function;
}
