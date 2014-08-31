#ifndef JUNCTION_H
#define JUNCTION_H
#pragma once

#include <Windows.h>

namespace libntfslinks
{

/**
 * Determines if the specified path is a valid NTFS junction (reparse point).
 *
 * @param Path The path to verify is a junction.
 * @return Returns true if the specified path is a valid NTFS junction, otherwise false.
 */
bool IsJunction(LPCTSTR Path);

/**
 * Retrieves the target path for the specified NTFS junction.
 *
 * @param Path The path of the NTFS junction to retrieve data for.
 * @param TargetPath The buffer to write the junction's target path to. [OUT]
 * @param TargetSize The size of the TargetPath buffer.
 * @return Returns zero if the operation was successful, otherwise a non-zero value if an error occurred.
 */
DWORD GetJunctionTarget(LPCTSTR Path, LPTSTR TargetPath, size_t TargetSize);

/**
 * Creates a new NTFS junction at the specified link path which points to the given target path.
 *
 * @param Link The path of the NTFS junction to create that will link to Target.
 * @param Target The destination path that the new junction will point to.
 * @return Returns zero if the operation was successful, otherwise a non-zero value if an error occurred.
 */
DWORD CreateJunction(LPCTSTR Link, LPCTSTR Target);

/**
 * Deletes an NTFS junction at the specified path location.
 *
 * @param Path The path of the NTFS junction to delete.
 * @return Returns zero if the operation was successful, otherwise a non-zero value if an error occurred.
 */
DWORD DeleteJunction(LPCTSTR Path);

} // namespace libntfslinks

#endif //JUNCTION_H