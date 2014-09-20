///////////////////////////////////////////////////////////////////////////////
//
// This file is part of libntfslinks.
//
// Copyright (c) 2014, Jean-Philippe Steinmetz
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#include "Symlink.h"

#include <strsafe.h>
#include <winioctl.h>

#include "CharUtils.h"
#include "ntfstypes.h"
#include "StringUtils.h"

namespace libntfslinks
{

bool IsSymlink(LPCTSTR Path)
{
	bool result = false;
	HANDLE fileHandle;

	// We need an existing file handle in order to call DeviceIoControl
	fileHandle = CreateFile(Path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		// Allocate enough space to fit the maximum sized reparse data buffer
		char Tmp[MAXIMUM_REPARSE_DATA_BUFFER_SIZE] = {0};
		// Redefine the above char array into a REPARSE_DATA_BUFFER we can work with
		REPARSE_DATA_BUFFER& reparseData = *(REPARSE_DATA_BUFFER*)Tmp;

		// Call DeviceIoControl to get the reparse point data
		DWORD bytesRet;
		if (DeviceIoControl(fileHandle, FSCTL_GET_REPARSE_POINT, NULL, 0, &reparseData, sizeof(Tmp), &bytesRet, NULL))
		{
			// The reparse tag indicates if this is a symbolic link or not
			result = reparseData.ReparseTag == IO_REPARSE_TAG_SYMLINK;
		}

		// Close the handle
		CloseHandle(fileHandle);
	}

	return result;
}

DWORD GetSymlinkTarget(LPCTSTR Path, LPTSTR TargetPath, size_t TargetSize)
{
	DWORD result = (DWORD)E_FAIL;
	HANDLE fileHandle;

	// We need an existing file handle in order to call DeviceIoControl
	fileHandle = CreateFile(Path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		// Allocate enough space to fit the maximum sized reparse data buffer
		char Tmp[MAXIMUM_REPARSE_DATA_BUFFER_SIZE] = {0};
		// Redefine the above char array into a REPARSE_DATA_BUFFER we can work with
		REPARSE_DATA_BUFFER& reparseData = *(REPARSE_DATA_BUFFER*)Tmp;

		// Call DeviceIoControl to get the reparse point data
		DWORD bytesRet;
		if (DeviceIoControl(fileHandle, FSCTL_GET_REPARSE_POINT, NULL, 0, &reparseData, sizeof(Tmp), &bytesRet, NULL))
		{
			// Verify quickly that the reparse tag is what we expect
			if (reparseData.ReparseTag == IO_REPARSE_TAG_SYMLINK)
			{
				// WTF For some reason Windows doesn't fill in the reparseData buffer correctly and so the PrintName
				// and SubstituteName get smashed together without the proper null character in between. In some cases
				// the length and offset is even messed up and points to the end of the path buffer.
				size_t CopyLength = reparseData.SymbolicLinkReparseBuffer.SubstituteNameLength;
				// Find the first null character
				size_t NullCharIdx = 0;
				while (reparseData.SymbolicLinkReparseBuffer.PathBuffer[NullCharIdx] != 0)
				{
					NullCharIdx++;
				}
				// Is the null character at the end of both the PrintName and SubstituteName?
				if (NullCharIdx > 0 && NullCharIdx >= reparseData.SymbolicLinkReparseBuffer.PrintNameOffset && NullCharIdx >= reparseData.SymbolicLinkReparseBuffer.SubstituteNameOffset)
				{
					// There are three ways to find the start of the second string in PathBuffer. First, the target is
					// a full drive path in which case we can look for the ':' character. Second, is a full path that
					// begins with \\??\\. Third, there are no identifying characters so we just split the buffer in
					// half.
					int FoundIdx = StrFind(reparseData.SymbolicLinkReparseBuffer.PathBuffer, TEXT("\\??\\"), 4);
					if (FoundIdx >= 4)
					{
						CopyLength = FoundIdx;
					}
					else
					{
						FoundIdx = StrFind(reparseData.SymbolicLinkReparseBuffer.PathBuffer, TEXT(":"), 2);
						if (FoundIdx >= 2)
						{
							CopyLength = FoundIdx;
						}
						else
						{
							CopyLength = NullCharIdx / 2;
						}
					}
				}

				// Copy the target out of the struct into a temporary string
				WCHAR Tmp[MAX_PATH] = {0};
				if (SUCCEEDED(StringCchCopyN(Tmp, MAX_PATH, &reparseData.SymbolicLinkReparseBuffer.PathBuffer[reparseData.SymbolicLinkReparseBuffer.PrintNameOffset/sizeof(WCHAR)], CopyLength)))
				{
					// Copy the temp string to the provided Target buffer
					result = WCHARtoTCHAR(Tmp, min(CopyLength, MAX_PATH), TargetPath, TargetSize) == S_OK ? S_OK : GetLastError();
				}
				else
				{
					result = GetLastError();
				}
			}
		}
		else
		{
			result = GetLastError();
		}

		// Close the handle
		CloseHandle(fileHandle);
	}
	else
	{
		result = GetLastError();
	}

	return result;
}

DWORD CreateSymlink(LPCTSTR Link, LPCTSTR Target)
{
	DWORD result = (DWORD)E_FAIL;

	// Grab the file attributes of Target. This allows us to determine the target exists but also if it is a file or
	// directory.
	DWORD fileAttributes = GetFileAttributes(Target);
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		// If the target does't exist it could be a relative path from the link instead of the current working
		// directory. In this case, we'll append Target to the base Link path and see if that exists.
		TCHAR AdjTarget[MAX_PATH];
		StringCchCopyN(AdjTarget, MAX_PATH, Link, StrFind(Link, TEXT("\\"), -1, -1)+1);
		StringCchCat(AdjTarget, MAX_PATH, Target);
		fileAttributes = GetFileAttributes(AdjTarget);
		if (fileAttributes == INVALID_FILE_ATTRIBUTES)
		{
			// Okay so that didn't work either. This means that we likely have an unknown path type Windows can't
			// figure out. For this we'll do a dirty trick and assume that a path whose basename does not
			// include an extension is a directory. This should account for the vast majority of circumstances.
			if (StrFind(Target, TEXT("."), -1, -1) < StrFind(Target, TEXT("\\"), -1, -1))
			{
				fileAttributes = FILE_ATTRIBUTE_DIRECTORY;
			}
		}
	}

	// Determine if Target is a file or directory. If no attributes could be found proceed anyway
	DWORD dwFlags = 0;
	if (fileAttributes != INVALID_FILE_ATTRIBUTES && fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		dwFlags = SYMBOLIC_LINK_FLAG_DIRECTORY;
	}

	// Create the symlink
	if (CreateSymbolicLink(Link, Target, dwFlags) != 0)
	{
		result = S_OK;
	}
	else
	{
		result = GetLastError();
	}

	return result;
}

DWORD DeleteSymlink(LPCTSTR Path)
{
	DWORD result = (DWORD)E_FAIL;

	// Grab the file attributes of Target
	DWORD fileAttributes = GetFileAttributes(Path);
	if (fileAttributes != INVALID_FILE_ATTRIBUTES)
	{
		// Is the path a file or a directory?
		BOOL bOpResult = false;
		if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			bOpResult = RemoveDirectory(Path);
		}
		else
		{
			bOpResult = DeleteFile(Path);
		}

		result = bOpResult ? S_OK : GetLastError();
	}
	else
	{
		result = GetLastError();
	}

	return result;
}

} // namespace libntfslinks
