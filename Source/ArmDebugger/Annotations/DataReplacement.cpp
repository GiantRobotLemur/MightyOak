//! @file ArmDebugger/Annotations/DataReplacement.cpp
//! @brief The definition of an annotation which replaces the emulator memory
//! contents to be interpreted for display to the user.
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

#include "DataReplacement.hpp"
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

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// DataReplacement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
DataReplacement::DataReplacement() :
    Annotation(AnnotationType::DataReplacement)
{
}

DataReplacement::DataReplacement(uint32_t startAddress, const uint32_t *data,
                                 uint32_t length) :
    Annotation(AnnotationType::DataReplacement, startAddress, length & ~3u)
{
    size_t wordCount = length / 4;
    _replacementWords.reserve(wordCount);

    std::copy_n(data, wordCount, std::back_inserter(_replacementWords));
}

const uint32_t *DataReplacement::replacementData() const
{
    return _replacementWords.data();
}

QJsonObject DataReplacement::write() const
{
    QJsonObject obj = Annotation::write();

    if (_replacementWords.empty() == false)
    {
        QJsonArray words;

        for (uint32_t word : _replacementWords)
        {
            words.append(static_cast<qint64>(word));
        }

        obj.insert("Data", words);
    }

    return obj;
}

void DataReplacement::read(const QJsonObject &jsonObj)
{
    _replacementWords.clear();

    QJsonValue dataValue;

    if (tryGetJsonValue(jsonObj, "Data", dataValue) &&
        dataValue.isArray())
    {
        QJsonArray dataArray = dataValue.toArray();
        _replacementWords.reserve(dataArray.size());

        for (const auto &jsonValue : dataArray)
        {
            if (jsonValue.isDouble())
            {
                uint32_t word = static_cast<uint32_t>(jsonValue.toInteger());

                _replacementWords.push_back(word);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

