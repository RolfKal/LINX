/****************************************************************************************
**  LINX header for BeagleBone Black
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_BEAGLEBONEBLACK_H
#define LINX_BEAGLEBONEBLACK_H

/****************************************************************************************
**  Defines
****************************************************************************************/	
#define DEVICE_NAME_LEN 17

#define NUM_AI_CHANS 7
#define AI_RES_BITS 12
#define AI_REFV 1800000
#define NUM_AI_INT_REFS 0

#define NUM_DIGITAL_CHANS 16

#define NUM_PWM_CHANS 4

#define NUM_SPI_CHANS 1
#define NUM_SPI_SPEEDS 13

#define NUM_I2C_CHANS 1

#define NUM_UART_CHANS 3
#define NUM_UART_SPEEDS 18

#define NUM_SERVO_CHANS 0

/****************************************************************************************
**  Includes
****************************************************************************************/	
#include "LinxDevice.h"
#include "LinxLinuxChannel.h"
#include <map>
#include <string>

class LinxBBBUartChannel : public LinxUnixUartChannel
{
	public:
		/****************************************************************************************
		**  Constructor
		****************************************************************************************/
		LinxBBBUartChannel(LinxFmtChannel *debug, const char *channelName, const char *dtoName, const char *dtoSlotsPath);
		virtual ~LinxBBBUartChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t SmartOpen(void);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		const char *m_DtoName;
		const char *m_DtoSlotsPath;
};

class LinxBBBI2cChannel : public LinxSysfsI2cChannel
{
	public:
		/****************************************************************************************
		**  Constructor
		****************************************************************************************/
		LinxBBBI2cChannel(LinxFmtChannel *debug, const char *channelName, const char *dtoName, const char *dtoSlotsPath);
		virtual ~LinxBBBI2cChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Open(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		const char *m_DtoName;
		const char *m_DtoSlotsPath;
};

class LinxBBBSpiChannel : public LinxSysfsSpiChannel
{
	public:
		/****************************************************************************************
		**  Constructor
		****************************************************************************************/
		LinxBBBSpiChannel(LinxFmtChannel *debug, const char *channelName, LinxDevice *device, uint32_t speed, const char *dtoName, const char *dtoSlotsPath);
		virtual ~LinxBBBSpiChannel(void) {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int32_t Open(void);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		const char *m_DtoName;
		const char *m_DtoSlotsPath;
};

using namespace std;
	
class LinxBeagleBoneBlack : public LinxDevice
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		
		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxBeagleBoneBlack(LinxFmtChannel *debug = NULL);
		virtual ~LinxBeagleBoneBlack(void);
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual uint8_t GetDeviceName(unsigned char *buffer, uint8_t length);

		static bool loadDto(const char *slotsPath, const char* dtoName);				

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		LinxFmtChannel *m_Debug;

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		const char *m_OverlaySlotsPath;		// The overlay slot export path
		const char *m_DtoSlotsPath;			// Path to device tree overlay slots file.  Varies by OS version.
		int32_t m_FilePathLayout;				// Used to indicate the file path layout 7 for 7.x and 8 for 8.x
};

#endif //LINX_BEAGLEBONEBLACK_H
