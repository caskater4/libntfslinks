#include "stdafx.h"

#include <strsafe.h>
#include <Windows.h>

#include "Symlink.h"

/**
 * Removes any files left over from previous tests.
 */
void ClearSymlinkTestFiles()
{
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"), TEXT(" /C \"rmdir TestDirSymlink & rmdir TestDirSymlink2 & rmdir /s /q TestDir\""), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int SymlinkTest()
{
	using namespace libntfslinks;

	ClearSymlinkTestFiles();

	// Create a temporary directory
	CreateDirectory(TEXT("TestDir"), NULL);
	HANDLE booHandle = CreateFile(TEXT("TestDir\\boo.txt"), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, NULL, NULL);
	CloseHandle(booHandle);

	if (CreateSymbolicLink(TEXT("TestDirSymlink"), TEXT("TestDir"), SYMBOLIC_LINK_FLAG_DIRECTORY) == 0)
	{
		printf("Failed to create symlink to TestDir.");
		return GetLastError();
	}

	// Test the IsSymlink() function
	if (IsSymlink(TEXT("TestDir")) == true)
	{
		printf("Failed IsSymlink(\"TestDir\") test.");
		return 1;
	}
	if (IsSymlink(TEXT("TestDirSymlink")) == false)
	{
		printf("Failed IsSymlink(\"TestDirSymlink\") test.");
		return 1;
	}

	TCHAR TargetPath[MAX_PATH] = {0};
	DWORD result = GetSymlinkTarget(TEXT("TestDirSymlink"), TargetPath, MAX_PATH);
	if (result != S_OK)
	{
		printf("Failed GetSymlinkTarget(\"TestDirSymlink\")");
		return result;
	}
	
	// Test the CreateSymlink function
	result = CreateSymlink(TEXT("TestDirSymlink2"), TEXT("TestDir"));
	if (result != S_OK)
	{
		printf("Failed CreateSymlink(\"TestDirSymlink2\", \"TestDir\")");
		return result;
	}
	if (IsSymlink(TEXT("TestDirSymlink2")) == false)
	{
		printf("Failed IsSymlink(\"TestDirSymlink2\") test.");
		return 1;
	}

	// Make sure our link has the same target as the one created with CreateSymbolicLink.
	TCHAR TargetPath2[MAX_PATH] = {0};
	result = GetSymlinkTarget(TEXT("TestDirSymlink2"), TargetPath2, MAX_PATH);
	if (result != S_OK)
	{
		printf("Failed to GetSymlinkTarget(\"TestDirSymlink2\")");
		return result;
	}
	if (_tcscmp(TargetPath, TargetPath2) != 0)
	{
		printf("GetSymlinkTarget(\"TestDirSymlink2\") returned an unexpected result.");
		return E_FAIL;
	}

	// Test the DeleteSymlink function
	result = DeleteSymlink(TEXT("TestDirSymlink2"));
	if (result != S_OK)
	{
		printf("Failed DeleteSymlink(\"TestDirSymlink2\")");
		return result;
	}
	if (IsSymlink(TEXT("TestDirSymlink2")) == true)
	{
		printf("Failed to delete symbolic link TestDirSymlink2.");
		return 1;
	}

	// Clean up the original symlink
	result = DeleteSymlink(TEXT("TestDirSymlink"));
	if (result != S_OK)
	{
		printf("Failed to delete symlink TestDirSymlink.");
		return result;
	}
	if (IsSymlink(TEXT("TestDirSymlink")) == true)
	{
		printf("Failed to delete symlink TestDirSymlink.");
		return 1;
	}

	// Clean up the temp directory
	if (!DeleteFile(TEXT("TestDir\\boo.txt")))
	{
		printf("Failed to delete TestDir\\boo.txt.");
		return GetLastError();
	}
	if (!RemoveDirectory(TEXT("TestDir")))
	{
		printf("Failed to delete TestDir.");
		return GetLastError();
	}

	return 0;
}