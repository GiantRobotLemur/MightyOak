//! @file Ag/Core/Memory.hpp
//! @brief The declaration of utility functions related to memory management.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2023-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_CORE_MEMORY_HPP__
#define __AG_CORE_MEMORY_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <memory>
#include <new>

#ifndef __cpp_lib_hardware_interference_size
    // Make up for the lack of definitions provided with gcc.
namespace std {
    // 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
    constexpr std::size_t hardware_constructive_interference_size = 64;
    constexpr std::size_t hardware_destructive_interference_size = 64;
}
#endif

namespace Ag {

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
void *mallocAligned(size_t alignment, size_t sizeInBytes);
void *mallocAlignedPow2(uint8_t alignmentPow2, size_t sizeInBytes);
void freeAligned(void *alignedBlock);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////
//! @brief A type suitable for use as the deleter in an std:: unique_ptr<> type
//! declaration where the object is constructed in a region of memory allocated
//! using mallocAligned*().
//! @tparam T The data type of the object being deleted.
template<typename T> struct AlignedDeleter
{
    void operator()(T *obj) const
    {
        if (obj != nullptr)
        {
            // Manually call the destructor.
            obj->~T();

            // Free the block of memory.
            freeAligned(obj);
        }
    }
};

//! @brief A template structure used to dynamically allocate objects on fixed
//! address boundaries.
//! @tparam T The data type of the object being instantiated.
//! @tparam TAlignment The address boundary at which the object must be stored,
//! must be a power of 2.
template<typename T, size_t TAlignment = alignof(T)>
struct AlignmentTraits
{
    static constexpr size_t Alignment = TAlignment;
    using TargetType = T;
    using Deleter = AlignedDeleter<T>;
    using UPtr = std::unique_ptr<T, Deleter>;

    //! @brief Dynamically allocates an object on an aligned address boundary.
    //! @tparam TArgs The types of the construction arguments of the object
    //! to construct.
    //! @param[in] args The argument values to pass to the object constructor.
    //! @return A newly allocated object wrapped in a unique_ptr<> so that it is
    //! properly disposed of when going out of scope.
    template<typename... TArgs>
    static UPtr create(TArgs&&... args)
    {
        void *block = mallocAligned(TAlignment, sizeof(TargetType));

        return UPtr(new(block) TargetType(std::forward<TArgs>(args)...));
    }
};

//! @brief Dynamically allocates an object on an aligned address boundary.
//! @tparam T The data type of the object to allocate and construct.
//! @tparam TAlignment The address boundary at which the object must be stored,
//! must be a power of 2.
//! @tparam TArgs The types of the construction arguments of the object to construct.
//! @param[in] args The argument values to pass to the object constructor.
//! @return A newly allocated object which needs to be manually destroyed and
//! de-allocated using freeAligned(), or via AlignedDeleter<T>.
template<typename T, size_t TAlignment = alignof(T), typename... TArgs>
T *createAligned(TArgs... args)
{
    void *block = mallocAligned(TAlignment, sizeof(T));

    return new(block) T(std::forward(args)...);
}

//! @brief Dynamically allocates an object on an aligned address boundary.
//! @tparam T The data type of the object to allocate and construct.
//! @tparam TAlignment The address boundary at which the object must be stored,
//! must be a power of 2.
//! @tparam TArgs The types of the construction arguments of the object to construct.
//! @param[in] args The argument values to pass to the object constructor.
//! @return A newly allocated object wrapped in a unique_ptr<> so that it is
//! properly disposed of when going out of scope.
template<typename T, size_t TAlignment = alignof(T), typename... TArgs>
std::unique_ptr<T, AlignedDeleter<T>> createUniqueAligned(TArgs&&... args)
{
    void *block = mallocAligned(TAlignment, sizeof(T));

    return std::unique_ptr<T, AlignedDeleter<T>>(new(block) T(std::forward<TArgs>(args)...));
}

} // namespace Ag

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
