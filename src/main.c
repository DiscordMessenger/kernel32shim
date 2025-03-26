// Kernel32 Forward
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stddef.h>

#define EXPORT_MANDATORY(name) void* P_ ## name = NULL
#define EXPORT_OPTIONAL(name) extern void PL_ ## name (void); void* P_ ## name = NULL

#include "forward.h"

#undef EXPORT_MANDATORY
#undef EXPORT_OPTIONAL

static char Initialized = 0;

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


