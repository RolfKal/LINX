/****************************************************************************************
**  LINX file for generic LINX Utilities.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**
**  Written By Rolf Kalbermatter
**
** BSD2 License.
****************************************************************************************/
#include "LinxDefines.h"
#include <stdio.h>
#include <string.h>
#if Unix
#include <sys/stat.h>
#elif Win32
#include <windows.h>
#endif
#include "LinxUtilities.h"

int WriteU8ToBuff(unsigned char *buffer, int offset, unsigned char val)
{
	buffer[offset++] = val;
	return offset;
}

int WriteU16ToBuff(unsigned char *buffer, int offset, unsigned short val)
{
	buffer[offset++] = ((val >> 8) & 0xFF);
	buffer[offset++] = (val & 0xFF);
	return offset;
}

int WriteU32ToBuff(unsigned char *buffer, int offset, unsigned int val)
{
	buffer[offset++] = ((val >> 24) & 0xFF);
	buffer[offset++] = ((val >> 16) & 0xFF);
	buffer[offset++] = ((val >> 8) & 0xFF);
	buffer[offset++] = (val & 0xFF);
	return offset;
}

int WriteU8ArrToBuff(unsigned char *buffer, int offset, unsigned char *arr, int length)
{
	if (length < 0)
		length = (int)strlen((char*)arr);
	memcpy(buffer + offset, arr, length);
	return offset + length;
}

unsigned char GetU8FromBuff(unsigned char *buffer, int offset)
{
	return buffer[offset];
}

unsigned short GetU16FromBuff(unsigned char *buffer, int offset)
{
	return (((unsigned short)buffer[offset + 0] << 8) |
		    ((unsigned short)buffer[offset + 1]));
}

unsigned int GetU32FromBuff(unsigned char *buffer, int offset)
{
	return (((unsigned int)buffer[offset + 0] << 24) |
		    ((unsigned int)buffer[offset + 1] << 16) |
			((unsigned int)buffer[offset + 2] << 8) |
		    ((unsigned int)buffer[offset + 3]));
}

int ReadU8FromBuff(unsigned char *buffer, int offset, unsigned char *val)
{
	*val = buffer[offset++];
	return offset;
}

int ReadU16FromBuff(unsigned char *buffer, int offset, unsigned short *val)
{
	*val = (((unsigned int)buffer[offset + 0] << 8) |
		    ((unsigned int)buffer[offset + 1]));
	return offset + 2;
}

int ReadU32FromBuff(unsigned char *buffer, int offset, unsigned int *val)
{
	*val = (((unsigned int)buffer[offset + 0] << 24) |
		    ((unsigned int)buffer[offset + 1] << 16) |
		    ((unsigned int)buffer[offset + 2] << 8) |
		    ((unsigned int)buffer[offset + 3]));
	return offset + 4;
}

int ReadU8ArrFromBuff(unsigned char *buffer, int offset, unsigned char *arr, int length)
{
	memcpy(arr, buffer + offset, length);
	return offset + length;
}

int ReadStringFromBuff(unsigned char *buffer, int offset, unsigned char *arr, int length)
{
	memcpy(arr, buffer + offset, length);
	buffer[offset + length] = 0;
	return offset + length;
}


// Return true If file specified by path exists.
int fileExists(const char* path)
{
#if Unix
	struct stat buffer;
	return (stat(path, &buffer) == 0);
#elif Win32
	return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#endif
}

#if Win32
static LARGE_INTEGER g_Frequency = {0};
static int isAvailable = -1;
static int initializeFrequency()
{
	if (isAvailable < 0)
	{
		isAvailable = QueryPerformanceFrequency(&g_Frequency) != 0;
		if (isAvailable)
			g_Frequency.QuadPart /= 1000;
	}
	return isAvailable;
}
#endif

unsigned long long getUsTicks()
{
#if Unix
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	return (((unsigned long long)mTime.tv_sec * 1000000) + mTime.tv_nsec / 1000);
#elif Win32
	if (initializeFrequency())
	{
		LARGE_INTEGER counter;
		if (QueryPerformanceCounter(&counter))
		{
			counter.QuadPart *= 1000;
			counter.QuadPart /= g_Frequency.QuadPart;
			return counter.QuadPart;
		}
	}
	return GetTickCount();
#endif
}

void delayMs(unsigned int ms)
{
#if Unix
	usleep(ms * 1000);
#elif Win32
	Sleep(ms);
#endif
}

int fileExists(const char* path, int *length)
{
#if Unix
	struct stat buffer;
	int ret = stat(path, &buffer);
	if (ret == 0)
		*length = buffer.st_size;
	return (ret == 0);
#elif Win32
	WIN32_FIND_DATAA findBuf;
	HANDLE findhandle = FindFirstFileA(path, &findBuf);
    if (findhandle == INVALID_HANDLE_VALUE)
		*length = findBuf.nFileSizeLow;
	return findhandle != INVALID_HANDLE_VALUE;
#endif
}

int fileExists(const char* directory, const char* fileName)
{
	char fullPath[260];
	sprintf(fullPath, "%s%s", directory, fileName);
#if Unix
	struct stat buffer;
	return (stat(fullPath, &buffer) == 0);
#elif Win32
	return GetFileAttributesA(fullPath) != INVALID_FILE_ATTRIBUTES;
#endif
}

int fileExists(const char* directory, const char* fileName, unsigned int timeout)
{
	char fullPath[260];
	sprintf(fullPath, "%s%s", directory, fileName);
	unsigned int startTime = getMilliSeconds();
#if Unix
	struct stat buffer;
	while (getMilliSeconds()) - startTime < timeout)
	{
		if (stat(fullPath, &buffer) == 0)
			return TRUE;
		delayMs(10);
	}
#elif Win32
#endif
	while (getMilliSeconds() - startTime < timeout)
	{
		if (GetFileAttributesA(fullPath) != INVALID_FILE_ATTRIBUTES)
			return TRUE;
		delayMs(10);
	}
	return FALSE;
}

