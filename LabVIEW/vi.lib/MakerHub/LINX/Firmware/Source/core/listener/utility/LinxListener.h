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

class LinxListener
{
	public:	
		/****************************************************************************************
		**  Constructors/Destructors
		****************************************************************************************/
		LinxListener(LinxDevice *deviceL);
		virtual ~LinxListener();

		/****************************************************************************************
		** Functions
		****************************************************************************************/
		// Start Listener with the device to relay commands to
		virtual int Start(LinxCommChannel *channel,		// Channel to use for listening for messages
						  int bufferSize = 255);		// maximum buffer size for messages
		virtual int WaitForConnection() = 0;			// Wait for incoming connection, child needs to implement this
		virtual int CheckForCommand();					// Check for next command and decode it to relay it to the device
		virtual int Close();

		virtual int EnableDebug(LinxCommChannel *chan);

		// Attach a custom command callback function. The class allows up to MAX_CUSTOM_CMDS to be installed and
		// any message with the command word being 0xFCxx whit xx being the command number between 0 and 15 is then
		// forwarded to this callback function to be processed
		int AttachCustomCommand(unsigned short commandNumber, CustomCommand callback);
		int AttachPeriodicTask(PeriodicTask task);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxDevice *m_LinxDev;
		LinxCommChannel *m_Channel;
		LinxFmtChannel *m_Debug;

		/****************************************************************************************
		** Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		CustomCommand m_CustomCommands[MAX_CUSTOM_CMDS];
		PeriodicTask m_PeriodicTask;

		int m_ListenerBufferSize;
		unsigned char* m_RecBuffer;
		unsigned char* m_SendBuffer;

		/****************************************************************************************
		** Functions
		****************************************************************************************/
		int PacketizeAndSend(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, int dataSize, int status);
		int ProcessCommand(unsigned char* commandPacketBuffer, int offset, int length, unsigned short command, unsigned char* responsePacketBuffer);
};
#endif //LINX_LISTENER_H