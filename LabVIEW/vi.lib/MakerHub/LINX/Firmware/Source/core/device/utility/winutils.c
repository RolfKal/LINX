#include <ntstatus.h>
#define WIN32_NO_STATUS
#include <windows.h>
#include <winternl.h>

#define REG_KEY_PATH_LENGTH 1024

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation,
    KeyCachedInformation,
    KeyFlagsInformation,
    KeyVirtualizationInformation,
    KeyHandleTagsInformation,
    KeyTrustInformation,
    KeyLayerInformation,
    MaxKeyInfoClass
} KEY_INFORMATION_CLASS;

typedef struct _KEY_NAME_INFORMATION {
    ULONG NameLength;
    WCHAR Name[1];
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef NTSTATUS (NTAPI *PFN_NtQueryKey)(
    __in        HANDLE  /* KeyHandle */,
    __in        KEY_INFORMATION_CLASS /* KeyInformationClass */,
    __out_opt   PVOID   /* KeyInformation */,
    __in        ULONG   /* Length */,
    __out       ULONG * /* ResultLength */
);

DWORD RegQueryKeyPath(HKEY hKey, LPWSTR string, ULONG strLen)
{
    NTSTATUS status;
    if (hKey != NULL)
    {
        HMODULE hinstDLL = GetModuleHandleW(L"ntdll.dll");
        if (hinstDLL != NULL)
        {
            PFN_NtQueryKey pfn = (PFN_NtQueryKey)GetProcAddress(hinstDLL, "NtQueryKey");
            if (pfn != NULL)
            {
                ULONG length = FIELD_OFFSET(KEY_NAME_INFORMATION, Name) + sizeof(WCHAR) * REG_KEY_PATH_LENGTH;
                LPWSTR buffer = (LPWSTR)malloc(length);
                KEY_NAME_INFORMATION *pkni;

            TryAgain:
                status = (pfn)(hKey, KeyNameInformation, buffer, length, &length);
                switch (status)
				{
					case STATUS_BUFFER_TOO_SMALL:
					case STATUS_BUFFER_OVERFLOW:
						buffer = realloc(buffer, length);
						goto TryAgain;
					case STATUS_SUCCESS:
						pkni = (KEY_NAME_INFORMATION *)buffer;
						if (strLen > pkni->NameLength / sizeof(WCHAR))
							strLen = pkni->NameLength / sizeof(WCHAR);
						else
							strLen--;
						wcsncpy(string, pkni->Name, strLen);
						string[strLen] = 0;
					default:
						break;
				}
				free(buffer);
            }
        }
    }
    return status;
}
