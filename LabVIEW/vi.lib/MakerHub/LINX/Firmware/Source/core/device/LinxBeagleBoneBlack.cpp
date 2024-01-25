/****************************************************************************************
**  LINX BeagleBone Black Code
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
**  Includes
****************************************************************************************/		
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <string.h>
#include "LinxDefines.h"
#include <unistd.h>	
#include <termios.h>
#include "LinxDevice.h"
#include "LinxUtilities.h"
#include "LinxBeagleBoneBlack.h"

//using namespace std;

/****************************************************************************************
**  Channel implementations
****************************************************************************************/
//System
static const char *m_DeviceName = "BeagleBone Black";

//-------------------------------------- AI -------------------------------------
static const unsigned char g_AiChans[NUM_AI_CHANS] = {0, 1, 2, 3, 4, 5, 6};
static const char *g_AiPaths[NUM_AI_CHANS] = {"/sys/bus/iio/devices/iio:device0/in_voltage0_raw",
											  "/sys/bus/iio/devices/iio:device0/in_voltage1_raw",
											  "/sys/bus/iio/devices/iio:device0/in_voltage2_raw",
											  "/sys/bus/iio/devices/iio:device0/in_voltage3_raw",
											  "/sys/bus/iio/devices/iio:device0/in_voltage4_raw",
											  "/sys/bus/iio/devices/iio:device0/in_voltage5_raw",
											  "/sys/bus/iio/devices/iio:device0/in_voltage6_raw"};
//static const unsigned int m_AiRefIntVals[NUM_AI_INT_REFS] = {};
//static const int m_AiRefCodes[NUM_AI_INT_REFS] = {};

//-------------------------------------- AO -------------------------------------
//None

//-------------------------------------- DIO ------------------------------------
static const unsigned char g_DigitalChans[NUM_DIGITAL_CHANS] = { 7,  8,  9, 10, 11, 12, 15, 16, 17, 18, 26, 58, 61, 69,  73,  76};
static const unsigned char g_gpioChan[NUM_DIGITAL_CHANS] =     {66, 67, 69, 68, 45, 44, 47, 46, 27, 65, 61, 60, 48, 49, 115, 112};


//-------------------------------------- PWM ------------------------------------
// Default to 7.x Layout, Updated in Constructor if newer
static const unsigned char g_PwmChans[NUM_PWM_CHANS] = {13, 19, 60, 62};
static string g_PwmDirPaths[NUM_PWM_CHANS] = {"/sys/class/pwm/pwm6", "/sys/class/pwm/pwm5", "/sys/class/pwm/pwm3", "/sys/class/pwm/pwm4"};
//static const string m_PwmDtoNames[NUM_PWM_CHANS] = {"bone_pwm_P8_13", "bone_pwm_P8_19", "bone_pwm_P9_14", "bone_pwm_P9_16"};
static const char *g_DutyCycleFileName = "duty_ns";
static const char *g_PeriodFileName = "period_ns";
static const char *g_EnableFileName = "run";


//------------------------------------- Uart ------------------------------------
static unsigned char g_UartChans[NUM_UART_CHANS] = {0, 1, 4};
static const char *g_UartPaths[NUM_UART_CHANS] = { "/dev/ttyO0", "/dev/ttyO1", "/dev/ttyO4"};
static const char *g_UartDtoNames[NUM_UART_CHANS] = { "BB-UART0", "BB-UART1", "BB-UART4"};

LinxBBBUartChannel::LinxBBBUartChannel(LinxFmtChannel *debug, const char *deviceName, const char *dtoName, const char *dtoSlotsPath) : 
	LinxUnixUartChannel(debug, deviceName)
{
	m_DtoName = dtoName;
	m_DtoSlotsPath = dtoSlotsPath;
}

int LinxBBBUartChannel::SmartOpen(void)
{
	//Load DTO If Needed
	if (!fileExists(m_DeviceName) && m_DtoName)
	{
		if (!LinxBeagleBoneBlack::loadDto(m_DtoSlotsPath, m_DtoName))	
		{
			LinxUartChannel::m_Debug->Write("UART Fail - Failed To Load Device Tree Overlay ");
			LinxUartChannel::m_Debug->Write(m_DtoName);
			return  LUART_OPEN_FAIL;
		}			
	}
	return LinxUnixSocketChannel::SmartOpen();
}

//------------------------------------- I2c -------------------------------------
static const unsigned char g_I2cChans[NUM_I2C_CHANS] = {2};
static const char *g_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-1" };		//Out of order numbering is correct for BBB 7.x!!
static const char *g_I2cDtoNames[NUM_I2C_CHANS] = {"BB-I2C2"};

LinxBBBI2cChannel::LinxBBBI2cChannel(LinxFmtChannel *debug, const char *channelName, const char *dtoName, const char *dtoSlotsPath) : LinxSysfsI2cChannel(debug, channelName)
{
	m_DtoName = dtoName;
	m_DtoSlotsPath = dtoSlotsPath;
}

int LinxBBBI2cChannel::Open(void)
{
	//Export Dev Tree Overlay If Device does not exist
	if (!fileExists(m_ChannelName) && m_DtoName && m_DtoName[0])
	{
		m_Debug->Write("I2C - Loading DTO ");
		m_Debug->Writeln(m_DtoName);
		if (!LinxBeagleBoneBlack::loadDto(m_DtoSlotsPath, m_DtoName))
		{
			m_Debug->Writeln("I2C Fail - Failed To Load I2C DTO");
			return  LI2C_OPEN_FAIL;
		}
	}
	return LinxSysfsI2cChannel::Open();
}


//------------------------------------- SPI -------------------------------------
static const unsigned char g_SpiChans[NUM_SPI_CHANS] = {0};
static const char *g_SpiPaths[NUM_SPI_CHANS] = { "/dev/spidev1.1"};
static const char *g_SpiDtoNames[NUM_SPI_CHANS] = { "BB-SPIDEV0"};
static unsigned int g_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
static int g_SpiSpeedCodes[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
static int g_SpiDefaultSpeed = 3900000;

LinxBBBSpiChannel::LinxBBBSpiChannel(LinxFmtChannel *debug, const char *channelName, LinxDevice *device, unsigned int speed, const char *dtoName, const char *dtoSlotsPath) : LinxSysfsSpiChannel(debug, channelName, device, speed)
{
	m_DtoName = dtoName;
	m_DtoSlotsPath = dtoSlotsPath;
	m_NumSpiSpeeds = NUM_SPI_SPEEDS;
	m_SpiSupportedSpeeds = g_SpiSupportedSpeeds;
	m_SpiSpeedCodes = g_SpiSpeedCodes;
}

int LinxBBBSpiChannel::Open(void)
{
	//Load SPI DTO if necessary
	if (!fileExists(m_ChannelName))
	{
		if (!LinxBeagleBoneBlack::loadDto(m_DtoSlotsPath, m_DtoName))
		{
			m_Debug->Write("SPI Fail - Failed To Load SPI DTO");
			return  LSPI_OPEN_FAIL;
		}
	}
	return LinxSysfsSpiChannel::Open();
}

//----------------------------------- Device ------------------------------------

/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxBeagleBoneBlack::LinxBeagleBoneBlack(LinxFmtChannel *debug) : LinxDevice(debug)
{
	//LINX Device Information
	DeviceFamily = LINX_FAMILY_TI;	// TI Family Code
	DeviceId = 0x01;				// BeagleBone Black

	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;

	//Check file system layout
	if (fileExists("/sys/devices/bone_capemgr.9/slots"))
	{
		//7.x Layout
		m_FilePathLayout = 7;
		m_DtoSlotsPath = "/sys/devices/bone_capemgr.9/slots";
	}
	else if (fileExists("/sys/devices/platform/bone_capemgr/slots"))
	{
		//8.x Layout
		m_FilePathLayout = 8;
		m_DtoSlotsPath = "/sys/devices/platform/bone_capemgr/slots";
	}
	else
	{
		//Assume 9.x Layout
		m_DtoSlotsPath = "";
		m_FilePathLayout = 9;
	}
		
		
	//AI
	AiResolution = AI_RES_BITS;
	
	AiRefDefault = AI_REFV;
	AiRefSet = AI_REFV;
//	AiRefCodes = NULL;
	
//	NumAiRefIntVals = NUM_AI_INT_REFS;
//	AiRefIntVals = NULL;
	
//	AiRefExtMin = 0;
//	AiRefExtMax = 0;
	
	//AO
//	AoResolution = 0;
//  AoRefDefault = 0;
//	AoRefSet = 0;
	
	//PWM
	//Shared Non Varying Components
	
	unsigned int g_PwmDefaultPeriod = 500000;	
	const char *g_PolarityFileName = "polarity";
	
	//7.x Only
	const char *g_PwmDtoNames[NUM_PWM_CHANS] = {"bone_pwm_P8_13", "bone_pwm_P8_19", "bone_pwm_P9_14", "bone_pwm_P9_16"};
		
	//8.x Only
	const char *g_PwmMuxPaths[NUM_PWM_CHANS] = {"/sys/devices/platform/ocp/ocp:P8_13_pinmux/state", "/sys/devices/platform/ocp/ocp:P8_19_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_14_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_16_pinmux/state"};
	const char *g_SpiMuxPaths[3] = {"/sys/devices/platform/ocp/ocp:P9_18_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_21_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_22_pinmux/state"};
	const char *g_UartMuxPaths[4] = {"/sys/devices/platform/ocp/ocp:P9_24_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_26_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_11_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_13_pinmux/state"};

	
	//Shared, Varying Components - Default To 7.x
	std::string g_PwmExportPaths[NUM_PWM_CHANS] = {"/sys/class/pwm/export", "/sys/class/pwm/export", "/sys/class/pwm/export", "/sys/class/pwm/export"};
	unsigned char g_PwmExportVal[NUM_PWM_CHANS] = {6, 5, 3, 4};


	//Update to 8.x layout if necessary
	if (m_FilePathLayout >= 8)
	{
		const char *pwmBasePath = "/sys/class/pwm/";
		
		const char *PwmP8ChipDir = "", *PwmP9ChipDir = "";
		bool sawPwmChanLinks = false;

		std::list<std::string> dirList;

		if (listDirectory(pwmBasePath, dirList))
		{
			for (std::list<std::string>::iterator it = dirList.begin(); it != dirList.end(); it++)
			{
				//PWM Chip Dir, Check Where Symlink Points
				char pwmChipSymlink[256];
				char pwmChipSymlinkTarget[256];
				bool isPwmChanLink = (strchr(it->c_str(), ':') != NULL);

				// Debian 9/stretch has direct links to PWM chans (e.g. "pwm7:0"); the chip dir does not have pwm0/pwm1 subdirs
				if (isPwmChanLink)
					sawPwmChanLinks = true;
					
				sprintf(pwmChipSymlink, "%s%s", pwmBasePath, it->c_str());
				readlink(pwmChipSymlink, pwmChipSymlinkTarget, 128);
					
				//Parse DTO Address
				//fprintf(stdout, pwmChipSymlinkTarget);	//Debug
				//fprintf(stdout, "\n");	//Debug
				char* token = strtok(pwmChipSymlinkTarget, "\\/");
				while(token != NULL)
				{
					unsigned int val = 0;
					sscanf(token, "%u", &val);
					//printf("%u\n", val);
					if (val == 48304200)
					{
						if (isPwmChanLink)
						{
							string pwmLink = pwmChipSymlink;
							pwmLink += "/";
				 			if (strchr(it->c_str(), '1') != NULL)
								g_PwmDirPaths[0] = pwmLink;
							else
								g_PwmDirPaths[1] = pwmLink;
						}
						else
						{
							PwmP8ChipDir = pwmChipSymlink;
						}
					}
					else if (val == 48302200)
					{
						if (isPwmChanLink)
						{
							string pwmLink = pwmChipSymlink;
							pwmLink += "/";
				 			if (strchr(it->c_str(), '0') != NULL)
								g_PwmDirPaths[2] = pwmLink;
							else
								g_PwmDirPaths[3] = pwmLink;
						}
						else
						{
							PwmP9ChipDir = pwmChipSymlink;
						}
					}
					token = strtok(NULL, "\\/");
				}
			}
		}
				
		//Build  PWM Paths
		g_PwmExportVal[0] = 1;
		g_PwmExportVal[1] = 0;
		g_PwmExportVal[2] = 0;
		g_PwmExportVal[3] = 1;
		
		if (!sawPwmChanLinks)
		{
			g_PwmDirPaths[0] = std::string(PwmP8ChipDir, "/pwm1/");
			g_PwmDirPaths[1] = std::string(PwmP8ChipDir, "/pwm0/");
			g_PwmDirPaths[2] = std::string(PwmP9ChipDir, "/pwm0/");
			g_PwmDirPaths[3] = std::string(PwmP9ChipDir, "/pwm1/");
		}
		
		g_PwmExportPaths[0] = std::string(PwmP8ChipDir, "/export");
		g_PwmExportPaths[1] = std::string(PwmP8ChipDir, "/export");
		g_PwmExportPaths[2] = std::string(PwmP9ChipDir, "/export");
		g_PwmExportPaths[3] = std::string(PwmP9ChipDir, "/export");
		
		g_DutyCycleFileName = "duty_cycle";
		g_PeriodFileName = "period";
		g_EnableFileName = "enable";
		
		//Update I2C Path
		g_I2cPaths[0] = "/dev/i2c-2";
	}

	// Updates to 9.x layout
	if (m_FilePathLayout >= 9) {
		g_SpiPaths[0] = "/dev/spidev0.0";
	}
	
//	PwmDutyCycleFileName = g_DutyCycleFileName;
//	PwmPeriodFileName = g_PeriodFileName;
//	PwmEnableFileName = g_EnableFileName;
		
//	PwmDefaultPeriod = g_PwmDefaultPeriod;
		
	//QE
		
	//UART

	//I2C
		
	//SPI
		
	//CAN
	
	//SERVO
	
	//------------------------------------- ANALOG -------------------------------------
	//Export Dev Tree Overlay For AI If It DNE And Open AI Handles
	bool dtoLoaded = false;
	if (!fileExists("/sys/bus/iio/devices/iio:device0"))
	{
		if (loadDto(m_DtoSlotsPath, "BB-ADC"))
		{
			dtoLoaded = true;			
		}
		else
		{
			m_Debug->Writeln("AI Fail - Failed To Load BB-ADC DTO");
		}
	}
	else
	{
		//DTO Already Loaded
		dtoLoaded = true;
	}
	
	if (dtoLoaded)
	{
		//Open AI Handles		
		for (int i = 0; i < NUM_AI_CHANS; i++)
		{
			LinxChannel *chan = new LinxSysfsAiChannel(m_Debug, g_AiPaths[i]);
			if (chan)
				RegisterChannel(IID_LinxAiChannel, g_AiChans[i], chan);
			else
				m_Debug->Write("AI Fail - Failed to open AI channel handle: ");
				m_Debug->Writeln(g_AiPaths[i]);
		}
	}	
	
	//------------------------------------- DIGITAL -------------------------------------
	// Initialize the digital lookup map
	
	//------------------------------------- PWM -------------------------------------
	
	//PWM General Initialization
	if (m_FilePathLayout == 7)
	{
		//Load AM33xx_PWM DTO If No PWM Channels Have Been Exported Since Boot
		if (!fileExists(g_PwmDirPaths[NUM_PWM_CHANS - 1].c_str(), g_PeriodFileName))
		{
			if (!loadDto(m_DtoSlotsPath, "am33xx_pwm"))
			{
				m_Debug->Writeln("PWM Fail - Failed To Load am33xx_pwm DTO");
			}
			
			//Export PWM Channels Before Loading Channel Specific DTOs Below
			/*
			for(int i=0; i< NUM_PWM_CHANS; i++)
			{
				
			}
			*/
		}
	}
	else if (m_FilePathLayout >= 8)
	{
		// Set Mux to PWM
		for (int i = 0; i < NUM_PWM_CHANS; i++)
		{
			FILE* pwmMuxHandle = fopen(g_PwmMuxPaths[i], "r+w+");
			if (pwmMuxHandle != NULL)
			{
				fprintf(pwmMuxHandle, "pwm");
				fclose(pwmMuxHandle);							
			}
		}
	}
	
	// Per Pin Initialization
	for (int i = 0; i < NUM_PWM_CHANS; i++)
	{
		//Store Default Values
//		PwmDirPaths[g_PwmChans[i]] = g_PwmDirPaths[i];
//		PwmPeriods[g_PwmChans[i]] = g_PwmDefaultPeriod;
		RegisterChannel(IID_LinxPwmChannel, g_PwmChans[i], (LinxPwmChannel*)new LinxSysfsPwmChannel(m_Debug, g_PwmDirPaths[i].c_str(), g_EnableFileName, g_PeriodFileName, g_DutyCycleFileName, g_PwmDefaultPeriod));
		
		//Export PWM Channels - This Must Happend Before 7.x Loads Channel Specific DTOs
		if (!fileExists(g_PwmDirPaths[i].c_str(), g_PeriodFileName))
		{
			FILE* pwmExportHandle = fopen(g_PwmExportPaths[i].c_str(), "w");
			if (pwmExportHandle != NULL)
			{
				fprintf(pwmExportHandle, "%u", g_PwmExportVal[i]);
				fclose(pwmExportHandle);
			}
			else
			{
				m_Debug->Writeln("PWM Fail - Unable to open pwmExportHandle");
			}
		}
		
		//7.x Per Pin Init
		if (m_FilePathLayout == 7)
		{	
			//Load Chip Specific PWM DTO If Not Already Loaded
			if (!loadDto(m_DtoSlotsPath, g_PwmDtoNames[i]))
			{
				m_Debug->Write("PWM Fail - Failed To Load PWM DTO ");
				m_Debug->Writeln(g_PwmDtoNames[i]);
			}
			
			//Make Sure DTO Has Time To Load Before Opening Handles
			else if (!fileExists(g_PwmDirPaths[i].c_str(), "period_ns", 3000))
			{
				m_Debug->Write("PWM Fail - PWM DTO Did Not Load Correctly: ");				
				m_Debug->Writeln(g_PwmDirPaths[i].c_str());				
			}		
		}
		//Export PWM Chans.  If 7.x layout this is done above.  This should probably be moved.
		if (m_FilePathLayout >= 8)
		{
			//Nothing 8.x Specific For Now
		}
		
		//Set Polarity To 0 So PWM Value Corresponds To 'Percent On' Rather Than 'Percent Off'
		char polarityPath[64];
		sprintf(polarityPath, "%s%s", g_PwmDirPaths[i].c_str(), g_PolarityFileName);
		
		FILE* pwmPolarityHandle = fopen(polarityPath, "w");
		if (pwmPolarityHandle != NULL)
		{
			fprintf(pwmPolarityHandle, "0");
			fclose(pwmPolarityHandle);
		}
		else
		{
			m_Debug->Write("PWM Fail - Unable to open pwmPolarityHandle");				
		}
	}	
	
	//------------------------------------- I2C -------------------------------------
	// Store I2C Master Paths In Map
	for (int i = 0; i < NUM_I2C_CHANS; i++)
	{	
		LinxI2cChannel *chan = new LinxBBBI2cChannel(m_Debug, g_I2cPaths[i], m_FilePathLayout == 7 ? g_I2cDtoNames[i] : NULL, m_DtoSlotsPath);
		RegisterChannel(IID_LinxI2cChannel, g_I2cChans[i], chan);
	}

	//------------------------------------- UART ------------------------------------
	if (m_FilePathLayout >= 8)
	{
		// Set Mux to UART
		for (int i = 0; i < 4; i++)
		{
			FILE* uartMuxHandle = fopen(g_UartMuxPaths[i], "r+w+");
			if (uartMuxHandle != NULL)
			{
				fprintf(uartMuxHandle, "uart");
				fclose(uartMuxHandle);							
			}
		}
	}
	
	// Store Uart Paths In Map
	for (int i = 0; i < NUM_UART_CHANS; i++)
	{
		RegisterChannel(IID_LinxUartChannel, g_UartChans[i], (LinxUartChannel*)new LinxBBBUartChannel(m_Debug, g_UartPaths[i], g_UartDtoNames[i], m_DtoSlotsPath));
	}
	
	//------------------------------------- SPI ------------------------------------
	if (m_FilePathLayout >= 8)
	{
		//Set Mux to SPI
		for (int i = 0; i < 3; i++)
		{
			FILE* spiMuxHandle = fopen(g_SpiMuxPaths[i], "r+w+");
			if (spiMuxHandle != NULL)
			{
				// in later debian versions the state value for the clk line changed
				if (m_FilePathLayout == 8)
					fprintf(spiMuxHandle, "spi");
				else if (m_FilePathLayout >= 9 && (i%3) < 2)
					fprintf(spiMuxHandle, "spi");
				else if (m_FilePathLayout >= 9 && (i%3) == 2)
					fprintf(spiMuxHandle, "spi_sclk");  // assume last mux path is the sclk
				else
					m_Debug->Write("SPI Fail - Unexpected SpiMuxPath");
				fclose(spiMuxHandle);
			}
		}
	}
	
	//Load SPI Paths and DTO Names, Configure SPI Master Default Values
	for (int i = 0; i < NUM_SPI_CHANS; i++)
	{
		RegisterChannel(IID_LinxSpiChannel, g_SpiChans[i], new LinxBBBSpiChannel(m_Debug, g_SpiPaths[i], this, g_SpiDefaultSpeed, g_SpiDtoNames[i], m_DtoSlotsPath));
	}
}

//Destructor
LinxBeagleBoneBlack::~LinxBeagleBoneBlack(void)
{	
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
//Load Device Tree Overlay
bool LinxBeagleBoneBlack::loadDto(const char *slotsPath, const char* dtoName)
{
	if (slotsPath[0] == '\0')
		return true;

	FILE* slotsHandle = fopen(slotsPath, "r+w+");
	if (slotsHandle != NULL)
	{
		fprintf(slotsHandle, "%s", dtoName);
		fclose(slotsHandle);
		return true;
	}
	return false;
}

/****************************************************************************************
**  Public Functions
****************************************************************************************/
unsigned char LinxBeagleBoneBlack::GetDeviceName(unsigned char* buffer, unsigned char length)
{
	if (buffer)
		strncpy((char*)buffer, m_DeviceName, length); 
	return (unsigned char)strlen(m_DeviceName);
}

//--------------------------------------------------------PWM-------------------------------------------------------

//--------------------------------------------------------SPI-------------------------------------------------------

//--------------------------------------------------------I2C-------------------------------------------------------

//--------------------------------------------------------UART-------------------------------------------------------
