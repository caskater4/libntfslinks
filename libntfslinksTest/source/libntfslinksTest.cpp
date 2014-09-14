// libjunctionTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	// Forward declare the junction test function
	int JunctionTest();
	// Execute the junction test
	int result = JunctionTest();
	if (result != 0) { return result; }

	// Forward declare the symlink test function
	int SymlinkTest();
	// Execute the symlink test
	result = SymlinkTest();
	if (result != 0) { return result; }

	return 0;
}

