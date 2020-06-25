/****************************************************************************************
**  LINX BeagleBone Black Code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "LinxDefines.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <string.h>
#ifndef Win32
#include <dirent.h>
#include <unistd.h>	
#include <termios.h>
#else
#include <io.h>
#endif
#include "LinxDevice.h"
#include "LinxUtilities.h"
#include "LinxLinuxDevice.h"
#include "LinxBeagleBoneBlack.h"

using namespace std;

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System
static const char *m_DeviceName = "BeagleBone Black";

//-------------------------------------- AI -------------------------------------
static const unsigned char m_AiChans[NUM_AI_CHANS] = {0, 1, 2, 3, 4, 5, 6};
static const string m_AiValuePaths[NUM_AI_CHANS] = {"/sys/bus/iio/devices/iio:device0/in_voltage0_raw",
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
static const unsigned char m_DigitalChans[NUM_DIGITAL_CHANS] = { 7,  8,  9, 10, 11, 12, 15, 16, 17, 18, 26, 58, 61, 69,  73,  76};
static const unsigned char m_gpioChan[NUM_DIGITAL_CHANS] =     {66, 67, 69, 68, 45, 44, 47, 46, 27, 65, 61, 60, 48, 49, 115, 112};


//-------------------------------------- PWM ------------------------------------
// Default to 7.x Layout, Updated in Constructor if newer
static const unsigned char m_PwmChans[NUM_PWM_CHANS] = {13, 19, 60, 62};
static string m_PwmDirPaths[NUM_PWM_CHANS] = {"/sys/class/pwm/pwm6", "/sys/class/pwm/pwm5", "/sys/class/pwm/pwm3", "/sys/class/pwm/pwm4"};
static string m_EnableFileName;
//static const string m_PwmDtoNames[NUM_PWM_CHANS] = {"bone_pwm_P8_13", "bone_pwm_P8_19", "bone_pwm_P9_14", "bone_pwm_P9_16"};


//------------------------------------- Uart ------------------------------------
static unsigned char g_UartChans[NUM_UART_CHANS] = {0, 1, 4};
static const char *g_UartPaths[NUM_UART_CHANS] = { "/dev/ttyO0", "/dev/ttyO1", "/dev/ttyO4"};
static const char *g_UartDtoNames[NUM_UART_CHANS] = { "BB-UART0", "BB-UART1", "BB-UART4"};

LinxBBBUartChannel::LinxBBBUartChannel(const char *channelName, LinxFmtChannel *debug, const char *dtoName, const char *dtoSlotsPath) : LinxUnixUartChannel(channelName, debug)
{
	m_DtoName = dtoName;
	m_DtoSlotsPath = dtoSlotsPath;
}

LinxChannel *LinxBBBUartChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxBBBUartChannel)
	{
		AddRef();
		return this;
	}
	return LinxUnixUartChannel::QueryInterface(interfaceId);
}

int LinxBBBUartChannel::SmartOpen()
{
	//Load DTO If Needed
	if (!fileExists(m_DeviceName) && m_DtoName)
	{
		if (!LinxBeagleBoneBlack::loadDto(m_DtoSlotsPath, m_DtoName))	
		{
			m_Debug->Write("UART Fail - Failed To Load ");
			m_Debug->Write(m_DtoName);
			m_Debug->Writeln(" DTO");
			return  LUART_OPEN_FAIL;
		}			
	}
	return LinxUnixUartChannel::SmartOpen();
}


//------------------------------------- I2c -------------------------------------
static const unsigned char g_I2cChans[NUM_I2C_CHANS] = {2};
static const char *g_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-1" };		//Out of order numbering is correct for BBB 7.x!!
static const char *g_I2cDtoNames[NUM_I2C_CHANS] = {"BB-I2C2"};

LinxBBBI2cChannel::LinxBBBI2cChannel(const char *channelName, LinxFmtChannel *debug, const char *dtoName, const char *dtoSlotsPath) : LinxSysfsI2cChannel(channelName, debug)
{
	m_DtoName = dtoName;
	m_DtoSlotsPath = dtoSlotsPath;
}

LinxChannel *LinxBBBI2cChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxBBBI2cChannel)
	{
		AddRef();
		return this;
	}
	return LinxSysfsI2cChannel::QueryInterface(interfaceId);
}

int LinxBBBI2cChannel::Open()
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

LinxBBBSpiChannel::LinxBBBSpiChannel(const char *channelName, LinxFmtChannel *debug, LinxLinuxDevice *device, unsigned int speed, const char *dtoName, const char *dtoSlotsPath) : LinxSysfsSpiChannel(channelName, debug, device, speed)
{
	m_DtoName = dtoName;
	m_DtoSlotsPath = dtoSlotsPath;
	m_NumSpiSpeeds = NUM_SPI_SPEEDS;
	m_SpiSupportedSpeeds = g_SpiSupportedSpeeds;
	m_SpiSpeedCodes = g_SpiSpeedCodes;
}

LinxChannel *LinxBBBSpiChannel::QueryInterface(int interfaceId)
{
	if (interfaceId == IID_LinxBBBSpiChannel)
	{
		AddRef();
		return this;
	}
	return LinxSysfsSpiChannel::QueryInterface(interfaceId);
}

int LinxBBBSpiChannel::Open()
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
LinxBeagleBoneBlack::LinxBeagleBoneBlack()
{
	//LINX Device Information
	DeviceFamily = 0x06;	//TI Family Code
	DeviceId = 0x01;			//BeagleBone Black

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
	AiRefCodes = NULL;
	
	NumAiRefIntVals = NUM_AI_INT_REFS;
	AiRefIntVals = NULL;
	
	AiRefExtMin = 0;
	AiRefExtMax = 0;
	
	//AO
	AoResolution = 0;
    AoRefDefault = 0;
	AoRefSet = 0;
	
	//PWM
	//Shared Non Varying Components
	
	unsigned int m_PwmDefaultPeriod = 500000;	
	string m_PolarityFileName = "polarity";
	
	//7.x Only
	const char *m_PwmDtoNames[NUM_PWM_CHANS] = {"bone_pwm_P8_13", "bone_pwm_P8_19", "bone_pwm_P9_14", "bone_pwm_P9_16"};
		
	//8.x Only
	const char *m_PwmMuxPaths[NUM_PWM_CHANS] = {"/sys/devices/platform/ocp/ocp:P8_13_pinmux/state", "/sys/devices/platform/ocp/ocp:P8_19_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_14_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_16_pinmux/state"};
	const char *m_SpiMuxPaths[3] = {"/sys/devices/platform/ocp/ocp:P9_18_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_21_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_22_pinmux/state"};
	const char *m_UartMuxPaths[4] = {"/sys/devices/platform/ocp/ocp:P9_24_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_26_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_11_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_13_pinmux/state"};

	
	//Shared, Varying Components - Default To 7.x
	std::string m_PwmExportPaths[NUM_PWM_CHANS] = {"/sys/class/pwm/export", "/sys/class/pwm/export", "/sys/class/pwm/export", "/sys/class/pwm/export"};
	unsigned char m_PwmExportVal[NUM_PWM_CHANS] = {6, 5, 3, 4};
	std::string m_DutyCycleFileName = "duty_ns";
	std::string m_PeriodFileName = "period_ns";
	m_EnableFileName = "run";


	//Update to 8.x layout if necessary
	if (m_FilePathLayout >= 8)
	{
		string pwmBasePath = "/sys/class/pwm/";
		
		string PwmP8ChipDir = "", PwmP9ChipDir = "";
		bool sawPwmChanLinks = false;
		
		DIR* pwmDirHandle = opendir(pwmBasePath.c_str());

		dirent* dp;

		//Loop Over All Dirs In PWM Base Dir
		while (pwmDirHandle)
		{
			if ((dp = readdir(pwmDirHandle)) != NULL)
			{
				//Make Sure Dir Is Not . or ..
				if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
				{
					//PWM Chip Dir, Check Where Symlink Points
					char pwmChipSymlink[256];
					char pwmChipSymlinkTarget[256];
					bool isPwmChanLink = (strchr(dp->d_name, ':') != NULL);

					// Debian 9/stretch has direct links to PWM chans (e.g. "pwm7:0"); the chip dir does not have pwm0/pwm1 subdirs
					if (isPwmChanLink)
						sawPwmChanLinks = true;
					
					sprintf(pwmChipSymlink, "%s%s", pwmBasePath.c_str(), dp->d_name);
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
					 			if (strchr(dp->d_name, '1') != NULL)
									m_PwmDirPaths[0] = pwmLink;
								else
									m_PwmDirPaths[1] = pwmLink;
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
					 			if (strchr(dp->d_name, '0') != NULL)
									m_PwmDirPaths[2] = pwmLink;
								else
									m_PwmDirPaths[3] = pwmLink;
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
			else
			{
				closedir(pwmDirHandle);
				break;
			}
		}
				
		//Build  PWM Paths
		m_PwmExportVal[0] = 1;
		m_PwmExportVal[1] = 0;
		m_PwmExportVal[2] = 0;
		m_PwmExportVal[3] = 1;
		
		if (!sawPwmChanLinks)
		{
			m_PwmDirPaths[0] = PwmP8ChipDir + "/pwm1/";
			m_PwmDirPaths[1] = PwmP8ChipDir + "/pwm0/";
			m_PwmDirPaths[2] = PwmP9ChipDir + "/pwm0/";
			m_PwmDirPaths[3] = PwmP9ChipDir + "/pwm1/";
		}
		
		m_PwmExportPaths[0] = PwmP8ChipDir + "/export";
		m_PwmExportPaths[1] = PwmP8ChipDir + "/export";
		m_PwmExportPaths[2] = PwmP9ChipDir + "/export";
		m_PwmExportPaths[3] = PwmP9ChipDir + "/export";
		
		m_DutyCycleFileName = "duty_cycle";
		m_PeriodFileName = "period";
		m_EnableFileName = "enable";
		
		//Update I2C Path
		g_I2cPaths[0] = "/dev/i2c-2";
	}

	// Updates to 9.x layout
	if (m_FilePathLayout >= 9) {
		g_SpiPaths[0] = "/dev/spidev0.0";
	}
	
	PwmDutyCycleFileName = m_DutyCycleFileName;
	PwmPeriodFileName = m_PeriodFileName;
	PwmEnableFileName = m_EnableFileName;
		
	PwmDefaultPeriod = m_PwmDefaultPeriod;
		
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
			AiValuePaths[m_AiChans[i]] = m_AiValuePaths[i];
			AiValueHandles[m_AiChans[i]] = fopen(m_AiValuePaths[i].c_str(), "r+");
			
			if (AiValueHandles[m_AiChans[i]] <= 0)
			{
				m_Debug->Writeln("AI Fail - Failed Open AI Channel Handle");
			}			
		}
	}	
	
	//------------------------------------- DIGITAL -------------------------------------
	// Initialize the digital lookup map
	
	//------------------------------------- PWM -------------------------------------
	
	//PWM General Initialization
	if (m_FilePathLayout == 7)
	{
		//Load AM33xx_PWM DTO If No PWM Channels Have Been Exported Since Boot
		if (!fileExists(m_PwmDirPaths[NUM_PWM_CHANS - 1].c_str(), m_PeriodFileName.c_str()))
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
			FILE* pwmMuxHandle = fopen(m_PwmMuxPaths[i], "r+w+");
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
		PwmDirPaths[m_PwmChans[i]] = m_PwmDirPaths[i];
		PwmPeriods[m_PwmChans[i]] = m_PwmDefaultPeriod;
		
		//Export PWM Channels - This Must Happend Before 7.x Loads Channel Specific DTOs
		if (!fileExists(m_PwmDirPaths[i].c_str(), m_PeriodFileName.c_str()))
		{
			FILE* pwmExportHandle = fopen(m_PwmExportPaths[i].c_str(), "w");
			if (pwmExportHandle != NULL)
			{
				fprintf(pwmExportHandle, "%u", m_PwmExportVal[i]);
				fclose(pwmExportHandle);
			}
			else
			{
				m_Debug->Writeln("PWM Fail - Unable to open pwmExportHandle");
			}

			//Set Default Period Only First Time
			char periodPath[64];
			sprintf(periodPath, "%s%s", m_PwmDirPaths[i].c_str(), m_PeriodFileName.c_str());
			
			FILE* pwmPeriodleHandle = fopen(periodPath, "r+w+");
			if (pwmPeriodleHandle != NULL)
			{
				fprintf(pwmPeriodleHandle, "%u", m_PwmDefaultPeriod);
				fclose(pwmPeriodleHandle);							
			}
			else
			{
				m_Debug->Writeln("PWM Fail - Unable to open pwmPeriodHandle");
			}
		}
		
		//7.x Per Pin Init
		if (m_FilePathLayout == 7)
		{	
			//Load Chip Specific PWM DTO If Not Already Loaded
			if (!loadDto(m_DtoSlotsPath, m_PwmDtoNames[i]))
			{
				m_Debug->Write("PWM Fail - Failed To Load PWM DTO ");
				m_Debug->Writeln(m_PwmDtoNames[i]);
			}
			
			//Make Sure DTO Has Time To Load Before Opening Handles
			else if (!fileExists(m_PwmDirPaths[i].c_str(), "period_ns", 3000))
			{
				m_Debug->Write("PWM Fail - PWM DTO Did Not Load Correctly: ");				
				m_Debug->Writeln(m_PwmDirPaths[i].c_str());				
			}		
		}
		//Export PWM Chans.  If 7.x layout this is done above.  This should probably be moved.
		if (m_FilePathLayout >= 8)
		{
			//Nothing 8.x Specific For Now
		}
		
		//Set Polarity To 0 So PWM Value Corresponds To 'Percent On' Rather Than 'Percent Off'
		char polarityPath[64];
		sprintf(polarityPath, "%s%s", PwmDirPaths[m_PwmChans[i]].c_str(), m_PolarityFileName.c_str());
		
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
			
		//Set Default Duty Cycle To 0	
		char dutyCyclePath[64];
		sprintf(dutyCyclePath, "%s%s", PwmDirPaths[m_PwmChans[i]].c_str(), m_DutyCycleFileName.c_str());			
		
		FILE* pwmDutyCycleHandle = fopen(dutyCyclePath, "r+w+");
		if (pwmDutyCycleHandle != NULL)
		{
			fprintf(pwmDutyCycleHandle, "0");
			fclose(pwmDutyCycleHandle);
		}
		else
		{
			m_Debug->Write("PWM Fail - Unable to open pwmDutyCycleHandle");				
		}		
		
		//Turn On PWM		
		char enablePath[64];
		sprintf(enablePath, "%s%s", PwmDirPaths[m_PwmChans[i]].c_str(), m_EnableFileName.c_str());			
		FILE* pwmEnableHandle = fopen(enablePath, "r+w+");
		if (pwmEnableHandle != NULL)
		{
			fprintf(pwmEnableHandle, "1");
			fclose(pwmEnableHandle);		
		}
		else
		{
			m_Debug->Write("PWM Fail - Unable to open pwmEnableHandle");				
		}	
	}	
	
	//------------------------------------- I2C -------------------------------------
	// Store I2C Master Paths In Map
	for (int i = 0; i < NUM_I2C_CHANS; i++)
	{	
		LinxI2cChannel *chan = new LinxBBBI2cChannel(g_I2cPaths[i], m_Debug, m_FilePathLayout == 7 ? g_I2cDtoNames[i] : NULL, m_DtoSlotsPath);
		RegisterChannel(IID_LinxI2cChannel, g_UartChans[i], chan);
	}

	//------------------------------------- UART ------------------------------------
	if (m_FilePathLayout >= 8)
	{
		// Set Mux to UART
		for (int i = 0; i < 4; i++)
		{
			FILE* uartMuxHandle = fopen(m_UartMuxPaths[i], "r+w+");
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
		RegisterChannel(IID_LinxUartChannel, g_UartChans[i], new LinxBBBUartChannel(g_UartPaths[i], m_Debug, g_UartDtoNames[i], m_DtoSlotsPath));
	}
	
	//------------------------------------- SPI ------------------------------------
	if (m_FilePathLayout >= 8)
	{
		//Set Mux to SPI
		for (int i = 0; i < 3; i++)
		{
			FILE* spiMuxHandle = fopen(m_SpiMuxPaths[i], "r+w+");
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
	SpiDefaultSpeed = 3900000;
	for (int i = 0; i < NUM_SPI_CHANS; i++)
	{
		RegisterChannel(IID_LinxSpiChannel, g_SpiChans[i], new LinxBBBSpiChannel(g_SpiPaths[i], m_Debug, this, SpiDefaultSpeed, g_SpiDtoNames[i], m_DtoSlotsPath));
	}
	
	//If Debugging Is Enabled Call EnableDebug()
	#if DEBUG_ENABLED >= 0
		EnableDebug(DEBUG_ENABLED);
	#endif
}

//Destructor
LinxBeagleBoneBlack::~LinxBeagleBoneBlack()
{	
	//Close AI Handles
	for (int i = 0; i < NUM_AI_CHANS; i++)
	{
		if (AiValueHandles[m_AiChans[i]] != 0)
		{
			fclose(AiValueHandles[m_AiChans[i]]);
		}
	}
	
	//Close PWM Handles If Open
	for (int i = 0; i < NUM_PWM_CHANS; i++)
	{
		if (PwmPeriodHandles[m_PwmChans[i]] != NULL)
		{
			fclose(PwmPeriodHandles[m_PwmChans[i]]);
		}
		if (PwmDutyCycleHandles[m_PwmChans[i]] != NULL)
		{
			fprintf(PwmDutyCycleHandles[m_PwmChans[i]], "0");
			fclose(PwmDutyCycleHandles[m_PwmChans[i]]);
		}
		
		//Turn Off PWM		
		char enablePath[64];
		sprintf(enablePath, "%s%s", m_PwmDirPaths[i].c_str(), m_EnableFileName.c_str());
		FILE* pwmEnableHandle = fopen(enablePath, "r+w+");
		if (pwmEnableHandle != NULL)
		{
			fprintf(pwmEnableHandle, "0");
			fclose(pwmEnableHandle);
		}
		else
		{
			m_Debug->Write("PWM Fail - Unable to open pwmEnableHandle");
		}
	}
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

//--------------------------------------------------------ANALOG-------------------------------------------------------
int LinxBeagleBoneBlack::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{

	//unsigned int analogValue = 0;
	unsigned char responseByteOffset = 0;
	unsigned char responseBitsRemaining = 8;
	unsigned char dataBitsRemaining = AiResolution;
	fstream fs;	//AI File Handle

	values[responseByteOffset] = 0x00;    //Clear First	Response Byte

	//Loop Over All AI channels In Command Packet
	for (int i = 0; i < numChans; i++)
	{
		//Acquire AI Sample
		int aiVal = 0;
		AiValueHandles[channels[i]] = freopen(AiValuePaths[channels[i]].c_str(), "r+", AiValueHandles[channels[i]]);
		fscanf(AiValueHandles[channels[i]], "%u", &aiVal);

		/*
		fs.open(AiPaths[channels[i]], fstream::in);
		fs >> analogValue;
		fs.close();
		*/

		dataBitsRemaining = AiResolution;

		//Byte Packet AI Values In Response Packet
		while (dataBitsRemaining > 0)
		{
			*(values + responseByteOffset) |= ( ((unsigned int)aiVal >> (AiResolution - dataBitsRemaining)) << (8 - responseBitsRemaining));
			//*(values+responseByteOffset) = 69;

			if (responseBitsRemaining > dataBitsRemaining)
			{
				//Current Byte Still Has Empty Bits
				responseBitsRemaining -= dataBitsRemaining;
				dataBitsRemaining = 0;
			}
			else
			{
				//Current Byte Full
				dataBitsRemaining = dataBitsRemaining - responseBitsRemaining;
				responseByteOffset++;
				responseBitsRemaining = 8;
				values[responseByteOffset] = 0x00;    //Clear Next Response Byte
			}
		}
	}
	return L_OK;
}

int LinxBeagleBoneBlack::AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned long* values)
{
	//Loop Over All AI channels In Command Packet
	for (int i = 0; i < numChans; i++)
	{
		AiValueHandles[channels[i]] = freopen(AiValuePaths[channels[i]].c_str(), "r+", AiValueHandles[channels[i]]);
		fscanf(AiValueHandles[channels[i]], "%lu", values+i);
	}
	return L_OK;
}

//--------------------------------------------------------PWM-------------------------------------------------------
int LinxBeagleBoneBlack::PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//unsigned int period = 500000;		//Period Defaults To 500,000 nS. To Do Update This When Support For Changing Period / Frequency Is Added
	unsigned int dutyCycle = 0;

	//Smart Open PWM Channels
	pwmSmartOpen(numChans, channels);

	for (int i = 0; i < numChans; i++)
	{
		if (values[i] == 0)
		{
			dutyCycle = 0;
		}
		else if (values[i] == 255)
		{
			dutyCycle = PwmPeriods[channels[i]];
		}
		else
		{
			dutyCycle = (unsigned int)(PwmPeriods[channels[i]] * (values[i] / 255.0));
		}

		//Update Output
		m_Debug->Write("Setting Duty Cycle = ");
		m_Debug->Write(dutyCycle, DEC);
		fprintf(PwmDutyCycleHandles[channels[i]], "%u", dutyCycle);
		m_Debug->Write(" ... Duty Cycle Set ... ");
		fflush(PwmDutyCycleHandles[channels[i]]);
		m_Debug->Writeln("Flushing.");
	}
	return L_OK;
}


//--------------------------------------------------------SPI-------------------------------------------------------

//--------------------------------------------------------I2C-------------------------------------------------------

//--------------------------------------------------------UART-------------------------------------------------------
