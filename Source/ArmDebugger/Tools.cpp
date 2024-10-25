//! @file ArmDebugger/Tools.cpp
//! @brief The definition of various non-domain-specific utility functions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <QJsonArray>
#include <QMessageBox>

#include "Ag/QtInterop/Conversion.hpp"

#include "Tools.hpp"
#include "DebuggerApp.hpp"

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// MemoryRegion Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
MemoryRegion::MemoryRegion() :
    BaseAddress(0),
    Length(0),
    IsLogical(false)
{
}

MemoryRegion::MemoryRegion(uint32_t baseAddr, uint32_t length, bool isLogical) :
    BaseAddress(baseAddr),
    Length(length),
    IsLogical(isLogical)
{
}

bool MemoryRegion::containsAddress(uint32_t address) const
{
    return (address >= BaseAddress) && ((address - BaseAddress) < Length);
}

uint32_t MemoryRegion::getEndAddress() const
{
    return BaseAddress + Length;
}

bool MemoryRegion::operator<(const MemoryRegion &rhs) const
{
    return BaseAddress < rhs.BaseAddress;
}

bool MemoryRegion::operator==(const MemoryRegion &rhs) const
{
    return (BaseAddress == rhs.BaseAddress) &&
        (Length == rhs.Length) &&
        (IsLogical == rhs.IsLogical);
}

bool MemoryRegion::operator!=(const MemoryRegion &rhs) const
{
    return (BaseAddress != rhs.BaseAddress) ||
        (Length != rhs.Length) ||
        (IsLogical != rhs.IsLogical);
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Reports an exception to the user.
//! @param[in] parent The widget which will be the owner of any UI shown.
//! @param[in] title The title of any UI shown.
//! @param[in] error The exception to report.
void reportError(QWidget *parent, const QString &title,
                 const Ag::Exception &error)
{
    if (parent == nullptr)
    {
        if (auto *app = qobject_cast<DebuggerApp *>(QCoreApplication::instance()))
        {
            parent = app->getMainWindow();
        }
    }

    QString message;
    Ag::Qt::appendUtf8ToQString(message, error.getMessage());
    message.append('\n');
    message.append('\n');
    Ag::Qt::appendUtf8ToQString(message, error.getDetail());

    QMessageBox::critical(parent, title, message, QMessageBox::Ok);

    qDebug() << title << ": " << message;
}

void setJsonValue(QJsonObject &parent, Ag::utf8_cptr_t key, uint32_t value)
{
    parent.insert(QString::fromUtf8(key), static_cast<qint64>(value));
}

bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key,
                     QJsonValue &value)
{
    auto pos = parent.constFind(QString::fromUtf8(key));

    if (pos == parent.constEnd())
    {
        value = QJsonValue::Undefined;
        return false;
    }
    else
    {
        value = *pos;
        return true;
    }
}

bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, QJsonObject &value)
{
    QJsonValue field;
    bool hasValue = false;

    if (tryGetJsonValue(parent, key, field) && field.isObject())
    {
        value = field.toObject();
        hasValue = true;
    }

    return hasValue;
}

bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, QJsonArray &value)
{
    QJsonValue field;
    bool hasValue = false;

    if (tryGetJsonValue(parent, key, field) && field.isArray())
    {
        value = field.toArray();
        hasValue = true;
    }

    return hasValue;
}

bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, uint32_t &value)
{
    QJsonValue jsonValue;
    bool hasValue = false;
    value = 0;

    if (tryGetJsonValue(parent, key, jsonValue))
    {
        hasValue = true;
        double realValue;

        switch (jsonValue.type())
        {
        case QJsonValue::Bool:
            value = jsonValue.toBool() ? 1 : 0;
            break;

        case QJsonValue::Double:
            realValue = jsonValue.toDouble();

            if ((realValue >= 0.0) && (realValue <= UINT_MAX))
            {
                hasValue = true;
                value = static_cast<uint32_t>(std::rint(realValue));
            }
            break;

        case QJsonValue::String:
            value = jsonValue.toString().toUInt(&hasValue, 0);
            break;

        case QJsonValue::Null:
        case QJsonValue::Array:
        case QJsonValue::Object:
        default:
            hasValue = false;
            break;
        }
    }

    return hasValue;
}

bool tryGetJsonValue(const QJsonObject &parent, Ag::utf8_cptr_t key, QString &value)
{
    QJsonValue jsonValue;
    bool hasValue = false;

    if (tryGetJsonValue(parent, key, jsonValue) &&
        jsonValue.isString())
    {
        value = jsonValue.toString();
        hasValue = true;
    }
    else
    {
        value.clear();
    }

    return hasValue;
}


} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

