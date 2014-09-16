#include "StringUtils.h"

#include <strsafe.h>

int StrFind(LPCTSTR Str, LPCTSTR Sub, int StartIdx, int Dir)
{
	int result = -1;

	// Grab the length of the string to search
	size_t StrLength;
	if (FAILED(StringCchLength(Str, MAX_PATH, &StrLength)) || StrLength == 0)
	{
		return result;
	}

	// Grab the length of the substring
	size_t SubLength;
	if (FAILED(StringCchLength(Sub, MAX_PATH, &SubLength)) || SubLength == 0)
	{
		return result;
	}

	// When the substring is longer it is not possible to find it
	if (SubLength > StrLength)
	{
		return result;
	}

	// Make sure the StartIdx is not beyond StrLength
	if (StartIdx >= (int)StrLength)
	{
		return result;
	}

	int StrIdx = min(StartIdx, (int)StrLength-1);
	int SubIdx = Dir < 0 ? (int)SubLength - 1 : 0;
	for (; StrIdx >= 0 && StrIdx < (int)StrLength; StrIdx = StrIdx + Dir)
	{
		if (Str[StrIdx] == Sub[SubIdx])
		{
			if (result < 0)
			{
				result = StrIdx;
			}

			SubIdx = SubIdx + Dir;
		}
		else if (result >= 0 && (StrIdx - result) * Dir >= (int)SubLength)
		{
			break;
		}
		else
		{
			result = -1;
			SubIdx = Dir < 0 ? (int)SubLength - 1 : 0;
		}
	}

	// When searching in reverse correct the starting index
	if (result >= 0 && Dir < 0)
	{
		result = result - (int)SubLength + 1;
	}

	return result;
}

bool StrReplace(LPCTSTR SrcStr, LPCTSTR Search, LPCTSTR Replace, LPTSTR DestStr, int Dir)
{
	// Determine the length of SrcStr
	size_t SrcStrLength;
	if (FAILED(StringCchLength(SrcStr, MAX_PATH, &SrcStrLength)) || SrcStrLength == 0)
	{
		return false;
	}

	// Determine the length of Search
	size_t SearchLength;
	if (FAILED(StringCchLength(Search, MAX_PATH, &SearchLength)) || SearchLength == 0)
	{
		return false;
	}

	// Find the starting index of Search in SrcStr
	int StartIdx = StrFind(SrcStr, Search, Dir < 0 ? (int)SrcStrLength-1 : 0, Dir);
	if (StartIdx < 0)
	{
		return true;
	}

	// Copy the beginning of SrcStr and swap out the Search for Replace
	StringCchCopy(DestStr, StartIdx+1, SrcStr);
	StringCchCat(DestStr, MAX_PATH, Replace);
	StringCchCat(DestStr, MAX_PATH, &SrcStr[StartIdx+SearchLength]);

	return true;
}