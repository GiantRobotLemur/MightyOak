
//! @brief Contains all elements of the Silver (Ag) library suite.
namespace Ag {

//! @brief Defines a number of operations on primitive binary values.
namespace Bin {

} // namespace Bin

//! @defgroup AgFundamentals Core Fundamentals
//! @brief The Ag Core library defines a number of fundamental constants,
//! macros and data types which are in the global namespace for the sake
//! of convenience.
//! @{

/*! @brief A macro defined if the wchar_t data type is a 32-bit data type.
@details
The Ag libraries assume that if this macro is defined, wchar_t values
represent whole Unicode code points. If it is not defined, they assume that
a wchar_t value is a UTF-16 word, either a single character or part of a
surrogate pair.

The Ag/Configuration.hpp header file determines whether the macro should be
defined.
*/ 
#define WCHAR_IS_32BIT

/*! @brief Defines a compiler independent attribute used to align structures
and classes on specific memory bounds.
@param x The whole number of bits to align the object on. This value must be
a whole power of 2, e.g. 2, 4, 8, 16, 32, etc.
@details
@par Example
@code{.cpp}
class STRUCT_ALIGN(16) MyClass ...
@endcode
*/
#define STRUCT_ALIGN(x)
//! @}


} // namespace Ag