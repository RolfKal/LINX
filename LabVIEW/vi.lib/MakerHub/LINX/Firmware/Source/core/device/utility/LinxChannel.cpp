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

LinxChannel::LinxChannel(LinxFmtChannel *debug, const char *channelName)
{
	m_Debug = debug;
	if (!m_Debug)
		m_Debug = new LinxFmtChannel();
	else
		m_Debug->AddRef();
	m_ChannelName = (char*)malloc(strlen(channelName) + 1);
	strcpy(m_ChannelName, channelName);
	// Start with refcount 1
	m_Refcount = 1;
}

LinxChannel::~LinxChannel()
{
	free(m_ChannelName);
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

int LinxChannel::GetName(char* buffer, unsigned char numBytes)
{
	size_t len = strlen(m_ChannelName);
	if (numBytes > len)
		strcpy(buffer, m_ChannelName);
	else
		strncpy(buffer, m_ChannelName, numBytes);
	return L_OK;
}

LinxFmtChannel::LinxFmtChannel() : LinxCommChannel(NULL, "FormatChannel")
{
	m_Channel = NULL;
}

LinxFmtChannel::LinxFmtChannel(LinxCommChannel *channel) : LinxCommChannel(NULL, "FormatChannel")
{
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

int LinxFmtChannel::Read(unsigned char* recBuffer, int numBytes, int timeout, int* numBytesRead)
{
	if (m_Channel)
		return m_Channel->Read(recBuffer, numBytes, timeout, numBytesRead);
	return L_OK;
}

int LinxFmtChannel::Write(unsigned char* sendBuffer, int numBytes, int timeout)
{
	if (m_Channel)
		return m_Channel->Write(sendBuffer, numBytes, timeout);
	return L_OK;
}

int LinxFmtChannel::Write(char c)
{
	if (m_Channel)
		return m_Channel->Write((unsigned char*)&c, 1, TIMEOUT_INFINITE);
	return L_OK;
}

int LinxFmtChannel::Write(const char s[])
{
	if (m_Channel)
		return m_Channel->Write((unsigned char*)s, (int)strlen(s), TIMEOUT_INFINITE);
	return L_OK;
}

int LinxFmtChannel::Write(unsigned char c)
{
	if (m_Channel)
		return m_Channel->Write(&c, 1, TIMEOUT_INFINITE);
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
		status = Write((unsigned char*)"-", 1, TIMEOUT_INFINITE);
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
		return Write((unsigned char*)&n, 1, TIMEOUT_INFINITE);
	}
	else if (base == 10)
	{
		return Write(n);
	}
	int status = L_OK;
	if (n < 0) 
	{
		status = Write((unsigned char*)"-", 1, TIMEOUT_INFINITE);
		n = -n;
	}
	if (!status)
		return WriteNumber(n, base);
	return status;
}

int LinxFmtChannel::Writeln()
{
	if (m_Channel)
		return m_Channel->Write((unsigned char*)"\r\n", 2, TIMEOUT_INFINITE);
	return L_OK;
}

int LinxFmtChannel::Writeln(char c)
{
	if (m_Channel)
	{
		int status = m_Channel->Write((unsigned char*)&c, 1, TIMEOUT_INFINITE);
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
		int status = m_Channel->Write((unsigned char*)s, (int)strlen(s), TIMEOUT_INFINITE);
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
		int status = m_Channel->Write(&c, 1, TIMEOUT_INFINITE);
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
		int status = Write((long)n);
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

int LinxFmtChannel::WriteNumber(unsigned long n, unsigned char base)
{
	unsigned int i, e = 8 * sizeof(long);
	unsigned char *buf = (unsigned char*)alloca(e); // Assumes 8-bit chars. 

	if (n == 0) 
	{
		return m_Channel->Write((unsigned char*)"0", 1, TIMEOUT_INFINITE);
	} 

	i = e;
	while (n > 0) 
	{
		unsigned char tmp = (unsigned char)(n % base);
		buf[--i] = tmp < 10 ? '0' + tmp : 'A' + tmp - 10;
		n /= base;
	}
	return m_Channel->Write(buf + i, e - i, TIMEOUT_INFINITE);
}

int LinxFmtChannel::SetChannel(LinxCommChannel *channel)
{
	if (m_Channel)
		m_Channel->Release();
	m_Channel = channel;
	if (m_Channel)
		m_Channel->AddRef();
	return L_OK;
}
