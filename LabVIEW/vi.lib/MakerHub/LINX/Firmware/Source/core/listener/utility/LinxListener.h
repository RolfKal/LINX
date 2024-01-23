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

/****************************************************************************************
** Defines
****************************************************************************************/
#define MAX_CUSTOM_CMDS 16
#define MAX_PERIODIC_TASKS	1

/****************************************************************************************
** Typedefs
****************************************************************************************/
typedef int (*CustomCommand)(unsigned char* commandPacketBuffer, int length, unsigned char* responsePacketBuffer, int* responseLength);
typedef int (*PeriodicTask)(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer);

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
		int AttachCustomCommand(unsigned short commandNumber, CustomCommand callback);
		int AttachPeriodicTask(PeriodicTask task);

		int ProcessLoop(bool loop = false);
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
		int Run(LinxCommChannel *channel,		// Channel to use for listening for messages
			    int timeout,                    // Communication timeout
				int bufferSize = 255);			// maximum buffer size for messages
		virtual int WaitForConnection(void);	// Wait for incoming connection
		virtual int Close(void);

		int ControlMutex(bool lock);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxCommChannel *m_Channel;
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
		int m_ListenerBufferSize;
		unsigned char* m_RecBuffer;
		unsigned char* m_SendBuffer;
		unsigned char m_ProtocolVersion;
		int m_Run;
		int m_Timeout;

		/****************************************************************************************
		** Functions
		****************************************************************************************/
		int CheckForCommand(void);	// Check for next command and decode it to relay it to the device
		
		int EnumerateChannels(int type, unsigned char request, unsigned char *responseBuffer, unsigned int offset, unsigned int responseLength);
		int PacketizeAndSend(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, int dataSize, int status);
		int ProcessCommand(unsigned short command, unsigned char* commandPacketBuffer, int offset, int length, unsigned char* responsePacketBuffer, int responseLength);
};
#endif //LINX_LISTENER_H