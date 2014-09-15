#ifndef STRINGUTILS_H
#define STRINGUTILS_H
#pragma once

#include <Windows.h>

/**
 * Finds the first occurrence of the string Sub in string Str.
 *
 * @param Str The string to search for the substring.
 * @param Sub The string to search for in Str.
 * @param StartIdx The index of Str to begin the search from. Default is 0.
 * @param Dir The direction to perform the search in. Set to 1 for a left-to-right search, set to -1 for a
 *		right-to-left search.
 * @return Returns the starting index of the substring in Str or -1 if not found.
 */
int StrFind(LPCTSTR Str, LPCTSTR Sub, int StartIdx = 0, int Dir = 1);

/**
 * Searches a string for the first occurrence of a provided search string and replaces it with another.
 *
 * @param SrcStr The source string to search for the substring and perform replacement on.
 * @param Search The substring to search for.
 * @param Replace The string to replace the substring with.
 * @param DestStr The destination to write the resulting string to.
 * @param Dir The direction to perform the search in. Set to 1 for a left-to-right search, set to -1 for a
 *		right-to-left search.
 * @return Returns true if the operation was successful, otherwise false.
 */
bool StrReplace(LPCTSTR SrcStr, LPCTSTR Search, LPCTSTR Replace, LPTSTR DestStr, int Dir = 1);

#endif //STRINGUTILS_H