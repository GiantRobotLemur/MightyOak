//! @file Core/Memory.cpp
//! @brief The definition of utility functions related to memory management.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>

#include "Ag/Core/Exception.hpp"
#include "Ag/Core/Memory.hpp"
#include "Ag/Core/Variant.hpp"
#include "Ag/Core/VariantTypes.hpp"

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Allocates a block of memory with a specific address alignment.
//! @param[in] alignment The address boundary the block should be aligned on,
//! must be a power of 2.
//! @paarm[in] sizeInBytes The minimum count of bytes in the allocated block.
//! @return A pointer to the newly allocated block of memory, to be freed with
//! a call to freeAligned().
void *mallocAligned(size_t alignment, size_t sizeInBytes)
{
#ifdef _MSC_VER
#define MALLOC_FN "_aligned_malloc()"
    void *block = _aligned_malloc(sizeInBytes, alignment);
#else
#define MALLOC_FN "aligned_malloc()"
    void *block = aligned_alloc(alignment, sizeInBytes);
#endif

    if (block == nullptr)
    {
        if (errno == 0)
        {
            String detail = String::format("Failed to allocate a block of {0} "
                                           "bytes on an {1} byte address boundary.",
                                           { sizeInBytes, alignment });
            throw OperationException(detail);
        }
        else
        {
            throw RuntimeLibraryException(MALLOC_FN, errno);
        }
    }

#undef MALLOC_FN

    return block;
}

//! @brief Allocates a block of memory with a specific address alignment.
//! @param[in] alignmentPow2 The address boundary the block should be aligned on,
//! expressed as a power of 2.
//! @paarm[in] sizeInBytes The minimum count of bytes in the allocated block.
//! @return A pointer to the newly allocated block of memory, to be freed with
//! a call to freeAligned().
void *mallocAlignedPow2(uint8_t alignmentPow2, size_t sizeInBytes)
{
    return mallocAligned(static_cast<size_t>(1) << alignmentPow2,
                         sizeInBytes);
}

//! @brief Frees a block of memory previously allocated with mallocAligned*().
//! @param[in] alignedBlock A pointer to the start of the memory block to free,
//! if nullptr, no action is performed.
void freeAligned(void *alignedBlock)
{
    if (alignedBlock != nullptr)
    {
#ifdef _MSC_VER
        _aligned_free(alignedBlock);
#else
        // In non-MSVC runtime libraries, the conventional free function
        // is good enough.
        free(alignedBlock);
#endif
    }
}

} // namespace Ag
////////////////////////////////////////////////////////////////////////////////

