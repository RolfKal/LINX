/****************************************************************************************
**  LINX - BeagleBone code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter based on code from Sam Kristoff
**
** BSD2 License.
****************************************************************************************/


/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stddef.h> 
#include "LinxDefines.h"
#include "LinxDevice.h"
#include "LinxLinuxChannel.h"
#include "LinxLinuxDevice.h"

/****************************************************************************************
**  Constructors / Destructors
****************************************************************************************/

/****************************************************************************************
**  Public Functions
****************************************************************************************/
int LinxLinuxDevice::UartOpen(const char *channelName, unsigned char *channel, LinxUartChannel **chan)
{
	LinxUartChannel *temp = NULL;
	unsigned char numChannels = EnumerateChannels(IID_LinxUartChannel, NULL, 0);
	if (numChannels)
	{
		unsigned char *channels = (unsigned char*)malloc(numChannels);
		if (channels)
		{
			char buffer[32];
			numChannels = EnumerateChannels(IID_LinxUartChannel, channels, numChannels);
			for (int i = 0; i < numChannels; i++)
			{
				temp = (LinxUartChannel*)LookupChannel(IID_LinxUartChannel, channels[i]);
				if (temp)
				{
					temp->GetName(buffer, 32);
					if (!strcmp(channelName, buffer))
					{
						*channel = channels[i];
						if (chan)
							*chan = temp;
						else
							temp->Release();
						free(channels);
						return L_OK;
					}
					temp->Release();
				}
			}
			free(channels);
		}
	}
	temp = new LinxUnixUartChannel(channelName, m_Debug);
	*channel = RegisterChannel(IID_LinxUartChannel, temp);
	if (chan)
	{
		temp->AddRef();
		*chan = temp;
	}
	return LERR_BADPARAM;
}

/****************************************************************************************
**  Protected Functions
****************************************************************************************/

//Open Direction And Value Handles If They Are Not Already Open And Set Direction
int LinxLinuxDevice::pwmSmartOpen(unsigned char numChans, unsigned char* channels)
{
	for (int i = 0; i < numChans; i++)
	{
		//Open Period Handle If It Is Not Already
		if (PwmPeriodHandles[channels[i]] == NULL)
		{
			char periodPath[64];
			sprintf(periodPath, "%s%s", PwmDirPaths[channels[i]].c_str(), PwmPeriodFileName.c_str());
			m_Debug->Write("Opening ");
			m_Debug->Writeln(periodPath);
			PwmPeriodHandles[channels[i]] = fopen(periodPath, "r+w+");

			//Initialize PWM Period
			fprintf(PwmPeriodHandles[channels[i]], "%u", PwmDefaultPeriod);
			PwmPeriods[channels[i]] = PwmDefaultPeriod;
			fflush(PwmPeriodHandles[channels[i]]);
		}

		//Open Duty Cycle Handle If It Is Not Already
		if (PwmDutyCycleHandles[channels[i]] == NULL)
		{
			char dutyCyclePath[64];
			sprintf(dutyCyclePath, "%s%s", PwmDirPaths[channels[i]].c_str(), PwmDutyCycleFileName.c_str());
			m_Debug->Write("Opening ");
			m_Debug->Writeln(dutyCyclePath);
			PwmDutyCycleHandles[channels[i]] = fopen(dutyCyclePath, "r+w+");
		}
	}
	return L_OK;
}

/****************************************************************************************
**  Public Channel Registry Functions
****************************************************************************************/
unsigned char LinxLinuxDevice::EnumerateChannels(int type, unsigned char *buffer, unsigned char length)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	int i = 0, num = (int)m.size();
	if (num && buffer)
	{
		for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin();  i < length && it != m.end(); ++it)
		{
			buffer[i] = it->first;
		}
	}
	return (unsigned char)num;
}

/****************************************************************************************
**  Protected Channel Registry Functions
****************************************************************************************/
unsigned char LinxLinuxDevice::RegisterChannel(int type, LinxChannel *chan)
{
	unsigned char channel = 0;
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin(); it != m.end(); it++)
	{
		if (it->first == channel)
		{
			channel = it->first + 1;
		}
		else
		{
			break;
		}
	}
	m.insert(std::pair<unsigned char, LinxChannel*>(channel, chan));
	return channel;
}

void LinxLinuxDevice::RegisterChannel(int type, unsigned char channel, LinxChannel *chan)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::pair<std::map<unsigned char, LinxChannel*>::iterator, bool> result = m.insert(std::pair<unsigned char, LinxChannel*>(channel, chan));
	if (!result.second)
	{
		result.first->second->Release();
		result.first->second = chan;
		result.first->second->AddRef();
	}
}

LinxChannel* LinxLinuxDevice::LookupChannel(int type, unsigned char channel)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::map<unsigned char, LinxChannel*>::iterator it = m.find(channel);
	return it != m.end() ? it->second->QueryInterface(type) : NULL;
}

void LinxLinuxDevice::RemoveChannel(int type, unsigned char channel)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	std::map<unsigned char, LinxChannel*>::iterator it = m.find(channel);
	if (it != m.end())
	{
		it->second->Release();
		m.erase(it);
	}
}

void LinxLinuxDevice::ClearChannels(int type)
{
	std::map<unsigned char, LinxChannel*> m = m_ChannelRegistry[type - 1];
	for (std::map<unsigned char, LinxChannel*>::iterator it = m.begin(); it != m.end(); it++)
	{
		int count = it->second->Release();
		if (count)
		{
			m_Debug->Write("Channel not released! Bad refcount");
		}
	}
	m.clear();
}

