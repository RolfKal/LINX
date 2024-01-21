/****************************************************************************************
**  LINX - Generic LINX channel implementation.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "LinxDefines.h"
#if Unix
#include <alloca.h>
//#include <sys/types.h>
#include <sys/socket.h>
//#include <unistd.h>
#include <arpa/inet.h>
#elif Win32
#include <malloc.h>
#endif
#include "LinxChannel.h" 
#include "LinxUtilities.h"

LinxChannel::LinxChannel(const unsigned char *channelName)
{
	m_Debug = NULL;
	m_ChannelName = (char*)malloc(strlen((char*)channelName) + 1);
	strcpy(m_ChannelName, (char*)channelName);
	// Start with refcount 1
	m_Refcount = 1;
}

LinxChannel::LinxChannel(LinxFmtChannel *debug, const unsigned char *channelName)
{
	if (debug)
	{
		debug->AddRef();
	}
	else
	{
		debug = new LinxFmtChannel();
	}
	m_Debug = debug;
	m_ChannelName = (char*)malloc(strlen((char*)channelName) + 1);
	strcpy(m_ChannelName, (char*)channelName);
	// Start with refcount 1
	m_Refcount = 1;
}

LinxChannel::~LinxChannel()
{
	free(m_ChannelName);
	if (m_Debug)
		m_Debug->Release();
}

unsigned int LinxChannel::AddRef()
{
#if Win32
	return InterlockedIncrement(&m_Refcount);
#elif Unix
	return __sync_fetch_and_add(&m_Refcount, 1);
#endif
}

unsigned int LinxChannel::Release()
{
	unsigned int refcount = 
#if Win32
	InterlockedDecrement(&m_Refcount);
#elif Unix
	__sync_sub_and_fetch(&m_Refcount, 1);
#endif
	if (!refcount)
		delete this;
	return refcount;
}

unsigned char LinxChannel::GetName(unsigned char* buffer, unsigned char numBytes)
{
	unsigned char len = (unsigned char)strlen(m_ChannelName);
	if (buffer)
	{
		strncpy((char*)buffer, m_ChannelName, numBytes);
	}
	return len;
}

LinxFmtChannel::LinxFmtChannel(int timeout, const unsigned char *channelName) : LinxChannel(channelName)
{
	m_Timeout = timeout;
	m_Channel = NULL;
}

LinxFmtChannel::LinxFmtChannel(LinxCommChannel *channel, int timeout, const unsigned char *channelName) : LinxChannel(channelName)
{
	m_Timeout = timeout;
	if (channel)
		channel->AddRef();
	m_Channel = channel;
}

LinxFmtChannel::~LinxFmtChannel()
{
	if (m_Channel)
	{
		m_Channel->Close();
		m_Channel->Release();
	}
}

int LinxFmtChannel::Write(char c)
{
	if (m_Channel)
		return m_Channel->Write((const unsigned char*)&c, 1, m_Timeout);
	return L_OK;
}

int LinxFmtChannel::Write(const char s[], int len)
{
	if (m_Channel)
		return m_Channel->Write((const unsigned char*)s, (int)strlen(s), m_Timeout);
	return L_OK;
}

int LinxFmtChannel::Write(const char s[])
{
	return Write(s, (int)strlen(s));
}

int LinxFmtChannel::Write(unsigned char c)
{
	if (m_Channel)
		return m_Channel->Write(&c, 1, m_Timeout);
	return L_OK;
}

int LinxFmtChannel::Write(int n)
{
	return Write((long)n);
}

int LinxFmtChannel::Write(unsigned int n)
{
	return Write((unsigned long)n);
}

int LinxFmtChannel::Write(long n)
{
	int status = L_OK;
	if (n < 0) 
	{
		status = Write('-');
		n = -n;
	}
	if (!status)
		status = WriteNumber(n, 10);
	return status;
}

int LinxFmtChannel::Write(unsigned long n)
{
	return  WriteNumber(n, 10);
}

int LinxFmtChannel::Write(long n, int base)
{
	if (base == 0)
	{
		return Write((char)n);
	}
	else if (base == 10)
	{
		return Write(n);
	}
	int status = L_OK;
	if (n < 0) 
	{
		status = Write('-');
		n = -n;
	}
	if (!status)
		return WriteNumber(n, base);
	return status;
}

int LinxFmtChannel::Writeln()
{
	if (m_Channel)
		return m_Channel->Write((const unsigned char*)"\r\n", 2, m_Timeout);
	return L_OK;
}

int LinxFmtChannel::Writeln(char c)
{
	if (m_Channel)
	{
		int status = Write(c);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(const char s[])
{
	if (m_Channel)
	{
		int status = Write(s);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(unsigned char c)
{
	if (m_Channel)
	{
		int status = Write(c);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(int n)
{
	if (m_Channel)
	{
		int status = Write(n);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(long n)
{
	if (m_Channel)
	{
		int status = Write(n);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(unsigned long n)
{
	if (m_Channel)
	{
		int status = Write(n);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Writeln(long n, int base)
{
	if (m_Channel)
	{
		int status = Write(n, base);
		if (!status)
			status = Writeln();
		return status;
	}
	return L_OK;
}

int LinxFmtChannel::Close()
{
	if (m_Channel)
		return m_Channel->Close();
	return L_OK;
}

int LinxFmtChannel::SetTimeout(int timeout)
{
	m_Timeout = timeout;
	return L_OK;
}

int LinxFmtChannel::SetDebugChannel(LinxCommChannel *channel)
{
	if (m_Channel)
		m_Channel->Release();
	if (channel)
		channel->AddRef();
	m_Channel = channel;
	return L_OK;
}

int LinxFmtChannel::WriteNumber(unsigned long n, unsigned char base)
{
	unsigned int i, e = 8 * sizeof(long);
	unsigned char *buf = (unsigned char*)alloca(e); // Assumes 8-bit chars. 

	if (n == 0) 
	{
		return Write('0');
	} 

	i = e;
	while (n > 0) 
	{
		unsigned char tmp = (unsigned char)(n % base);
		buf[--i] = tmp < 10 ? '0' + tmp : 'A' + tmp - 10;
		n /= base;
	}
	return Write((char*)buf + i, e - i);
}
