/****************************************************************************************
**  LINX header for Linux support functions used in Beaglebone Black and Raspberry Pi Device
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermater based on code from Sam Kristoff
**
** BSD2 License.
****************************************************************************************/

#ifndef LINX_LINUXDEVICE_H
#define LINX_LINUXDEVICE_H


/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stdio.h>
#include <string>
#include <map>
#include "LinxDefines.h"
#include "LinxDevice.h"

/****************************************************************************************
**  Typedefs
****************************************************************************************/

/****************************************************************************************
**  Variables
****************************************************************************************/

class LinxLinuxDevice : public LinxDevice
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/

		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		virtual ~LinxLinuxDevice() {};

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		//UART
		virtual int UartOpen(const char *deviceName, unsigned char *channel, LinxUartChannel **chan = NULL);

		// General
		virtual unsigned char EnumerateChannels(int type, unsigned char *buffer, unsigned char length);

	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		//System

		//DIO

		//PWM
		std::map<unsigned char, std::string> PwmDirPaths;		//PWM Device Tree Overlay Names
		std::map<unsigned char, FILE*> PwmPeriodHandles;		//File Handles For PWM Period Values
		std::map<unsigned char, FILE*> PwmDutyCycleHandles;		//File Handles For PWM Duty Cycle Values
		std::map<unsigned char, unsigned long> PwmPeriods;		//Current PWM  Values
		unsigned int PwmDefaultPeriod;							//Default Period For PWM Channels (nS)
		std::string PwmDutyCycleFileName;
		std::string PwmPeriodFileName;
		std::string PwmEnableFileName;

		//AI
//		unsigned char NumAiRefIntVals;							// Number Of Internal AI Reference Voltages
//		const unsigned long* AiRefIntVals;						// Supported AI Reference Voltages (uV)
//		const int* AiRefCodes;									// AI Ref Values (AI Ref Macros In Wiring Case)
//		unsigned int AiRefExtMin;								// Min External AI Ref Value (uV)
//		unsigned int AiRefExtMax;					   			// Max External AI Ref Value (uV)

		//AO
//		std::map<unsigned char, FILE*> AoValueHandles;			// AO Value Handles

		//UART
//		unsigned char NumUartSpeeds;							//Number Of Support UART Buads

		//SPI
//		unsigned int SpiDefaultSpeed;

		//I2C

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual unsigned char RegisterChannel(int type, LinxChannel *chan);
		virtual LinxChannel* LookupChannel(int type, unsigned char channel);
		virtual void RegisterChannel(int type, unsigned char channel, LinxChannel *chan);
		virtual void RemoveChannel(int type, unsigned char channel);
		virtual void ClearChannels(int type);

		virtual int pwmSmartOpen(unsigned char numChans, unsigned char* channels);
		bool uartSupportsVarBaudrate(const char* path, int baudrate);

	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		std::map<unsigned char, LinxChannel*> m_ChannelRegistry[LinxNumChanelTypes];

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_LINUXDEVICE_H