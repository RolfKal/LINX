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
		LinxBeagleBoneBlack();
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

		virtual int UartOpen(unsigned char channel, unsigned int baudRate, unsigned int* actualBaud);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		const char* overlaySlotsPath;		// The overlay slot export path
		string DtoSlotsPath;				// Path to device tree overlay slots file.  Varies by OS version.
		int FilePathLayout;					// Used to indicate the file path layout 7 for 7.x and 8 for 8.x
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		bool loadDto(const char* dtoName);				
};

#endif //LINX_BEAGLEBONEBLACK_H
