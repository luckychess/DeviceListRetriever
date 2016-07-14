// main.h - listing system devices and their DACL strings (if applicable)
// please run with administrator privilegies
#include <windows.h>
#include <Subauth.h>
#include <Aclapi.h>
#include <wtypes.h>
#include <Sddl.h>

#include <stdio.h>

#include "main.h"

int main()
{
	HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
	NTOPENDIRECTORYOBJECT NtOpenDirectoryObject = (NTOPENDIRECTORYOBJECT)GetProcAddress(hNtdll, "NtOpenDirectoryObject");
	NTQUERYDIRECTORYOBJECT NtQueryDirectoryObject = (NTQUERYDIRECTORYOBJECT)GetProcAddress(hNtdll, "NtQueryDirectoryObject");
	NTOPENFILE NtOpenFile = (NTOPENFILE)GetProcAddress(hNtdll, "NtOpenFile");
	if (NtOpenDirectoryObject && NtQueryDirectoryObject && NtOpenFile)
	{
		HANDLE dirHandle = NULL;
		OBJECT_ATTRIBUTES dirObj;
		UNICODE_STRING dirStr;
		WCHAR *uString = L"\\Device";
		dirStr.Length = lstrlenW(uString) * sizeof(WCHAR);
		dirStr.MaximumLength = dirStr.Length;
		dirStr.Buffer = uString;

		InitializeObjectAttributes(&dirObj, &dirStr, OBJ_CASE_INSENSITIVE, NULL, NULL);
		NTSTATUS ntStatus = NtOpenDirectoryObject(&dirHandle, 0x01, &dirObj);
		if (ntStatus == STATUS_SUCCESS)
		{
			bool restart = true;
			int bufferSize = 1024 * sizeof(OBJECT_DIRECTORY_INFORMATION);
			PUCHAR pBuffer = (PUCHAR)LocalAlloc(LPTR, bufferSize);
			ULONG context, returnLength;
			do
			{
				ntStatus = NtQueryDirectoryObject(dirHandle, pBuffer, bufferSize, false, restart, &context, &returnLength);
				restart = false;
				OBJECT_DIRECTORY_INFORMATION *pObjectDirectoryInformation = (OBJECT_DIRECTORY_INFORMATION*)pBuffer;
				for (int i = 0; pObjectDirectoryInformation[i].Name.Buffer; ++i)
				{
					printf("%S --- ", pObjectDirectoryInformation[i].Name.Buffer);
					HANDLE fileHandle;
					NTSTATUS fileOpenStatus;
					OBJECT_ATTRIBUTES fileObj;
					UNICODE_STRING fileStr;
					WCHAR fString[100] = L"\\Device\\";
					wcscat_s(fString, pObjectDirectoryInformation[i].Name.Buffer);
					fileStr.Length = lstrlenW(fString) * sizeof(WCHAR);
					fileStr.MaximumLength = fileStr.Length;
					fileStr.Buffer = fString;
					InitializeObjectAttributes(&fileObj, &fileStr, OBJ_CASE_INSENSITIVE, NULL, NULL);
					IO_STATUS_BLOCK fileStatusBlock;

					fileOpenStatus = NtOpenFile(&fileHandle, FILE_READ_DATA | FILE_READ_ATTRIBUTES, &fileObj, &fileStatusBlock, FILE_SHARE_READ, 0);
					if (fileOpenStatus == STATUS_SUCCESS)
					{
						PSECURITY_DESCRIPTOR secDesc;

						int res = GetSecurityInfo(fileHandle, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, 0, 0, 0, 0, &secDesc);
						
						if (res != ERROR_SUCCESS)
						{
							printf("Unable to get security data\n");
						}
						else
						{
							LPWSTR DACLDescriptorAsString;
							DWORD SecDescStringLengthNeeded = 0;
							res = ConvertSecurityDescriptorToStringSecurityDescriptor(secDesc, SDDL_REVISION_1,
								DACL_SECURITY_INFORMATION, &DACLDescriptorAsString, &SecDescStringLengthNeeded);
							if (res)
							{
								printf("%ws\n", DACLDescriptorAsString);
							}
							else
							{
								printf("sec string conv error\n");
							}
							LocalFree(secDesc);
						}

						CloseHandle(fileHandle);
					}
					else
					{
						printf("device open error\n");
					}
				}
			}
			while (ntStatus == STATUS_MORE_ENTRIES);
		}
	}
	system("pause");
}