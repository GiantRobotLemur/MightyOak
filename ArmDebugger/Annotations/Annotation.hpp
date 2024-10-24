//! @file ArmDebugger/Annotations/Annotation.hpp
//! @brief The declaration of a class representing an abstract annotation of
//! the memory contents of an emulated system.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_ANNOTATION_HPP__
#define __ARM_DEBUGGER_ANNOTATION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <memory>
#include <vector>

#include <QJsonArray>
#include <QJsonObject>

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
enum class AnnotationType : uint8_t
{
    DataReplacement,
    DataType,
    Format,

    Max
};


////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief A class representing an abstract annotation of the memory contents
//! of an emulated system.
class Annotation
{
public:
    // Construction/Destruction
    Annotation(AnnotationType type);
    Annotation(AnnotationType type, uint32_t baseAddr, uint32_t length);
    virtual ~Annotation() = default;

    // Accessors
    AnnotationType getType() const;
    uint32_t getBaseAddress() const;
    uint32_t getEndAddress() const;
    uint32_t getLength() const;
    bool containsAddress(uint32_t address) const;

    // Operations
    virtual QJsonObject write() const;
    virtual void read(const QJsonObject &jsonObj) =0;

    static std::unique_ptr<Annotation> readAnnotation(const QJsonObject &jsonObj);
private:
    // Internal Types

    // Internal Functions

    // Internal Fields
    uint32_t _baseAddress;
    uint32_t _length;
    AnnotationType _type;
};

using AnnotationUPtr = std::unique_ptr<Annotation>;
using AnnotationCollection = std::vector<AnnotationUPtr>;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////
void readAnnotations(const QJsonArray &jsonAnnotations,
                     AnnotationCollection &annotations);
QJsonArray writeAnnotations(const AnnotationCollection &annotations);
void sortAnnotations(AnnotationCollection &annotations);
bool tryFindAnnotation(const AnnotationCollection &annotations,
                       uint32_t targetAddress, size_t &index);
size_t findAnnotationLowerBound(const AnnotationCollection &annotations,
                                uint32_t targetAddress);


////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
