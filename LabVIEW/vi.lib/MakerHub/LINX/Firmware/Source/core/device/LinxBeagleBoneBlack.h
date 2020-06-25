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
#include "LinxLinuxDevice.h"
#include <map>
#include <string>

#define IID_LinxBBBUartChannel	64
#define IID_LinxBBBI2cChannel	71
#define IID_LinxBBBSpiChannel	81

class LinxBBBUartChannel : public LinxUnixUartChannel
{
	public:
		/****************************************************************************************
		**  Constructor
		****************************************************************************************/
		LinxBBBUartChannel(const char *channelName, LinxFmtChannel *debug, const char *dtoName, const char *dtoSlotsPath);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

	protected:
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int SmartOpen();

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
		LinxBBBI2cChannel(const char *channelName, LinxFmtChannel *debug, const char *dtoName, const char *dtoSlotsPath);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

		virtual int Open();

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
		LinxBBBSpiChannel(const char *channelName, LinxFmtChannel *debug, LinxLinuxDevice *device, unsigned int speed, const char *dtoName, const char *dtoSlotsPath);

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual LinxChannel *QueryInterface(int interfaceId);

		virtual int Open();

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
	
class LinxBeagleBoneBlack : public LinxLinuxDevice
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		
		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxBeagleBoneBlack(LinxFmtChannel *debug);
		~LinxBeagleBoneBlack();
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char GetDeviceName(unsigned char *buffer, unsigned char length);

		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned long* values);

		virtual int PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);

		virtual int SpiOpenMaster(unsigned char channel);

		virtual int I2cOpenMaster(unsigned char channel);

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
		int m_FilePathLayout;				// Used to indicate the file path layout 7 for 7.x and 8 for 8.x
};

#endif //LINX_BEAGLEBONEBLACK_H
