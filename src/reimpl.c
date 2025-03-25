#define WIN32_LEAN_AND_MEAN
#include <windows.h>

WINAPI BOOL PL_GetVersionExA(LPOSVERSIONINFOA lpvi)
{
	DWORD oldsize = lpvi->dwOSVersionInfoSize;
	memset(lpvi, 0, sizeof * lpvi);

	DWORD ver = GetVersion();
	lpvi->dwOSVersionInfoSize = oldsize;
	lpvi->dwMajorVersion = LOBYTE(ver);
	lpvi->dwMinorVersion = HIBYTE(LOWORD(ver));
	lpvi->dwBuildNumber = HIWORD(ver);
	// TODO: detect if this is NT
	lpvi->dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;

	strcpy(lpvi->szCSDVersion, "Kernel32Forwarder");
	return TRUE;
}

// we **DO NOT** care about the Windows W/A definition for this
#ifdef GetEnvironmentStrings
#undef GetEnvironmentStrings
#endif

// The NT 3.1 docs say this is a LPVOID, btw.
WINBASEAPI LPCH WINAPI GetEnvironmentStrings(VOID);

static char  __envStrsBuf[4096];
static WCHAR __unicodeToAnsiTemp[131072]; // More than enough I feel!

WINAPI LPWCH PL_GetEnvironmentStringsW(VOID)
{
	// WinNT 3.51 just returns what's part of the PEB which
	// means we're definitely getting a wide string.
	return (LPWCH) GetEnvironmentStrings();
}

/*
WINAPI LPCH PL_GetEnvironmentStringsA(VOID)
{
	// Since GetEnvironmentStringsW is trivial, means this isn't.
	LPWCH envStrs = GetEnvironmentStrings();
	
	__envStrsBuf[sizeof __envStrsBuf - 1] = 0;
	
	size_t i = 0;
	for (; i < sizeof __envStrsBuf - 1 && *envStrs; i++, envStrs++)
	{
		if (*envStrs > 126)
			__envStrsBuf[i] = '?';
		else
			__envStrsBuf[i] = (char) *envStrs;
	}
	
	return &__envStrsBuf;
}
*/

WINAPI void PL_FreeEnvironmentStringsW()
{
}

WINAPI void PL_FreeEnvironmentStringsA()
{
}

// now the important thing is that we CANNOT allocate anything ourselves.
// this is real unfortunately as I would have been fine with allocating stuff
// to "inflate" the string and stuff.
//
// for now, these just return OoM. (That's technically true.)
// MSVCRT calls GetStringTypeW first anyway.
WINAPI BOOL PL_GetStringTypeA(LCID Locale, DWORD dwInfoType, LPCSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
{
	SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	return FALSE;
}

WINAPI BOOL PL_LCMapStringA(LCID Locale, DWORD dwMapFlags, LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest)
{
	SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	return FALSE;
}

WINAPI int PL_CompareStringA(LCID Locale, DWORD dwCmpFlags, const char* lpString1, int cchCount1, const char* lpString2, int cchCount2)
{
	size_t str1Size, str2Size;
	if (cchCount1 >= 0)
		str1Size = (size_t) cchCount1;
	else
		str1Size = strlen(lpString1);
	
	if (cchCount2 >= 0)
		str2Size = (size_t) cchCount2;
	else
		str2Size = strlen(lpString2);
	
	size_t minimumSize = str1Size;
	if (minimumSize > str2Size)
		minimumSize = str2Size;
	
	// also compare the null terminator which SHOULD work
	return memcmp(lpString1, lpString2, minimumSize + 1);
}

WINAPI BOOL PL_IsValidLocale(LCID Locale, DWORD dwFlags)
{
	// whatever
	return TRUE;
}

int PL_GetLocaleInfoA(LCID Locale, LCTYPE lcType, LPSTR lpData, int cchData)
{
	__unicodeToAnsiTemp[0] = 0;
	int lci = GetLocaleInfoW(Locale, lcType, __unicodeToAnsiTemp, sizeof __unicodeToAnsiTemp);
	if (lci != 0)
		WideCharToMultiByte(CP_UTF8, 0, __unicodeToAnsiTemp, -1, lpData, cchData, NULL, NULL);
	
	return lci;
}

BOOL PL_EnumSystemLocalesA(LOCALE_ENUMPROCA enumProc, DWORD dwFlags)
{
	// assume there is just one locale.
	enumProc("Kernel32ForwarderLocale");
	return TRUE;
}

// finally over with this localization crap
BOOL PL_HeapValidate(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
	// all good chief
	return TRUE;
}

SIZE_T PL_HeapCompact(HANDLE hHeap, DWORD dwFlags)
{
	// don't know how to
	SetLastError(NO_ERROR);
	return 0;
}

BOOL PL_HeapWalk(HANDLE hHeap, LPPROCESS_HEAP_ENTRY lpProcessHeapEntry)
{
	// no
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

void PL_GetSystemTimeAsFileTime(LPFILETIME lpFileTime)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, lpFileTime);
}

BOOL PL_IsDBCSLeadByteEx(UINT codePage, BYTE testChar)
{
	return IsDBCSLeadByte(testChar);
}
