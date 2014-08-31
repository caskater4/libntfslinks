#include "Symlink.h"

#include <strsafe.h>
#include <winioctl.h>

#include "CharUtils.h"
#include "ntfstypes.h"

namespace libntfslinks
{

bool IsSymlink(LPCTSTR Path)
{
	bool result = false;
	HANDLE fileHandle;

	// We need an existing file handle in order to call DeviceIoControl
	fileHandle = CreateFile(Path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
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
	fileHandle = CreateFile(Path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
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
				// Copy the target out of the struct into a temporary string
				WCHAR Tmp[MAX_PATH] = {0};
				if (StringCchCopyW(Tmp, min(reparseData.SymbolicLinkReparseBuffer.PrintNameLength+1, MAX_PATH), &reparseData.SymbolicLinkReparseBuffer.PathBuffer[reparseData.SymbolicLinkReparseBuffer.PrintNameOffset/sizeof(WCHAR)]) == S_OK)
				{
					// Copy the temp string to the provided Target buffer
					result = WCHARtoTCHAR(Tmp, min(reparseData.SymbolicLinkReparseBuffer.PrintNameLength, MAX_PATH), TargetPath, TargetSize) == S_OK ? S_OK : GetLastError();
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

	// Grab the file attributes of Target
	DWORD fileAttributes = GetFileAttributes(Target);
	if (fileAttributes != INVALID_FILE_ATTRIBUTES)
	{
		// Determine if Target is a file or directory
		DWORD dwFlags = 0;
		if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			dwFlags = SYMBOLIC_LINK_FLAG_DIRECTORY;
		}

		// Create the symlink
		if (CreateSymbolicLink(Link, Target, dwFlags) == 0)
		{
			result = S_OK;
		}
		else
		{
			result = GetLastError();
		}
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
