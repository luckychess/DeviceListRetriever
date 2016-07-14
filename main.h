// main.h - different typedefs and macroces
#ifndef _MAIN_H_
#define _MAIN_H_

#define STATUS_MORE_ENTRIES             ((NTSTATUS)0x00000105L)
#define OBJ_CASE_INSENSITIVE			0x00000040L
#define OBJ_OPENIF						0x00000080L
#define FILE_NON_DIRECTORY_FILE			(0x00000040)

typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID    Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG           Length;
	HANDLE          RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG           Attributes;
	PVOID           SecurityDescriptor;
	PVOID           SecurityQualityOfService;
}  OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;


typedef NTSTATUS(WINAPI *NTOPENDIRECTORYOBJECT)(
	_Out_  PHANDLE DirectoryHandle,
	_In_   ACCESS_MASK DesiredAccess,
	_In_   POBJECT_ATTRIBUTES ObjectAttributes
	);

typedef NTSTATUS(WINAPI *NTQUERYDIRECTORYOBJECT)(
	_In_       HANDLE DirectoryHandle,
	_Out_opt_  PVOID Buffer,
	_In_       ULONG Length,
	_In_       BOOLEAN ReturnSingleEntry,
	_In_       BOOLEAN RestartScan,
	_Inout_    PULONG Context,
	_Out_opt_  PULONG ReturnLength
	);

typedef NTSTATUS(_stdcall *NTOPENFILE)(
	_Out_ PHANDLE            FileHandle,
	_In_  ACCESS_MASK        DesiredAccess,
	_In_  POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_ PIO_STATUS_BLOCK   IoStatusBlock,
	_In_  ULONG              ShareAccess,
	_In_  ULONG              OpenOptions
	);

typedef struct _OBJECT_DIRECTORY_INFORMATION {
	UNICODE_STRING Name;
	UNICODE_STRING TypeName;
} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

#endif // _MAIN_H_