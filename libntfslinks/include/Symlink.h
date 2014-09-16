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