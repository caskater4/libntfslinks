#include "stdafx.h"

#include <strsafe.h>
#include <Windows.h>

#include "Junction.h"

int JunctionTest()
{
	using namespace libntfslinks;

	// Create a temporary directory
	CreateDirectory(TEXT("TestDir"), NULL);
	HANDLE booHandle = CreateFile(TEXT("TestDir\\boo.txt"), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, NULL, NULL);
	CloseHandle(booHandle);

	// Use the 'mklink' tool to create a junction to it
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"), TEXT(" /C \"mklink /J TestDirJunc TestDir\""), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		printf("Failed to create junction with mklink.exe.\n", GetLastError());
		return GetLastError();
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// Also create a symbolic link to TestDir
	if (CreateSymbolicLink(TEXT("TestDirSymLink"), TEXT("TestDir"), SYMBOLIC_LINK_FLAG_DIRECTORY) == 0)
	{
		printf("Failed to create symlink to TestDir.");
		return GetLastError();
	}
	
	// Test the IsJunction function
	if (IsJunction(TEXT("TestDir")) == true)
	{
		printf("Failed IsJunction(\"TestDir\") test.");
		return 1;
	}
	if (IsJunction(TEXT("TestDirJunc")) == false)
	{
		printf("Failed IsJunction(\"TestDirJunc\") test.");
		return 1;
	}
	if (IsJunction(TEXT("TestDirSymLink")) == true)
	{
		printf("Failed IsJunction(\"TestDirSymLink\") test.");
		return 1;
	}
	
	// Test the GetJunctionTarget function
	TCHAR TargetPath[MAX_PATH] = {0};
	DWORD result = GetJunctionTarget(TEXT("TestDirJunc"), TargetPath, MAX_PATH);
	if (result != S_OK)
	{
		printf("Failed GetJunctionTarget(\"TestDirJunc\")");
		return result;
	}
	
	// Test the CreateJunction function
	result = CreateJunction(TEXT("TestDirJunc2"), TEXT("TestDir"));
	if (result != S_OK)
	{
		printf("Failed CreateJunction(\"TestDirJunc2\", \"TestDir\")");
		return result;
	}
	if (IsJunction(TEXT("TestDirJunc2")) == false)
	{
		printf("Failed IsJunction(\"TestDirJunc2\") test.");
		return 1;
	}

	// Make sure our junction has the same target as the one created with mklink
	TCHAR TargetPath2[MAX_PATH] = {0};
	result = GetJunctionTarget(TEXT("TestDirJunc2"), TargetPath2, MAX_PATH);
	if (result != S_OK)
	{
		printf("Failed to GetJunctionTarget(\"TestDirJunc2\")");
		return result;
	}
	if (_tcscmp(TargetPath, TargetPath2) != 0)
	{
		printf("GetJunctionTarget(\"TestDirJunc2\") returned an unexpected result.");
		return E_FAIL;
	}

	// Check to see if the junction actually works. We can verify this by checking that boo.txt exists.
	booHandle = CreateFile(TEXT("TestDirJunc2\\boo.txt"), GENERIC_READ, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (booHandle == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	CloseHandle(booHandle);

	// Test the DeleteJunction function
	result = DeleteJunction(TEXT("TestDirJunc2"));
	if (result != S_OK)
	{
		printf("Failed DeleteJunction(\"TestDirJunc2\")");
		return result;
	}
	if (IsJunction(TEXT("TestDirJunc2")) == true)
	{
		printf("Failed to delete junction TestDirJunc2.");
		return 1;
	}

	// Clean up the symlink
	if (!RemoveDirectory(TEXT("TestDirSymLink")))
	{
		printf("Failed to delete symlink TestDirSymLink.");
	}

	// Clean up the original junction
	result = DeleteJunction(TEXT("TestDirJunc"));
	if (result != S_OK)
	{
		printf("Failed to delete junction TestDirJunc.");
		return result;
	}
	if (IsJunction(TEXT("TestDirJunc")) == true)
	{
		printf("Failed to delete junction TestDirJunc.");
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