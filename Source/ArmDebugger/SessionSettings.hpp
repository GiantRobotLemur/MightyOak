//! @file ArmDebugger/SessionSettings.hpp
//! @brief The declaration of an object which encodes the settings used to
//! create the emulator session.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_SESSION_SETTINGS_HPP__
#define __ARM_DEBUGGER_SESSION_SETTINGS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <map>

#include <QCoreApplication>
#include <QJsonObject>

#include "MemoryViewOptions.hpp"
#include "ArmEmu/EmuOptions.hpp"
#include "Annotations/Annotation.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
using OrdinalMap = std::map<uint32_t, QString>;

//! @brief An object which encodes the settings used to create the emulator
//! session.
class SessionSettings : public QObject
{
Q_OBJECT
public:
    // Construction/Destruction
    SessionSettings(QObject *ownerObject = nullptr);
    ~SessionSettings() = default;

    // Accessors
    const OrdinalMap &getSwiMap() const;
    void setSwiMap(const OrdinalMap &swiDefinitions);
    const OrdinalMap &getSymbolMap() const;
    void setSymbolMap(const OrdinalMap &symbolDefinitions);
    const Arm::Options &getEmulatorOptions() const;
    void setEmulatorOptions(const Arm::Options &emulatorOptions);
    const AnnotationCollection &getAnnotations() const;
    const MemoryViewOptions &getMemoryViewOptions() const;
    void setMemoryViewOptions(const MemoryViewOptions &options);

    // Operations
    QJsonObject write() const;
    bool read(const QJsonObject &jsonObj);
    bool tryLoad(const QString &filePath, QString &error);
    bool trySave(const QString &filePath, QString &error);

signals:
    void SWIsChanged();
    void symbolsChanged();
    void annotationsChanged();
    void memoryViewOptionsChanged();

private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    Arm::Options _options;
    OrdinalMap _labels;
    OrdinalMap _swiNames;
    AnnotationCollection _annotations;
    MemoryViewOptions _memoryViewOptions;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
bool tryReadOrdinals(const QString &fileName, OrdinalMap &ordinals, QString &error);
bool tryWriteOrdinals(const QString &fileName, const OrdinalMap &ordinals, QString &error);

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
