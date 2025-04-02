// Kernel32 Forward
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define EXPORT_MANDATORY_DEBUG(name) EXPORT_MANDATORY(name)
#define EXPORT_MANDATORY(name) void* P_ ## name = NULL
#define EXPORT_OPTIONAL(name) extern void PL_ ## name (void); void* P_ ## name = NULL

#include "forward.h"
EXPORT_MANDATORY_DEBUG(VirtualQuery);
EXPORT_MANDATORY_DEBUG(VirtualProtect);

#undef EXPORT_MANDATORY
#undef EXPORT_OPTIONAL

static char Initialized = 0;

static char* pointerToString(char* buff, void* x)
{
	uintptr_t xi = (uintptr_t) x;
	
	int k = 0;
	for (int j = 28; j >= 0; j -= 4)
	{
		buff[k++] = "0123456789ABCDEF"[(xi >> j) & 0xF];
	}
	
	buff[k++] = ' ';
	buff[k] = 0;
	
	return buff;
}

WINAPI void PrintIntsDebug(void* x1, void* x2, void* x3, void* x4)
{
	char buffer[256], buffer2[32];
	buffer[0] = 0;
	
	strcat(buffer, pointerToString(buffer2, x1));
	strcat(buffer, pointerToString(buffer2, x2));
	strcat(buffer, pointerToString(buffer2, x3));
	strcat(buffer, pointerToString(buffer2, x4));
	strcat(buffer, "\n");
	
	OutputDebugStringA(buffer);
}

// HACK HACK HACK HACK
//
// Okay, so it seems on Windows 9x, right after calling VirtualProtect,
// the range pointed to by the base address of the code section has increased!
// 
// And not by little!
// Here's a hack I will do. I will reserve a little memory so that I can
// remember the old size and pass that into VirtualProtect.
#define MAX_ENTRIES 32
static struct {
	LPCVOID BaseAddress;
	size_t  ReportedSize;
}
VirtualQueryHackEntries[MAX_ENTRIES];

WINAPI SIZE_T VirtualQuery(LPCVOID address, PMEMORY_BASIC_INFORMATION info, SIZE_T length)
{
	typedef SIZE_T(WINAPI* PVQ)(LPCVOID, PMEMORY_BASIC_INFORMATION, SIZE_T);
	
	SIZE_T rv = ((PVQ)P_VirtualQuery)(address, info, length);
	
	// check if there is already a reported size
	bool hasReportedSize = false;
	for (int i = 0; i < MAX_ENTRIES; i++)
	{
		if (VirtualQueryHackEntries[i].BaseAddress == address)
		{
			info->RegionSize = VirtualQueryHackEntries[i].ReportedSize;
			hasReportedSize = true;
			break;
		}
	}
	
	if (!hasReportedSize)
	{
		// try adding
		for (int i = 0; i < MAX_ENTRIES; i++)
		{
			if (VirtualQueryHackEntries[i].BaseAddress != 0)
				continue;
			
			VirtualQueryHackEntries[i].BaseAddress = address;
			VirtualQueryHackEntries[i].ReportedSize = info->RegionSize;
			break;
		}
	}
	
	return rv;
}

WINAPI BOOL VirtualProtect(LPVOID address, SIZE_T size, DWORD newProtect, PDWORD oldProtect)
{
	typedef BOOL(WINAPI* PVP)(LPVOID, SIZE_T, DWORD, PDWORD);
	
	return ((PVP)P_VirtualProtect)(address, size, newProtect, oldProtect);
}

// note: Returns FALSE always.
BOOL __attribute__((dllexport)) WINAPI
ErrorMessage(const char* message, const char* inmessage, const char* aftermessage)
{
	char buffer[2048];
	strcpy(buffer, message);
	if (inmessage)    strcat(buffer, inmessage);
	if (aftermessage) strcat(buffer, aftermessage);
	
	MessageBoxA(NULL, buffer, "DiscordMessenger Kernel32 Forwarder", MB_ICONERROR);
	
	return FALSE;
}

BOOL PrepareExport(HMODULE hmod, void** fnout, const char* name, void* fnph)
{
	*fnout = GetProcAddress(hmod, name);
	if (!*fnout)
		*fnout = fnph;
	
	if (!*fnout)
		return ErrorMessage("Could not find mandatory export function ", name, " in the kernel32.dll module.  Let iProgramInCpp know!");
	
	return TRUE;
}

BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			if (Initialized)
				break;
			
			Initialized = 1;
			
			HMODULE hmod = GetModuleHandleA("kernel32.dll");
			if (!hmod)
				return ErrorMessage("Could not get module handler for kernel32.dll.", NULL, NULL);
			
			const char *c = "Could not find mandatory export function ", *d = " in the kernel32.dll module.  Let iProgramInCpp know!";
			void* fn;
			
			#define EXPORT_MANDATORY(name) do {                   \
				if (!PrepareExport(hmod, &P_##name, #name, NULL)) \
					return FALSE;                                 \
			} while (0)
			
			#define EXPORT_OPTIONAL(name) do {                          \
				if (!PrepareExport(hmod, &P_##name, #name, &PL_##name)) \
					return FALSE;                                       \
			} while (0)
			
			#include "forward.h"
			EXPORT_MANDATORY_DEBUG(VirtualQuery);
			EXPORT_MANDATORY_DEBUG(VirtualProtect);
			
			#undef EXPORT_MANDATORY
			#undef EXPORT_OPTIONAL
			break;
		}
	}
	
	return TRUE;
}

BOOL WINAPI
DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return DllMain(hinstDLL, fdwReason, lpvReserved);
}


