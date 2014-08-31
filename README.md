libntfslinks
===========

A C++ library for working with NTFS junctions and symbolic links.

#How to Build

The solution files for this project were created for Visual Studio 2012. Any
version after 2012 should work but has not been tested.

1. Open libntfslinks.sln in Visual Studio.

2. Select the desired platform and configuration (e.g. Release|x64)

3. Build the solution (Build->Build Solution)

Once successfully built a .lib file will be generated in the lib folder in the
root of the solution. Copy or link this file to your project and copy or link
to the header files in the libntfslinks\include folder.

#How to Use

There are two header files of importance when using this library, Junction.h
and Symlink.h.

Use the Junction.h header when you want to work with NTFS junctions and use
Symlink.h when you want to work with NTFS symbolic links. Per Microsoft's
documentation symbolic links are generally preferred over junctions however
when using them your application must be run with elevated privileges.

The following is an example of working with junctions. The example
creates a new junction, checks that it is in fact a junction, retrieves
the target and finally deletes it.
```cpp
#include <Junction.h>
#include <Windows.h>

using namespace libntfslinks;

// Create the junction
CreateJunction(TEXT("JunctionName"), TEXT("TargetDir"));

// Check that it is exists
if (IsJunction(TEXT("JunctionName"))
{
	printf("The junction exists!");
}

// Get the target
TCHAR Target[MAX_PATH];
GetJunctionTarget(TEXT("JunctionName"), Target, MAX_PATH);

// Delete the junction
DeleteJunction(TEXT("JunctionName"));
```
Working with symbolic links is just as easy.
```cpp
#include <Symlink.h>
#include <Windows.h>

using namespace libntfslinks;

// Create the symlink to a file
CreateSymlink(TEXT("FileLinkName"), TEXT("TargetFile"));

// Create a symlink to a directory
CreateSymlink(TEXT("DirLinkName"), TEXT("TargetDir"));

// Get the target of the file
TCHAR FileTarget[MAX_PATH];
GetSymlinkTarget(TEXT("FileLinkName"), FileTarget, MAX_PATH);

// Get the target of the dir
TCHAR DirTarget[MAX_PATH];
GetSymlinkTarget(TEXT("DirLinkName"), DirTarget, MAX_PATH);

// Delete both symlinks
DeleteSymlink(TEXT("FileLinkName"));
DeleteSymlink(TEXT("DirLinkName"));
```