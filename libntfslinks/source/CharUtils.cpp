#include "CharUtils.h"

#include <strsafe.h>

namespace libntfslinks
{

/**
 * Utilty function for converting a WCHAR string to a TCHAR string.
 */
bool WCHARtoTCHAR(WCHAR* src, size_t srcSize, TCHAR* dest, size_t destSize)
{
#ifdef UNICODE
	return StringCchCopy(dest, min(srcSize+1, destSize), (TCHAR*)src) == S_OK;
#else
	return WideCharToMultiByte(CP_UTF8, 0, src, (int)srcSize, dest, (int)destSize, NULL, NULL) != 0;
#endif
}

/**
 * Utility function for converting a TCHAR string to a WCHAR string.
 */
bool TCHARtoWCHAR(TCHAR* src, size_t srcSize, WCHAR* dest, size_t destSize)
{
#ifdef UNICODE
	return StringCchCopy(dest, min(srcSize+1, destSize), (TCHAR*)src) == S_OK;
#else
	return MultiByteToWideChar(CP_UTF8, 0, src, (int)srcSize, dest, (int)destSize) != 0;
#endif
}

} // namespace libntfslinks