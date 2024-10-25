//! @file ArmDebugger/Tools.hpp
//! @brief The declaration of various non-domain-specific utility functions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_TOOLS_HPP__
#define __ARM_DEBUGGER_TOOLS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QJsonObject>

#include "Ag/Core/EnumInfo.hpp"
#include "Ag/Core/Exception.hpp"

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
class QWidget;

namespace Mo {
////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A structure describing a run of addresses in an emulated system.
struct MemoryRegion
{
    uint32_t BaseAddress;
    uint32_t Length;
    bool IsLogical;

    // Construction/Destruction
    MemoryRegion();
    MemoryRegion(uint32_t baseAddr, uint32_t length, bool isLogical);

    // Accessors
    bool containsAddress(uint32_t address) const;
    uint32_t getEndAddress() const;

    // Operations
    bool operator<(const MemoryRegion &rhs) const;
    bool operator==(const MemoryRegion &rhs) const;
    bool operator!=(const MemoryRegion &rhs) const;
};

Q_DECLARE_METATYPE(MemoryRegion);

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
void reportError(QWidget *parent, const QString &title,
                 const Ag::Exception &error);

void setJsonValue(QJsonObject &parent, Ag::utf8_cptr_t key, uint32_t value);

template<typename TEnum, typename TEnumSymbol = Ag::EnumSymbol<TEnum>>
void setJsonValue(QJsonObject &parent, Ag::utf8_cptr_t key,
                  const Ag::EnumInfo<TEnum, TEnumSymbol> &enumInfo,
                  TEnum value)
{
    std::string_view utf8Text = enumInfo.toString(value);

    parent.insert(QString::fromUtf8(key),
                  QString::fromUtf8(utf8Text.data(), utf8Text.length()));
}

bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, QJsonValue &value);
bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, QJsonObject &value);
bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, QJsonArray &value);
bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, uint32_t &value);
bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, QString &value);

template<typename TEnum, typename TEnumSymbol = Ag::EnumSymbol<TEnum>>
bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key,
                     const Ag::EnumInfo<TEnum, TEnumSymbol> &enumInfo,
                     TEnum &value)
{
    QString textValue;
    bool hasValue = false;

    if (tryGetJsonValue(parent, key, textValue))
    {
        QByteArray utf8Data = textValue.toUtf8();

        hasValue = enumInfo.tryParse(std::string_view(utf8Data.constData(),
                                                      utf8Data.length()),
                                     value);
    }

    return hasValue;
}

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
