/****************************************************************************************
**  Generic LINX listener header.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_LISTENER_H
#define LINX_LISTENER_H

/****************************************************************************************
** Includes
****************************************************************************************/
#include "LinxDevice.h"
#include "LinxChannel.h"

/****************************************************************************************
** Defines
****************************************************************************************/
#define MAX_CUSTOM_CMDS 16
#define MAX_PERIODIC_TASKS	1

/****************************************************************************************
** Typedefs
****************************************************************************************/
typedef int32_t (*CustomCommand)(uint8_t* commandPacketBuffer, uint32_t length, uint8_t* responsePacketBuffer, uint32_t* responseLength);
typedef int32_t (*PeriodicTask)(uint8_t* commandPacketBuffer, uint8_t* responsePacketBuffer);

class LinxListener : public LinxBase
{
	public:	
		/****************************************************************************************
		**  Constructors/Destructors
		****************************************************************************************/
		LinxListener(LinxDevice *device, bool autoLaunch = false);
		virtual ~LinxListener(void);

		/****************************************************************************************
		** Functions
		****************************************************************************************/
		// Attach a custom command callback function. The class allows up to MAX_CUSTOM_CMDS to be installed and
		// any message with the command word being 0xFCxx whit xx being the command number between 0 and 15 is then
		// forwarded to this callback function to be processed
		int32_t AttachCustomCommand(uint16_t commandNumber, CustomCommand callback);
		int32_t AttachPeriodicTask(PeriodicTask task);

		int32_t ProcessLoop(bool loop = false);
	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxDevice *m_LinxDev;
		LinxFmtChannel *m_Debug;

		/****************************************************************************************
		** Functions
		****************************************************************************************/
		// Start Listener with the device to relay commands to
		int32_t Run(LinxCommChannel *channel,		// Channel to use for listening for messages
			    int32_t timeout,                    // Communication timeout
				int32_t bufferSize = 255);			// maximum buffer size for messages
		virtual int32_t WaitForConnection(void);	// Wait for incoming connection
		virtual int32_t Close(void);

		int32_t ControlMutex(bool lock);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxCommChannel *m_Channel;					// channel to remote device
		CustomCommand m_CustomCommands[MAX_CUSTOM_CMDS];
		PeriodicTask m_PeriodicTask;
		bool m_LaunchThread;
#if Win32
		HANDLE m_Thread;
		CRITICAL_SECTION m_Mutex;
#elif Linux
		pthread_t m_Thread;
		pthread_mutex_t m_Mutex;
#else
#endif
		int32_t m_ListenerBufferSize;
		uint8_t* m_DataBuffer;
		uint8_t m_ProtocolVersion;
		int32_t m_Run;
		int32_t m_Timeout;

		/****************************************************************************************
		** Functions
		****************************************************************************************/
		int32_t CheckForCommand(void);	// Check for next command and decode it to relay it to the device
		
		int32_t EnumerateChannels(int32_t type, uint8_t request, uint8_t *dataBuffer, uint32_t offset, uint32_t bufferLength);
		int32_t PacketizeAndSend(uint8_t* packetBuffer, uint32_t dataLength, int32_t status);
		int32_t ProcessCommand(uint8_t* packetBuffer, uint32_t offset, uint32_t dataLength, uint32_t bufferLength);
};
#endif //LINX_LISTENER_H