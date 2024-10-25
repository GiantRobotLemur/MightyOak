//! @file ArmDebugger/Annotations/Annotation.cpp
//! @brief The definition of a class representing an abstract annotation of
//! the memory contents of an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Annotation.hpp"

#include "Ag/Core/EnumInfo.hpp"

#include "AssemblyLabel.hpp"
#include "DataReplacement.hpp"
#include "InstructionFormat.hpp"
#include "Tools.hpp"
#include "TypeOverride.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct CompareAnnotationByAddress
{
    bool operator()(const AnnotationUPtr &lhs, const AnnotationUPtr &rhs) const
    {
        return lhs->getBaseAddress() < rhs->getBaseAddress();
    }
};

struct CompareAnnotationByAddressAndSize
{
    bool operator()(const AnnotationUPtr &lhs, const AnnotationUPtr &rhs) const
    {
        bool isLess = false;

        if (lhs->getBaseAddress() == rhs->getBaseAddress())
        {
            isLess = (lhs->getLength() < rhs->getLength());
        }
        else
        {
            isLess = (lhs->getBaseAddress() < rhs->getBaseAddress());
        }

        return isLess;
    }
};

class KeyAnnotation : public Annotation
{
public:
    KeyAnnotation(uint32_t baseAddress) :
        Annotation(AnnotationType::Max, baseAddress, 0)
    {
    }

    virtual void read(const QJsonObject &/*jsonObj*/) override { }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
const Ag::EnumInfo<AnnotationType> &getAnnotationTypeInfo()
{
    static Ag::EnumInfo<AnnotationType> info({
        { AnnotationType::DataReplacement, "DataReplacement" },
        { AnnotationType::DataType, "DataType" },
        { AnnotationType::Format, "Format" },
    });

    return info;
}


} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// Annotation Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
Annotation::Annotation(AnnotationType type) :
    _baseAddress(0),
    _length(0),
    _type(type)
{
}

Annotation::Annotation(AnnotationType type, uint32_t baseAddr, uint32_t length) :
    _baseAddress(baseAddr),
    _length(length),
    _type(type)
{
}

AnnotationType Annotation::getType() const { return _type; }
uint32_t Annotation::getBaseAddress() const { return _baseAddress; }
uint32_t Annotation::getEndAddress() const { return _baseAddress + _length; }
uint32_t Annotation::getLength() const { return _length; }
bool Annotation::containsAddress(uint32_t address) const
{
    return (address >= _baseAddress) && (address < getEndAddress());
}

QJsonObject Annotation::write() const
{
    QJsonObject obj;
    auto typeName = getAnnotationTypeInfo().toString(_type);

    obj.insert("Type", QString::fromUtf8(typeName.data(), typeName.length()));
    obj.insert("BaseAddress", static_cast<qint64>(_baseAddress));

    if (_length > 0)
    {
        obj.insert("Length", static_cast<qint64>(_length));
    }

    return obj;
}

AnnotationUPtr Annotation::readAnnotation(const QJsonObject &jsonObj)
{
    AnnotationUPtr annotationPtr;
    AnnotationType objType;
    uint32_t baseAddress = 0;
    uint32_t length;

    if (tryGetJsonValue(jsonObj, "Type", getAnnotationTypeInfo(), objType) &&
        tryGetJsonValue(jsonObj, "BaseAddress", baseAddress))
    {
        if (tryGetJsonValue(jsonObj, "Length", length) == false)
        {
            length = 0;
        }

        switch (objType)
        {
        case AnnotationType::DataReplacement:
            annotationPtr = std::make_unique<DataReplacement>();
            break;

        case AnnotationType::DataType:
            annotationPtr = std::make_unique<TypeOverride>();
            break;

        case AnnotationType::Format:
            annotationPtr = std::make_unique<InstructionFormat>();
            break;

        default:
            break;
        }

        if (annotationPtr)
        {
            annotationPtr->_baseAddress = baseAddress;
            annotationPtr->_length = length;
            annotationPtr->read(jsonObj);
        }
    }

    return annotationPtr;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////
void readAnnotations(const QJsonArray &jsonAnnotations,
                     AnnotationCollection &annotations)
{
    annotations.clear();

    for (const auto &value : jsonAnnotations)
    {
        if (value.isObject())
        {
            auto annotationPtr = Annotation::readAnnotation(value.toObject());

            if (annotationPtr)
            {
                annotations.emplace_back(std::move(annotationPtr));
            }
        }
    }

    // Ensure annotations are sorted when loaded.
    sortAnnotations(annotations);
}

QJsonArray writeAnnotations(const AnnotationCollection &annotations)
{
    QJsonArray jsonArray;

    for (const auto &annotation : annotations)
    {
        QJsonObject jsonAnnotation = annotation->write();

        if (jsonAnnotation.isEmpty() == false)
        {
            jsonArray.append(jsonAnnotation);
        }
    }

    return jsonArray;
}

void sortAnnotations(AnnotationCollection &annotations)
{
    std::sort(annotations.begin(), annotations.end(),
              CompareAnnotationByAddress());
}

bool tryFindAnnotation(const AnnotationCollection &annotations,
                       uint32_t targetAddress, size_t &index)
{
    KeyAnnotation key(targetAddress);
    index = annotations.size();
    bool isFound = false;

    if (annotations.empty() == false)
    {
        // NOTE: We cannot allows this to be destroyed before being released.
        AnnotationUPtr keyPtr(&key);

        try
        {
            auto pos = std::lower_bound(annotations.begin(), annotations.end(), keyPtr,
                                        CompareAnnotationByAddress());

            index = std::distance(annotations.begin(), pos);

            if (pos == annotations.begin())
            {
                isFound = (*pos)->containsAddress(targetAddress);
            }
            else if (pos == annotations.end())
            {
                // The address might be within the last block.
                if (annotations.back()->containsAddress(targetAddress))
                {
                    index = annotations.size() - 1;
                    isFound = true;
                }
            }
            else if ((*pos)->getBaseAddress() == targetAddress)
            {
                // The address could by at the very beginning of the block found.
                isFound = true;
            }
            else
            {
                --pos;
                if ((*pos)->containsAddress(targetAddress))
                {
                    index = std::distance(annotations.begin(), pos);
                    isFound = true;
                }
            }
        }
        catch(...)
        {
            // Ensure we don't try to dynamically free the key constructed
            // on the stack.
            keyPtr.release();
            throw;
        }

        // See above.
        keyPtr.release();
    }

    return isFound;
}

//! @brief Finds the first annotation with a base address greater than or
//! equal to a target address.
//! @param[in] annotations A sorted collection of annotations to search.
//! @param[in] targetAddress The address of the annotation to look up.
//! @return The index of the next applicable annotation, possibly the
//! size of the collection (meaning none).
size_t findAnnotationLowerBound(const AnnotationCollection &annotations,
                                uint32_t targetAddress)
{
    KeyAnnotation key(targetAddress);
    size_t index = annotations.size();

    if (annotations.empty() == false)
    {
        // NOTE: We cannot allows this to be destroyed before being released.
        AnnotationUPtr keyPtr(&key);

        try
        {
            auto pos = std::lower_bound(annotations.begin(), annotations.end(), keyPtr,
                                        CompareAnnotationByAddress());

            index = std::distance(annotations.begin(), pos);
        }
        catch (...)
        {
            // Ensure we don't try to dynamically free the key constructed
            // on the stack.
            keyPtr.release();
            throw;
        }

        // See above.
        keyPtr.release();
    }

    return index;
}


} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

