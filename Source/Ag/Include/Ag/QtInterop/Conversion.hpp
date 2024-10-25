//! @file Ag/QtInterop/Conversion.hpp
//! @brief The declaration of various functions to convert between Ag and Qt
//! data types.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __AG_QT_CONVERSION_HPP__
#define __AG_QT_CONVERSION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QString>

#include "Ag/Core/String.hpp"
#include "Ag/Core/FsPath.hpp"

namespace Ag {
namespace Qt {

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
QString toQString(Ag::string_cref_t agString);
void appendUtf8ToQString(QString &target, const std::string_view &utf8Text);
void appendUtf8ToQString(QString &target, Ag::string_cref_t utf8Text);
QString toQString(const Ag::Fs::Path &path);
String fromQString(const QStringView &qString);
Ag::Fs::Path pathFromQString(const QStringView &qString);

}} // namespace Ag::Qt

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
