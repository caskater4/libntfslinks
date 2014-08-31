#ifndef CHARUTILS_H
#define CHARUTILS_H
#pragma once

#include <Windows.h>

namespace libntfslinks
{

/**
 * Utilty function for converting a WCHAR string to a TCHAR string.
 */
bool WCHARtoTCHAR(WCHAR* src, size_t srcSize, TCHAR* dest, size_t destSize);

/**
 * Utility function for converting a TCHAR string to a WCHAR string.
 */
bool TCHARtoWCHAR(TCHAR* src, size_t srcSize, WCHAR* dest, size_t destSize);

} // namespace libntfslinks

#endif //CHARUTILS_H