#ifndef SYMBOLICLINK_H
#define SYMBOLICLINK_H
#pragma once

#include <Windows.h>

namespace libntfslinks
{

/**
 * Determines if the specified path is a valid NTFS symbolic link (reparse point).
 *
 * @param Path The path to verify is a symbolic link.
 * @return Returns true if the specified path is a valid NTFS symbolic link, otherwise false.
 */
bool IsSymlink(LPCTSTR Path);

/**
 * Retrieves the target path for the specified NTFS symbolic link.
 *
 * @param Path The path of the NTFS symbolic link to retrieve data for.
 * @param TargetPath The buffer to write the symbolic link's target path to. [OUT]
 * @param TargetSize The size of the TargetPath buffer.
 * @return Returns zero if the operation was successful, otherwise a non-zero value if an error occurred.
 */
DWORD GetSymlinkTarget(LPCTSTR Path, LPTSTR TargetPath, size_t TargetSize);

/**
 * Creates a new NTFS symbolic link at the specified link path which points to the given target path.
 *
 * @param Link The path of the NTFS symbolic link to create that will link to Target.
 * @param Target The destination path that the new symbolic link will point to.
 * @return Returns zero if the operation was successful, otherwise a non-zero value if an error occurred.
 */
DWORD CreateSymlink(LPCTSTR Link, LPCTSTR Target);

/**
 * Deletes an NTFS symbolic link at the specified path location.
 *
 * @param Path The path of the NTFS symbolic link to delete.
 * @return Returns zero if the operation was successful, otherwise a non-zero value if an error occurred.
 */
DWORD DeleteSymlink(LPCTSTR Path);

} // namespace libntfslinks

#endif //SYMBOLICLINK_H