/*******************************************************************************
 *  Copyright IBM Corp. and others 2018
 *
 *  This program and the accompanying materials are made available under
 *  the terms of the Eclipse Public License 2.0 which accompanies this
 *  distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 *  or the Apache License, Version 2.0 which accompanies this distribution and
 *  is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 *  This Source Code may also be made available under the following
 *  Secondary Licenses when the conditions for such availability set
 *  forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 *  General Public License, version 2 with the GNU Classpath
 *  Exception [1] and GNU General Public License, version 2 with the
 *  OpenJDK Assembly Exception [2].
 *
 *  [1] https://www.gnu.org/software/classpath/license.html
 *  [2] https://openjdk.org/legal/assembly-exception.html
 *
 *  SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#if !defined(OMR_BYTES_HPP_)
#define OMR_BYTES_HPP_

#include <omrcfg.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

namespace OMR
{

inline size_t
bytes(size_t n)
{
	return n;
}

inline size_t
kibibytes(size_t n)
{
	return n * bytes(1024);
}

inline size_t
mebibytes(size_t n)
{
	return n * kibibytes(1024);
}

inline size_t
gibibytes(size_t n)
{
	return n * mebibytes(1024);
}

/// True if x is a power of two.
inline bool
isPow2(size_t x)
{
	return x && ((x & (x - 1)) == 0);
}

/// The maximum safe alignment, when aligning sizes up to UNALIGNED_SIZE_MAX.
static const size_t ALIGNMENT_MAX = (SIZE_MAX >> 1) + 1;

/// The maximum safe size, when aligning up to ALIGNMENT_MAX.
static const size_t UNALIGNED_SIZE_MAX = (SIZE_MAX >> 1) + 1;

/// True if size is aligned to alignment. No safety checks.
/// alignment must be a power of two.
inline bool
alignedNoCheck(size_t size, size_t alignment)
{
	return (size & (alignment - 1)) == 0;
}

/// True if size is aligned to alignment.
/// alignment must be a power of two.
inline bool
aligned(size_t size, size_t alignment)
{
	assert(isPow2(alignment));
	return alignedNoCheck(size, alignment);
}

/// Round a size up to a multiple of alignment. No safety checks.
/// alignment must be a power of two.
inline size_t
alignNoCheck(size_t size, size_t alignment)
{
	return (size + (alignment - 1)) & ~(alignment - 1);
}

/// Round a size up to a multiple of alignment.
/// alignment must be a power of two.
///
/// The maximum unaligned size is intrinsically related to the alignment.
/// As a conservative measure, users should not align sizes greater than
/// UNALIGNED_SIZE_MAX.
inline size_t
align(size_t size, size_t alignment)
{
	assert(isPow2(alignment));
	assert(size <= SIZE_MAX - alignment + 1); // overflow check
	return alignNoCheck(size, alignment);
}

/// Returns an aligned pointer.
///
/// This function has the same semantics as C++11's std::align() and
/// exists to support old compilers that don't provide it.
inline void*
align(size_t alignment, size_t size, void* &ptr, size_t &space)
{
	if (size > space) {
		return NULL;
	}
	uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
	uintptr_t alignedP = (p + (alignment - 1)) & ~(alignment - 1);
	uintptr_t adjustment = alignedP - p;
	if (adjustment > (space - size)) {
		return NULL;
	}
	space -= adjustment;
	void *alignedPtr = reinterpret_cast<void*>(alignedP);
	ptr = alignedPtr;
	return alignedPtr;
}

} // namespace OMR

#endif // OMR_BYTES_HPP_
