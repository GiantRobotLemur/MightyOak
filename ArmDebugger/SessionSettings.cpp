//! @file ArmDebugger/SessionSettings.cpp
//! @brief The definition of an object which encodes the settings used to
//! create the emulator session.
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
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>

#include "Ag/QtInterop/Conversion.hpp"

#include "SessionSettings.hpp"
#include "Tools.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
constexpr QJsonDocument::JsonFormat OutputFormat
#ifdef _DEBUG
    = QJsonDocument::Indented;
#else
    = QJsonDocument::Compact;
#endif

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void writeOrdinalArray(QJsonArray &items, const OrdinalMap &elements)
{
    static const QString format("0x%1");
    static const QChar padding('0');

    for (const auto &[id, symbol] : elements)
    {
        QJsonObject pair;

        pair.insert("Key", format.arg(id, 8, 16, padding));
        pair.insert("Value", symbol);

        items.append(pair);
    }
}

void writeOrdinalMap(QJsonObject &parent, Ag::utf8_cptr_t key,
                     const OrdinalMap &elements)
{
    QJsonArray pairs;

    writeOrdinalArray(pairs, elements);

    parent.insert(QString::fromUtf8(key), pairs);
}

void readOrdinalArray(const QJsonArray &jsonArray, OrdinalMap &elements)
{
    for (const QJsonValue &item : jsonArray)
    {
        if (item.isObject())
        {
            QJsonObject pair = item.toObject();
            QString symbol;
            uint32_t id;

            if (tryGetJsonValue(pair, "Key", id) &&
                tryGetJsonValue(pair, "Value", symbol) &&
                (symbol.isEmpty() == false))
            {
                elements[id] = symbol;
            }
        }
    }
}

void readOrdinalMap(const QJsonObject &parent, Ag::utf8_cptr_t key,
                    OrdinalMap &elements)
{
    elements.clear();
    QJsonArray items;

    if (tryGetJsonValue(parent, key, items))
    {
        readOrdinalArray(items, elements);
    }
}

void writeOptions(QJsonObject &parent, Ag::utf8_cptr_t key,
                  const Arm::Options &options)
{
    QJsonObject jsonOpts;

    setJsonValue(jsonOpts, "Architecture", Arm::getSystemModelType(),
                 options.getHardwareArchitecture());
    setJsonValue(jsonOpts, "Processor", Arm::getProcessorModelType(),
                 options.getProcessorVariant());
    setJsonValue(jsonOpts, "ProcessorSpeed", options.getProcessorSpeedMHz());
    setJsonValue(jsonOpts, "RAMSize", options.getRamSizeKb());
    setJsonValue(jsonOpts, "SystemROM", Arm::getSystemROMPresetType(),
                 options.getSystemRom());

    if ((options.getSystemRom() == Arm::SystemROMPreset::Custom) &&
        (options.getRomPath().isEmpty() == false))
    {
        Ag::String pathText = options.getRomPath().toString();

        jsonOpts.insert("SystemROMPath", Ag::Qt::toQString(pathText));
    }

    parent.insert(QString::fromUtf8(key), jsonOpts);
}

bool readOptions(const QJsonObject &parent, Ag::utf8_cptr_t key,
                 Arm::Options &options)
{
    QJsonObject jsonOpts;
    bool isOK = false;

    if (tryGetJsonValue(parent, key, jsonOpts))
    {
        Arm::SystemModel arch;
        isOK = true;

        if (tryGetJsonValue(jsonOpts, "Architecture", Arm::getSystemModelType(), arch))
        {
            options.setHardwareArchitecture(arch);
        }

        Arm::ProcessorModel cpuModel;

        if (tryGetJsonValue(jsonOpts, "Processor", Arm::getProcessorModelType(), cpuModel))
        {
            options.setProcessorVariant(cpuModel);
        }

        uint32_t uint32Value;

        if (tryGetJsonValue(jsonOpts, "ProcessorSpeed", uint32Value))
        {
            options.setProcessorSpeedMHz(static_cast<uint16_t>(uint32Value));
        }

        if (tryGetJsonValue(jsonOpts, "RAMSize", uint32Value))
        {
            options.setRamSizeKb(uint32Value);
        }

        Arm::SystemROMPreset sysRom;

        if (tryGetJsonValue(jsonOpts, "SystemROM", Arm::getSystemROMPresetType(), sysRom))
        {
            QString romPathText;
            options.setSystemRom(sysRom);

            if ((sysRom == Arm::SystemROMPreset::Custom))
            {
                if (tryGetJsonValue(jsonOpts, "SystemROMPath", romPathText))
                {
                    Ag::String romPathTextUtf8 = Ag::Qt::fromQString(romPathText);
                    Ag::Fs::Path romPath;

                    if (Ag::Fs::Path::tryParse(romPathTextUtf8, romPath))
                    {
                        options.setCustomRom(romPath);
                    }
                    else
                    {
                        options.setCustomRom(Ag::Fs::Path());
                    }
                }
                else
                {
                    options.setCustomRom(Ag::Fs::Path());
                }
            }
        }
    }

    return isOK;
}


} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// SessionSettings Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
SessionSettings::SessionSettings(QObject *ownerObject /*= nullptr*/) :
    QObject(ownerObject)
{
}

const OrdinalMap &SessionSettings::getSwiMap() const { return _swiNames; }
void SessionSettings::setSwiMap(const OrdinalMap &swiDefinitions)
{
    _swiNames = swiDefinitions;

    emit SWIsChanged();
}

const OrdinalMap &SessionSettings::getSymbolMap() const { return _labels; }
void SessionSettings::setSymbolMap(const OrdinalMap &symbolDefinitions)
{
    _labels = symbolDefinitions;

    emit symbolsChanged();
}

//Arm::Options &SessionSettings::getEmulatorOptions() { return _options; }
const Arm::Options &SessionSettings::getEmulatorOptions() const { return _options; }

void SessionSettings::setEmulatorOptions(const Arm::Options &emulatorOptions)
{
    if (&emulatorOptions != &_options)
    {
        _options = emulatorOptions;
    }
}

const AnnotationCollection &SessionSettings::getAnnotations() const { return _annotations; }

const MemoryViewOptions &SessionSettings::getMemoryViewOptions() const { return _memoryViewOptions; }
void SessionSettings::setMemoryViewOptions(const MemoryViewOptions &options)
{
    _memoryViewOptions = options;

    emit memoryViewOptionsChanged();
}

QJsonObject SessionSettings::write() const
{
    QJsonObject rootObj;

    writeOptions(rootObj, "EmulatorOptions", _options);
    rootObj.insert("Annotations", writeAnnotations(_annotations));
    writeOrdinalMap(rootObj, "SWIs", _swiNames);
    writeOrdinalMap(rootObj, "Labels", _labels);
    rootObj.insert("MemoryViewOptions", _memoryViewOptions.write());

    return rootObj;
}

bool SessionSettings::read(const QJsonObject &jsonObj)
{
    bool isOK = readOptions(jsonObj, "EmulatorOptions", _options);
    readOrdinalMap(jsonObj, "SWIs", _swiNames);
    readOrdinalMap(jsonObj, "Labels", _labels);

    QJsonArray annotations;
    if (tryGetJsonValue(jsonObj, "Annotations", annotations))
    {
        readAnnotations(annotations, _annotations);
    }
    else
    {
        _annotations.clear();
    }

    QJsonObject memViewJson;
    if (tryGetJsonValue(jsonObj, "MemoryViewOptions", memViewJson))
    {
        _memoryViewOptions.read(memViewJson);
    }
    else
    {
        _memoryViewOptions.reset();
    }

    return isOK;
}

bool SessionSettings::tryLoad(const QString &filePath, QString &error)
{
    QFile inputFile(filePath);
    bool isOK = false;

    if (inputFile.exists())
    {
        if (inputFile.open(QIODevice::ExistingOnly | QIODevice::ReadOnly))
        {
            QByteArray fileContent = inputFile.readAll();
            QJsonParseError parseError;
            QJsonDocument json = QJsonDocument::fromJson(fileContent, &parseError);

            if (json.isNull())
            {
                error = tr("Failed to parse JSON text: %1").arg(parseError.errorString());
            }
            else if (read(json.object()))
            {
                isOK = true;
            }
            else
            {
                error = tr("The session file data was invalid.");
            }
        }
        else
        {
            error = inputFile.errorString();
        }
    }
    else
    {
        error = tr("Session settings file '%1' not found.").arg(filePath);
    }

    return isOK;
}

bool SessionSettings::trySave(const QString &filePath, QString &error)
{
    QFile outputFile(filePath);
    bool isOK = false;

    if (outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QJsonDocument doc;
        doc.setObject(write());

        QByteArray encodedData = doc.toJson(OutputFormat);

        outputFile.write(encodedData);
        isOK = true;
    }
    else
    {
        error = tr("Failed to write session settings to '%1': %2")
            .arg(filePath)
            .arg(outputFile.errorString());
    }

    return isOK;
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
bool tryReadOrdinals(const QString &fileName, OrdinalMap &ordinals, QString &error)
{
    bool isOK = false;
    ordinals.clear();
    error.clear();

    QFile inputFile(fileName);

    if (inputFile.exists())
    {
        if (inputFile.open(QIODevice::ReadOnly))
        {
            QByteArray fileContents = inputFile.readAll();
            QJsonParseError parseError;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(fileContents, &parseError);

            if (jsonDoc.isNull())
            {
                error = SessionSettings::tr("Ordinal mapping file '%1' had an invalid structure:\n\n%2")
                    .arg(fileName)
                    .arg(parseError.errorString());
            }
            else if (jsonDoc.isArray())
            {
                readOrdinalArray(jsonDoc.array(), ordinals);
                isOK = true;
            }
            else
            {
                error = SessionSettings::tr("Ordinal mapping file '%1' had an invalid structure.").arg(fileName);
            }
        }
        else
        {
            error = SessionSettings::tr("Failed to open ordinal mapping file '%1':\n\n%2")
                .arg(fileName)
                .arg(inputFile.errorString());
        }
    }
    else
    {
        error = SessionSettings::tr("Ordinal mapping file '%1' not found.").arg(fileName);
    }

    return isOK;
}

bool tryWriteOrdinals(const QString &fileName, const OrdinalMap &ordinals,
                      QString &error)
{
    bool isOK = false;
    error.clear();

    QFile outputFile(fileName);

    if (outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QJsonArray jsonItems;
        writeOrdinalArray(jsonItems, ordinals);

        QJsonDocument jsonDoc;
        jsonDoc.setArray(jsonItems);

        QByteArray encodedData = jsonDoc.toJson(OutputFormat);
        outputFile.write(encodedData);

        isOK = true;
    }
    else
    {
        error = SessionSettings::tr("Failed to create file '%1' for writing.")
            .arg(outputFile.fileName());
    }

    return isOK;
}


} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

