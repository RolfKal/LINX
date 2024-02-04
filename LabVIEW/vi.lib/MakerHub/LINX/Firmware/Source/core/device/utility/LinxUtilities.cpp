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
#include <stdio.h>
#include <string.h>
#include "LinxDefines.h"
#if Unix
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif
#include "LinxUtilities.h"

uint32_t WriteU8ToBuff(uint8_t *buffer, uint32_t offset, uint8_t val)
{
	buffer[offset++] = val;
	return offset;
}

uint32_t WriteU16ToBuff(uint8_t *buffer, uint32_t offset, uint16_t val)
{
	buffer[offset++] = ((val >> 8) & 0xFF);
	buffer[offset++] = (val & 0xFF);
	return offset;
}

uint32_t WriteU32ToBuff(uint8_t *buffer, uint32_t offset, uint32_t val)
{
	buffer[offset++] = ((val >> 24) & 0xFF);
	buffer[offset++] = ((val >> 16) & 0xFF);
	buffer[offset++] = ((val >> 8) & 0xFF);
	buffer[offset++] = (val & 0xFF);
	return offset;
}

uint32_t WriteU8ArrToBuff(uint8_t *buffer, uint32_t offset, uint8_t *arr, int32_t length)
{
	if (length < 0)
		length = (int32_t)strlen((char*)arr);
	memcpy(buffer + offset, arr, length);
	return offset + length;
}

uint8_t GetU8FromBuff(uint8_t *buffer, uint32_t offset)
{
	return buffer[offset];
}

uint16_t GetU16FromBuff(uint8_t *buffer, uint32_t offset)
{
	return (((uint16_t)buffer[offset + 0] << 8) |
		    ((uint16_t)buffer[offset + 1]));
}

uint32_t GetU32FromBuff(uint8_t *buffer, uint32_t offset)
{
	return (((uint32_t)buffer[offset + 0] << 24) |
		    ((uint32_t)buffer[offset + 1] << 16) |
			((uint32_t)buffer[offset + 2] << 8) |
		    ((uint32_t)buffer[offset + 3]));
}

uint32_t ReadU8FromBuff(uint8_t *buffer, uint32_t offset, uint8_t *val)
{
	*val = buffer[offset++];
	return offset;
}

uint32_t ReadU16FromBuff(uint8_t *buffer, uint32_t offset, uint16_t *val)
{
	*val = (((uint32_t)buffer[offset + 0] << 8) |
		    ((uint32_t)buffer[offset + 1]));
	return offset + 2;
}

uint32_t ReadU32FromBuff(uint8_t *buffer, uint32_t offset, uint32_t *val)
{
	*val = (((uint32_t)buffer[offset + 0] << 24) |
		    ((uint32_t)buffer[offset + 1] << 16) |
		    ((uint32_t)buffer[offset + 2] << 8) |
		    ((uint32_t)buffer[offset + 3]));
	return offset + 4;
}

uint32_t ReadU8ArrFromBuff(uint8_t *buffer, uint32_t offset, uint8_t *arr, uint32_t length)
{
	memcpy(arr, buffer + offset, length);
	return offset + length;
}

uint32_t ReadStringFromBuff(uint8_t *buffer, uint32_t offset, unsigned char *arr, uint32_t length)
{
	memcpy(arr, buffer + offset, length);
	buffer[offset + length] = 0;
	return offset + length;
}

uint8_t ComputeChecksum(uint8_t checksum, uint8_t* buffer, int32_t length)
{
	for (uint16_t i = 0; i < length; i++)
	{
		checksum += buffer[i];
	}
	return checksum;
}

uint8_t ReverseBits(uint8_t b) 
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void ReverseBits(unsigned char *buffer, uint32_t length) 
{
	for (uint32_t i = 0; i < length; i++)
	{
		buffer[i] = ReverseBits(buffer[i]);
	}
}


#if Win32
static LARGE_INTEGER g_Frequency = {0};
static int32_t isAvailable = -1;
static int32_t initializeFrequency(void)
{
	if (isAvailable < 0)
	{
		isAvailable = QueryPerformanceFrequency(&g_Frequency) != 0;
		if (isAvailable)
			g_Frequency.QuadPart;
	}
	return isAvailable;
}
#endif

uint64_t getUsTicks(void)
{
#if Unix
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	return (((uint64_t)mTime.tv_sec * 1000000) + (mTime.tv_nsec / 1000));
#elif Win32
	if (initializeFrequency())
	{
		LARGE_INTEGER counter;
		if (QueryPerformanceCounter(&counter))
		{
			counter.QuadPart /= g_Frequency.QuadPart * 1000000;
			return counter.QuadPart;
		}
	}
	return GetTickCount64();
#elif Arduino
	return micros();
#endif
}

uint32_t getMsTicks(void)
{
#if Unix
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	return ((mTime.tv_sec * 1000) + (mTime.tv_nsec / 1000000));
#elif Win32
	if (initializeFrequency())
	{
		LARGE_INTEGER counter;
		if (QueryPerformanceCounter(&counter))
		{
			counter.QuadPart /= g_Frequency.QuadPart * 1000;
			return (uint32_t)(counter.QuadPart & UINT_MAX);
		}
	}
	return GetTickCount();
#elif Arduino
	return millis();
#endif
}

void delayMs(uint32_t ms)
{
#if Unix
	usleep(ms * 1000);
#elif Win32
	Sleep(ms);
#elif Arduino
	delay(ms);
#endif
}

// Return true If file specified by path exists.
int32_t fileExists(const char* path)
{
#if Unix
	struct stat buffer;
	return (stat(path, &buffer) == 0);
#elif Win32
	return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#else
	return false;
#endif
}

int32_t fileExists(const char* path, int32_t *length)
{
#if Unix
	struct stat buffer;
	int32_t ret = stat(path, &buffer);
	if (ret == 0 && length)
		*length = buffer.st_size;
	return (ret == 0);
#elif Win32
	WIN32_FIND_DATAA findBuf;
	HANDLE findHandle = FindFirstFileA(path, &findBuf);
    if (findHandle != INVALID_HANDLE_VALUE)
	{
		if (length)
			*length = findBuf.nFileSizeLow;
		FindClose(findHandle);
	}
	return findHandle != INVALID_HANDLE_VALUE;
#else
	return false;
#endif
}

int32_t fileExists(const char* directory, const char* fileName, uint32_t timeout)
{
	char fullPath[260];
	sprintf(fullPath, "%s%s", directory, fileName);
	uint32_t startTime = getMsTicks();
	do
	{
		if (fileExists(fullPath))
			return true;
		delayMs(10);
	}
	while (getMsTicks() - startTime < timeout);
	return false;
}

int32_t listDirectory(const char* path, std::list<std::string> &list)
{
#if Unix
	dirent* dp;
	DIR* handle = opendir(path);
	if (handle == NULL)
		return false;

	// Loop over all entries in directory path
	while ((dp = readdir(handle)) != NULL)
	{
		//Make Sure Dir Is Not . or ..
		if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
		{
			list.push_back(std::string(dp->d_name));
		}
	}
	closedir(handle);
#elif Win32
	WIN32_FIND_DATAA findData;
	size_t len = strlen(path);
	char *temp = (char*)alloca(len + 6);
	strcpy(temp, path);
	if (temp[len] == '\\') len--;
	strcpy(temp + len, "\\*.*");
	HANDLE handle = FindFirstFileA(temp, &findData);
	if (handle == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		//Make Sure Dir Is Not . or ..
		if ((strcmp(findData.cFileName, ".") != 0) && (strcmp(findData.cFileName, "..") != 0))
		{
			list.push_back(std::string(findData.cFileName));
		}
	}
	while (FindNextFileA(handle, &findData));
	FindClose(handle);
#endif
	return true;
}