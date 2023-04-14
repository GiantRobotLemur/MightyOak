//! @file DataDirectiveStatement.cpp
//! @brief The definition of an object representing an assembly language
//! statement which defines one or more data elements to embed in code.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ConstantSet.hpp"
#include "DataDirectiveStatement.hpp"
#include "ExprContexts.hpp"
#include "LexicalAnalysers.hpp"
#include "SymbolTable.hpp"

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief A sub-class of DataDirectiveStatement which produces a count of
//! object code bytes which isn't dependent upon the count of values.
class VariableSizeDataStatement : public DataDirectiveStatement
{
public:
    // Construction/Destruction
    VariableSizeDataStatement(const Location &at, IExprUPtrCollection &&expressions) :
        DataDirectiveStatement(at, std::move(expressions))
    {
    }

    virtual ~VariableSizeDataStatement() = default;

    // Overrides
    // Inherited from Statement.
    virtual uint32_t calculateObjectCodeSize(IEvalContext *context) const override
    {
        // Actually assemble the object code to measure its size.
        Messages messages;
        ObjectCodeBuilder builder(messages, 0, 0);
        uint32_t objectCodeSize = 0;

        if (assemble(AssemblyState::Empty, context, builder))
        {
            objectCodeSize = static_cast<uint32_t>(builder.getSize());
        }

        return objectCodeSize;
    }
};

//! @brief A sub-class of DataDirectiveStatement which can encode bytes and
//! 8-bit character strings.
class ByteDataStatement : public VariableSizeDataStatement
{
public:
    // Construction/Destruction
    ByteDataStatement(const Location &at, IExprUPtrCollection &&expressions) :
        VariableSizeDataStatement(at, std::move(expressions))
    {
    }

    virtual ~ByteDataStatement() = default;

    // Overrides
    // Inherited from DataDirectiveStatement.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const override
    {
        bool isAssembled = false;
        auto &output = builder.getOutput();
        Value converted;

        switch (value.getDataType())
        {
        case DataType::Int32:
        case DataType::Uint32:
        case DataType::Int64:
        case DataType::Uint64:
            if (value.tryConvert(DataType::Int32, converted) &&
                (converted.asInt32() >= INT8_MIN) &&
                (converted.asInt32() <= UINT8_MAX))
            {
                builder.writeByte(static_cast<uint8_t>(converted.asInt32()));
                isAssembled = true;
            }
            else
            {
                output.appendError(getLocation(),
                                   "Data value is beyond the range of values "
                                   "which can be encoded as a byte.");
            }
            break;

        case DataType::Float:
        case DataType::Double:
        case DataType::ExtendedReal:
            output.appendError(getLocation(),
                               "Real data types cannot be encoded as a byte.");
            break;

        case DataType::String:
            // TODO: Should we be encoding the string as a specific character set?
            builder.writeBytes(value.asString().getUtf8Bytes(),
                               value.asString().getUtf8Length());
            isAssembled = true;
            break;

        case DataType::Character:
            // TODO: Should we be encoding the character using a
            // specific character set?
            if (value.asCharacter() < 256)
            {
                builder.writeByte(static_cast<uint8_t>(value.asCharacter()));
            }
            else
            {
                output.appendError(getLocation(),
                                   "The specified character cannot be "
                                   "encoded as a single byte.");
            }
            break;

        default:
            output.appendError(getLocation(),
                               "Unsupported expression type.");
            break;
        }

        return isAssembled;
    }
};

//! @brief A sub-class of DataDirectiveStatement which can encode 16-bit
//! half-words.
class HalfWordDataStatement : public DataDirectiveStatement
{
public:
    // Construction/Destruction
    HalfWordDataStatement(const Location &at,
                          IExprUPtrCollection &&expressions) :
        DataDirectiveStatement(at, std::move(expressions))
    {
    }

    virtual ~HalfWordDataStatement() = default;

    // Overrides
    // Inherited from Statement.
    virtual uint32_t calculateObjectCodeSize(IEvalContext * /* context */) const override
    {
        return static_cast<uint32_t>(getValueExpressions().size() * sizeof(uint16_t));
    }

    // Inherited from DataDirectiveStatement.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const override
    {
        bool isAssembled = false;
        auto &output = builder.getOutput();
        Value converted;

        switch (value.getDataType())
        {
        case DataType::Int32:
        case DataType::Uint32:
        case DataType::Int64:
        case DataType::Uint64:
            if (value.tryConvert(DataType::Int32, converted) &&
                (converted.asInt32() >= INT16_MIN) &&
                (converted.asInt32() <= UINT16_MAX))
            {
                builder.writeHalfWord(static_cast<uint16_t>(converted.asInt32()));
                isAssembled = true;
            }
            else
            {
                output.appendError(getLocation(),
                                   "Data value is beyond the range of values "
                                   "which can be encoded as a 16-bit word.");
            }
            break;

        case DataType::Float:
        case DataType::Double:
        case DataType::ExtendedReal:
            output.appendError(getLocation(),
                               "Real data types cannot be encoded as a 16-bit words.");
            break;

        case DataType::Character:
        case DataType::String:
            output.appendError(getLocation(),
                               "Character data cannot be encoded as 16-bit words.");
            break;

        default:
            output.appendError(getLocation(),
                               "Unsupported expression type.");
            break;
        }

        return isAssembled;
    }
};

//! @brief A sub-class of DataDirectiveStatement which can encode 32-bit
//! words.
class WordDataStatement : public DataDirectiveStatement
{
public:
    // Construction/Destruction
    WordDataStatement(const Location &at,
                      IExprUPtrCollection &&expressions) :
        DataDirectiveStatement(at, std::move(expressions))
    {
    }

    virtual ~WordDataStatement() = default;

    // Overrides
    // Inherited from Statement.
    virtual uint32_t calculateObjectCodeSize(IEvalContext */* context */) const override
    {
        return static_cast<uint32_t>(getValueExpressions().size() * sizeof(uint32_t));
    }

    // Inherited from DataDirectiveStatement.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const override
    {
        bool isAssembled = false;
        auto &output = builder.getOutput();
        Value converted;

        switch (value.getDataType())
        {
        case DataType::Int32:
            builder.writeWord(static_cast<uint32_t>(value.asInt32()));
            isAssembled = true;
            break;

        case DataType::Uint32:
            builder.writeWord(value.asUint32());
            isAssembled = true;
            break;

        case DataType::Int64:
            if (value.tryConvert(DataType::Int32, converted))
            {
                builder.writeWord(static_cast<uint32_t>(converted.asInt32()));
                isAssembled = true;
            }
            else
            {
                output.appendError(getLocation(),
                                   "Data value is beyond the range of values "
                                   "which can be encoded as a 32-bit word.");
            }
            break;

        case DataType::Uint64:
            if (value.tryConvert(DataType::Uint32, converted))
            {
                builder.writeWord(converted.asUint32());
                isAssembled = true;
            }
            else
            {
                output.appendError(getLocation(),
                                   "Data value beyond the range of values "
                                   "which can be encoded as a 32-bit word.");
            }
            break;

        case DataType::Float:
        case DataType::Double:
        case DataType::ExtendedReal:
            output.appendError(getLocation(),
                               "Real data types cannot be encoded as a 32-bit words.");
            break;

        case DataType::Character:
        case DataType::String:
            output.appendError(getLocation(),
                               "Character data cannot be encoded as 32-bit words.");
            break;

        default:
            output.appendError(getLocation(),
                               "Unsupported expression type.");
            break;
        }

        return isAssembled;
    }
};

//! @brief A sub-class of DataDirectiveStatement which can encode 64-bit
//! long words.
class LongWordDataStatement : public DataDirectiveStatement
{
public:
    // Construction/Destruction
    LongWordDataStatement(const Location &at,
                          IExprUPtrCollection &&expressions) :
        DataDirectiveStatement(at, std::move(expressions))
    {
    }

    virtual ~LongWordDataStatement() = default;

    // Overrides
    // Inherited from Statement.
    virtual uint32_t calculateObjectCodeSize(IEvalContext */* context */) const override
    {
        return static_cast<uint32_t>(getValueExpressions().size() * sizeof(uint64_t));
    }

    // Inherited from DataDirectiveStatement.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const override
    {
        bool isAssembled = false;
        auto &output = builder.getOutput();
        Value converted;

        switch (value.getDataType())
        {
        case DataType::Int32:
            // NOTE: Ensure the sign is extended.
            builder.writeLongWord(static_cast<int64_t>(value.asInt32()));
            isAssembled = true;
            break;

        case DataType::Uint32:
            builder.writeLongWord(value.asUint32());
            isAssembled = true;
            break;

        case DataType::Int64:
            builder.writeLongWord(value.asInt64());
            break;

        case DataType::Uint64:
            builder.writeLongWord(value.asUint64());
            break;

        case DataType::Float:
        case DataType::Double:
        case DataType::ExtendedReal:
            output.appendError(getLocation(),
                               "Real data types cannot be encoded as a 64-bit words.");
            break;

        case DataType::Character:
        case DataType::String:
            output.appendError(getLocation(),
                               "Character data cannot be encoded as 64-bit words.");
            break;

        default:
            output.appendError(getLocation(),
                               "Unsupported expression type.");
            break;
        }

        return isAssembled;
    }
};

//! @brief A sub-class of DataDirectiveStatement which can encode UTF-16
//! characters.
class UTF16DataStatement : public VariableSizeDataStatement
{
public:
    // Construction/Destruction
    UTF16DataStatement(const Location &at,
                      IExprUPtrCollection &&expressions) :
        VariableSizeDataStatement(at, std::move(expressions))
    {
    }

    virtual ~UTF16DataStatement() = default;

    // Overrides
    // Inherited from DataDirectiveStatement.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const override
    {
        bool isAssembled = false;
        auto &output = builder.getOutput();
        Value converted;

        switch (value.getDataType())
        {
        case DataType::Int32:
        case DataType::Uint32:
        case DataType::Int64:
        case DataType::Uint64:
            if (value.tryConvert(DataType::Uint32, converted) &&
                (converted.asUint32() <= 0xFFFF))
            {
                builder.writeHalfWord(static_cast<uint16_t>(converted.asUint32()));
                isAssembled = true;
            }
            else
            {
                output.appendError(getLocation(),
                                   "The integer value is beyond the range "
                                   "of that which can be encoded as a UTF-16 "
                                   "character.");
            }
            break;

        case DataType::Float:
        case DataType::Double:
        case DataType::ExtendedReal:
            output.appendError(getLocation(),
                               "Real data types cannot be encoded as a text.");
            break;

        case DataType::Character:
        case DataType::String:
            if (value.tryConvert(DataType::String, converted))
            {
                // Encode the string as UTF-16.
                std::u16string text = converted.asString().toUtf16();

                // Write the characters allowing byte swapping to take place.
                for (auto ch : text)
                {
                    builder.writeHalfWord(static_cast<uint16_t>(ch));
                }

                isAssembled = true;
            }
            break;

        default:
            output.appendError(getLocation(),
                               "Unsupported expression type.");
            break;
        }

        return isAssembled;
    }
};

//! @brief A sub-class of DataDirectiveStatement which can encode 32-bit
//! Unicode code points.
class UTF32DataStatement : public VariableSizeDataStatement
{
public:
    // Construction/Destruction
    UTF32DataStatement(const Location &at,
                       IExprUPtrCollection &&expressions) :
        VariableSizeDataStatement(at, std::move(expressions))
    {
    }

    virtual ~UTF32DataStatement() = default;

    // Overrides
    // Inherited from DataDirectiveStatement.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const override
    {
        bool isAssembled = false;
        auto &output = builder.getOutput();
        Value converted;

        switch (value.getDataType())
        {
        case DataType::Int32:
        case DataType::Uint32:
        case DataType::Int64:
        case DataType::Uint64:
            if (value.tryConvert(DataType::Uint32, converted))
            {
                builder.writeWord(static_cast<uint32_t>(converted.asUint32()));
                isAssembled = true;
            }
            else
            {
                output.appendError(getLocation(),
                                   "The integer value is beyond the range "
                                   "of that which can be encoded as a Unicode "
                                   "code point.");
            }
            break;

        case DataType::Float:
        case DataType::Double:
        case DataType::ExtendedReal:
            output.appendError(getLocation(),
                               "Real data types cannot be encoded as a text.");
            break;

        case DataType::Character:
        case DataType::String:
            if (value.tryConvert(DataType::String, converted))
            {
                // Write the Unicode code points allowing byte swapping to
                // take place.
                for (auto ch : converted.asString())
                {
                    builder.writeWord(static_cast<uint32_t>(ch));
                }

                isAssembled = true;
            }
            break;

        default:
            output.appendError(getLocation(),
                               "Unsupported expression type.");
            break;
        }

        return isAssembled;
    }
};

//! @brief A sub-class of DataDirectiveStatement which can encode 32-bit
//! floating point values.
class FloatDataStatement : public DataDirectiveStatement
{
public:
    // Construction/Destruction
    FloatDataStatement(const Location &at,
                       IExprUPtrCollection &&expressions) :
        DataDirectiveStatement(at, std::move(expressions))
    {
    }

    virtual ~FloatDataStatement() = default;

    // Overrides
    // Inherited from Statement.
    virtual uint32_t calculateObjectCodeSize(IEvalContext */* context */) const override
    {
        return static_cast<uint32_t>(getValueExpressions().size() * sizeof(float));
    }

    // Inherited from DataDirectiveStatement.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const override
    {
        bool isAssembled = false;
        auto &output = builder.getOutput();
        Value converted;

        switch (value.getDataType())
        {
        case DataType::Int32:
        case DataType::Uint32:
        case DataType::Int64:
        case DataType::Uint64:
        case DataType::Float:
        case DataType::Double:
            if (value.tryConvert(DataType::Float, converted))
            {
                float realValue = converted.asFloat();
                builder.writeWord(*reinterpret_cast<const uint32_t *>(&realValue));
                isAssembled = true;
            }
            else
            {
                output.appendError(getLocation(),
                                   "The value cannot be converted to a 32-bit "
                                   "floating point type.");
            }
            break;


        case DataType::ExtendedReal:
            output.appendError(getLocation(),
                               "96-bit real data types are not supported.");
            break;

        case DataType::Character:
        case DataType::String:
            output.appendError(getLocation(),
                               "Character types cannot be converted to "
                               "real numeric values.");
            break;

        default:
            output.appendError(getLocation(),
                               "Unsupported expression type.");
            break;
        }

        return isAssembled;
    }
};

//! @brief A sub-class of DataDirectiveStatement which can encode 32-bit
//! floating point values.
class DoubleDataStatement : public DataDirectiveStatement
{
public:
    // Construction/Destruction
    DoubleDataStatement(const Location &at,
                        IExprUPtrCollection &&expressions) :
        DataDirectiveStatement(at, std::move(expressions))
    {
    }

    virtual ~DoubleDataStatement() = default;

    // Overrides
    // Inherited from Statement.
    virtual uint32_t calculateObjectCodeSize(IEvalContext */* context */) const override
    {
        return static_cast<uint32_t>(getValueExpressions().size() * sizeof(double));
    }

    // Inherited from DataDirectiveStatement.
    virtual bool assembleValue(const Value &value,
                               ObjectCodeBuilder &builder) const override
    {
        bool isAssembled = false;
        auto &output = builder.getOutput();
        Value converted;

        switch (value.getDataType())
        {
        case DataType::Int32:
        case DataType::Uint32:
        case DataType::Int64:
        case DataType::Uint64:
        case DataType::Float:
        case DataType::Double:
            if (value.tryConvert(DataType::Float, converted))
            {
                float realValue = converted.asFloat();
                builder.writeWord(*reinterpret_cast<const uint32_t *>(&realValue));
                isAssembled = true;
            }
            else
            {
                output.appendError(getLocation(),
                                   "The value cannot be converted to a 64-bit "
                                   "floating point type.");
            }
            break;


        case DataType::ExtendedReal:
            output.appendError(getLocation(),
                               "96-bit real data types are not supported.");
            break;

        case DataType::Character:
        case DataType::String:
            output.appendError(getLocation(),
                               "Character types cannot be converted to "
                               "real numeric values.");
            break;

        default:
            output.appendError(getLocation(),
                               "Unsupported expression type.");
            break;
        }

        return isAssembled;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // TED


////////////////////////////////////////////////////////////////////////////////
// AssemblyDirectiveNode Member Functions Definitions
////////////////////////////////////////////////////////////////////////////////
DataDirectiveNode::DataDirectiveNode(ParseContext &context,
                                     DirectiveDataType dataType,
                                     const Location &start) :
    StatementNode(context, start),
    _state(State::AfterMnemonic),
    _dataType(dataType),
    _lexicalStackBase(context.pushLexicalContext(getExpressionLexer()))
{
}

// Inherited from ISyntaxNode.
bool DataDirectiveNode::isComplete() const
{
    return (_state == State::BeforeSeparator) || (_state == State::Complete);
}

// Inherited from ISyntaxNode.
bool DataDirectiveNode::isValid() const
{
    return _values.empty() == false;
}

// Inherited from ISyntaxNode.
ISyntaxNode *DataDirectiveNode::applyToken(ParseContext &context,
                                               const Token &token)
{
    ISyntaxNode *result = nullptr;

    if (_state == State::AfterMnemonic)
    {
        switch (token.getClass())
        {
        case TokenClass::StatementTerminator:
            context.getMessages().appendError(token.getLocation(),
                                              "No data specified after data directive mnemonic");

            // Pop the expression tokeniser.
            context.restoreLexicalState(_lexicalStackBase);
            _state = State::Complete;
            result = this;
            break;

        case TokenClass::Comma:
            _state = State::AfterSeparator;
            context.getMessages().appendError(token.getLocation(),
                                              "Missing data expression.");
            result = this;
            break;

        default: break;
        }
    }
    else if (_state == State::BeforeSeparator)
    {
        switch (token.getClass())
        {
        case TokenClass::Comma:
            _state = State::AfterSeparator;
            _lexicalStackBase = context.pushLexicalContext(getExpressionLexer());
            result = this;
            break;

        case TokenClass::StatementTerminator:
            // Pop the expression tokeniser.
            context.restoreLexicalState(_lexicalStackBase);
            _state = State::Complete;
            result = this;
            break;

        default: break;
        }
    }
    else if (_state == State::AfterSeparator)
    {
        switch (token.getClass())
        {
        case TokenClass::Comma:
            context.getMessages().appendError(token.getLocation(),
                                              "Empty data expression.");
            result = this;
            break;

        case TokenClass::StatementTerminator:
            // Pop the expression tokeniser.
            context.restoreLexicalState(_lexicalStackBase);
            context.getMessages().appendError(token.getLocation(),
                                              "Invalid trailing data item separator.");
            _state = State::Complete;
            result = this;
            break;

        default: break;
        }
    }

    return result;
}
// Inherited from ISyntaxNode.
ISyntaxNode *DataDirectiveNode::applyNode(ParseContext &context,
                                          ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;

    if ((_state == State::AfterMnemonic) || (_state == State::AfterSeparator))
    {
        ExpressionNodePtr expression = nullptr;

        if (tryCast(childNode, expression))
        {
            // Ensure the lexical state is properly restored.
            context.restoreLexicalState(_lexicalStackBase);

            _values.emplace_back(expression);
            _state = State::BeforeSeparator;
            result = this;
        }
        else
        {
            context.getMessages().appendError(getStart(),
                                              "Unexpected syntax.");
        }
    }

    return result;
}

// Inherited from ISyntaxNode.
void DataDirectiveNode::recover(ParseContext &context, ISyntaxNode *node)
{
    ExpressionNode *expression = nullptr;

    if ((_state == State::BeforeSeparator) && tryCast(node, expression))
    {
        _values.emplace_back(expression);

        context.getMessages().appendError(getStart(),
                                          "Unexpected expression, perhaps a "
                                          "separator (,) is missing?");
    }
    else if (_state != State::Complete)
    {
        // Force completion of the statement.
        context.restoreLexicalState(_lexicalStackBase);

        Token next;
        while (context.tryGetNextToken(next))
        {
            if (next.getClass() == TokenClass::StatementTerminator)
            {
                context.ungetToken(next);
                break;
            }
        }

        safeDelete(node);

        _state = State::Complete;
    }
}

// Inherited from StatementNode.
Statement *DataDirectiveNode::compile(Messages &output) const
{
    DataDirectiveStatement *statement = nullptr;

    if (_values.empty() == false)
    {
        IExprUPtrCollection expressions;
        expressions.reserve(_values.size());

        // There are no special constants for a data directive.
        ConstantSet emptyConstants;

        for (const auto &expression : _values)
        {
            expressions.emplace_back(constantOptimise(expression->compile(emptyConstants)));

            if (expressions.back().operator bool() == false)
            {
                // The expression failed to compile.
                expressions.pop_back();
            }
        }

        if (expressions.empty() == false)
        {
            switch (_dataType)
            {
            case DirectiveDataType::Byte:
                statement = new ByteDataStatement(getStart(),
                                                  std::move(expressions));
                break;

            case DirectiveDataType::HalfWord:
                statement = new HalfWordDataStatement(getStart(),
                                                      std::move(expressions));
                break;

            case DirectiveDataType::Word:
                statement = new WordDataStatement(getStart(),
                                                  std::move(expressions));
                break;

            case DirectiveDataType::LongWord:
                statement = new LongWordDataStatement(getStart(),
                                                      std::move(expressions));
                break;

            case DirectiveDataType::NativeString:
                statement = new ByteDataStatement(getStart(),
                                                  std::move(expressions));
                break;

            case DirectiveDataType::Utf8String:
                statement = new ByteDataStatement(getStart(),
                                                  std::move(expressions));
                break;

            case DirectiveDataType::Utf16String:
                statement = new UTF16DataStatement(getStart(),
                                                   std::move(expressions));
                break;

            case DirectiveDataType::Utf32String:
                statement = new UTF32DataStatement(getStart(),
                                                  std::move(expressions));
                break;

            case DirectiveDataType::Real32:
                statement = new FloatDataStatement(getStart(),
                                                   std::move(expressions));
                break;

            case DirectiveDataType::Real64:
                statement = new DoubleDataStatement(getStart(),
                                                    std::move(expressions));
                break;

            case DirectiveDataType::Real96:
            default:
                output.appendError(getStart(),
                                   "Unsupported data directive type.");
                break;
            }
        }
    }

    return statement;
}

////////////////////////////////////////////////////////////////////////////////
// DataDirectiveStatement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
DataDirectiveStatement::DataDirectiveStatement(const Location &at,
                                               IExprUPtrCollection &&expressions) :
    Statement(StatementType::DataDirective),
    _at(at),
    _values(std::move(expressions))
{
}

// Inherited from Statement.
bool DataDirectiveStatement::assemble(const AssemblyState &/*state*/,
                                      IEvalContext *context,
                                      ObjectCodeBuilder &output) const
{
    bool allValuesAdded = true;
    IScopedContext *offsetContext = dynamic_cast<IScopedContext *>(context);

    for (const auto &expr : _values)
    {
        String error;
        Value value;

        // HACK: Sneakily update the current context as we generate data.
        if (offsetContext != nullptr)
        {
            offsetContext->setAssemblyOffset(output.getCurrentOffset());
        }

        if (expr->tryEvaluate(context, value, error))
        {
            assembleValue(value, output);
        }
        else if (output.isFinalPass())
        {
            output.getOutput().appendError(_at,
                                           "Error in data value expression: {0}",
                                           { error });
            allValuesAdded = false;
            break;
        }
        else
        {
            // Assembly needs to be deferred until additional symbols are defined.
            allValuesAdded = false;

            // No need to continue.
            break;
        }
    }

    return allValuesAdded;
}

//! @brief Gets the location of the directive mnemonic in source code.
const Location &DataDirectiveStatement::getLocation() const
{
    return _at;
}

//! @brief Gets the collection of expressions which evaluate to values.
const IExprUPtrCollection &DataDirectiveStatement::getValueExpressions() const
{
    return _values;
}

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////

