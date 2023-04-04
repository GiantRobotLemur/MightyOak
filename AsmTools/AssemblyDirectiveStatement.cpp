//! @file AssemblyDirectiveStatement.cpp
//! @brief The definition of an object representing a statement containing an
//! assembly directive.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "AssemblyDirectiveStatement.hpp"
#include "ConstantSet.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"
#include "Token.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Ag {
namespace Asm {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! A base class for statements which update assembly state.
class UpdateAssemblyStateStatement : public Statement
{
public:
    // Construction/Destruction
    //! @brief Constructs an assembly directive statement.
    UpdateAssemblyStateStatement() :
        Statement(StatementType::AssemblyDirective)
    {
    }

    virtual ~UpdateAssemblyStateStatement() = default;
};

//! @brief A statement which updates the enabled instruction set.
class UpdateInstructionSetStatement : public UpdateAssemblyStateStatement
{
private:
    // Internal Fields
    InstructionSet _instructionSet;
public:
    // Construction/Destruction
    //! @brief Constructs a statement which updates the enabled instruction set.
    //! @param[in] instructionSet The new instruction set to enable.
    UpdateInstructionSetStatement(InstructionSet instructionSet) :
        _instructionSet(instructionSet)
    {
    }

    virtual ~UpdateInstructionSetStatement() = default;

    // Overrides
    // Inherited from UpdateAssemblyStateStatement.
    virtual bool updateAssemblyState(AssemblyState &state) const override
    {
        bool isModified = false;

        if (state.getInstructionSet() != _instructionSet)
        {
            state.setInstructionSet(_instructionSet);
            isModified = true;
        }

        return isModified;
    }
};

//! @brief A statement which enables an extension instruction set.
class EnableExtensionStatement : public UpdateAssemblyStateStatement
{
private:
    // Internal Fields
    ArchExtensionEnum _extension;
public:
    // Construction/Destruction
    //! @brief Constructs a statement which enables an extension instruction set.
    //! @param[in] extension The extension instruction set to enable.
    EnableExtensionStatement(ArchExtensionEnum extension) :
        _extension(extension)
    {
    }

    virtual ~EnableExtensionStatement() = default;

    // Overrides
    // Inherited from UpdateAssemblyStateStatement.
    virtual bool updateAssemblyState(AssemblyState &state) const override
    {
        bool isModified = false;

        if (state.isValidExtension(_extension) == false)
        {
            state.addExtension(_extension);
            isModified = true;
        }

        return isModified;
    }
};

//! @brief A statement which updates the expected processor mode.
class UpdateProcessorModeStatement : public UpdateAssemblyStateStatement
{
private:
    // Internal Fields
    ProcessorMode _mode;
public:
    // Construction/Destruction
    //! @brief Constructs a statement which updates the expected processor mode.
    //! @param[in] mode The new processor mode to set.
    UpdateProcessorModeStatement(ProcessorMode mode) :
        _mode(mode)
    {
    }

    virtual ~UpdateProcessorModeStatement() = default;

    // Overrides
    // Inherited from UpdateAssemblyStateStatement.
    virtual bool updateAssemblyState(AssemblyState &state) const override
    {
        bool isModified = false;

        if (state.getProcessorMode() != _mode)
        {
            state.setProcessorMode(_mode);
            isModified = true;
        }

        return isModified;
    }
};

//! @brief A statement which updates the address mode.
class UpdateAddresModeStatement : public UpdateAssemblyStateStatement
{
private:
    // Internal Fields
    AddressMode _mode;
public:
    // Construction/Destruction
    //! @brief Constructs a statement which updates the expected address mode.
    //! @param[in] mode The new processor mode to set.
    UpdateAddresModeStatement(AddressMode mode) :
        _mode(mode)
    {
    }

    virtual ~UpdateAddresModeStatement() = default;

    // Overrides
    // Inherited from UpdateAssemblyStateStatement.
    virtual bool updateAssemblyState(AssemblyState &state) const override
    {
        bool isModified = false;

        if (state.getAddressMode() != _mode)
        {
            state.setAddressMode(_mode);
            isModified = true;
        }

        return isModified;
    }
};


//! @brief A statement which aligns the object code address on a specific
//! address boundary.
class AlignStatement : public Statement
{
private:
    // Internal Fields
    uint32_t _value;
public:
    // Construction/Destruction
    //! @brief Constructs a statement which updates the expected address mode.
    //! @param[in] mode The new processor mode to set.
    AlignStatement(uint32_t value) :
        Statement(StatementType::DataDirective),
        _value(value)
    {
    }

    virtual ~AlignStatement() = default;

    // Overrides
    // Inherited from Statement.
    virtual uint32_t calculateObjectCodeSize(IEvalContext *context) const override
    {
        // Calculate how many bytes need to be written to align the assembly
        // address on the required address boundary.
        uint32_t currentAddr = context->getAssemblyAddress();

        int32_t pow2;

        if (Bin::bitScanReverse(_value, pow2) == false)
        {
            pow2 = 4;
        }

        uint32_t alignedAddr = Bin::roundUpPow2(currentAddr, pow2);

        return alignedAddr - currentAddr;
    }

    // Inherited from Statement.
    virtual bool assemble(const AssemblyState &/*state*/, IEvalContext *context,
                          ObjectCodeBuilder &output) const
    {
        uint32_t paddingBytes = calculateObjectCodeSize(context);

        // Pad with zeros up to the correct address boundary.
        output.writeZeros(paddingBytes);

        return true;
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
// AssemblyDirectiveNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a syntax node representing a directive which changes the
//! assembly state.
//! @param[in] context The current state of the parsing process.
//! @param[in] directive The first token of the directive.
AssemblyDirectiveNode::AssemblyDirectiveNode(ParseContext &context,
                                             const Token &directive) :
    StatementNode(context, directive),
    _type(directive.getProperty(TokenProperty::DirectiveType,
                                AssemblyDirectiveType::Include)),
    _instructionSet(directive.getProperty(TokenProperty::InstructionSet,
                                          InstructionSet::Max)),
    _extension(directive.getProperty(TokenProperty::ProcessorExtension,
                                     ArchExtensionEnum::None)),
    _processorMode(directive.getProperty(TokenProperty::ProcessorMode,
                                         ProcessorMode::Max)),
    _addressMode(directive.getProperty(TokenProperty::AddressMode,
                                       AddressMode::Bits26)),
    _isComplete(false)
{
}

// Inherited from ISyntaxNode.
bool AssemblyDirectiveNode::isComplete() const { return _isComplete; }

// Inherited from ISyntaxNode.
bool AssemblyDirectiveNode::isValid() const { return true; }

// Inherited from ISyntaxNode.
ISyntaxNode *AssemblyDirectiveNode::applyToken(ParseContext &/*context*/,
                                               const Token &token)
{
    ISyntaxNode *result = nullptr;

    if (_isComplete == false)
    {
        if (token.getClass() == TokenClass::StatementTerminator)
        {
            _isComplete = true;
            result = this;
        }
    }

    return result;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AssemblyDirectiveNode::applyNode(ParseContext &/*context*/,
                                              ISyntaxNode */*childNode*/)
{
    return nullptr;
}

// Inherited from ISyntaxNode.
void AssemblyDirectiveNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _isComplete = true;
    StatementNode::recover(context, node);
}

// Inherited from StatementNode.
Statement *AssemblyDirectiveNode::compile(Messages &/*output*/) const
{
    Statement *result = nullptr;

    switch (_type)
    {
    case AssemblyDirectiveType::InstructionSet:
        result = new UpdateInstructionSetStatement(_instructionSet);
        break;

    case AssemblyDirectiveType::ProcessorExtension:
        result = new EnableExtensionStatement(_extension);
        break;

    case AssemblyDirectiveType::ProcessorMode:
        result = new UpdateProcessorModeStatement(_processorMode);
        break;

    case AssemblyDirectiveType::AddressMode:
        result = new UpdateAddresModeStatement(_addressMode);
        break;

    case AssemblyDirectiveType::Include:
    default:
        // Nothing to do.
        break;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// AlignDirectiveNode Member Definitions
////////////////////////////////////////////////////////////////////////////////
AlignDirectiveNode::AlignDirectiveNode(ParseContext &context, const Token &directive) :
    StatementNode(context, directive),
    _oldLexerState(context.pushLexicalContext(getExpressionLexer())),
    _value(4),
    _state(State::AfterKeyword)
{
}

// Inherited from ISyntaxNode.
bool AlignDirectiveNode::isComplete() const { return _state == State::Complete; }

// Inherited from ISyntaxNode.
bool AlignDirectiveNode::isValid() const { return true; }

// Inherited from ISyntaxNode.
ISyntaxNode *AlignDirectiveNode::applyToken(ParseContext &context, const Token &token)
{
    ISyntaxNode *node = nullptr;

    switch (_state)
    {
    case State::AfterKeyword:
    case State::AfterValue:
        if (token.getClass() == TokenClass::StatementTerminator)
        {
            _state = State::Complete;
            context.restoreLexicalState(_oldLexerState);
            node = this;
        }
        break;

    }

    return node;
}

// Inherited from ISyntaxNode.
ISyntaxNode *AlignDirectiveNode::applyNode(ParseContext &context, ISyntaxNode *childNode)
{
    ISyntaxNode *result = nullptr;
    IntegerLiteralNode *valueNode;

    if ((_state == State::AfterKeyword) &&
        tryCast(childNode, valueNode))
    {
        IExprUPtr expr(valueNode->compile(ConstantSet::Empty));

        if (expr)
        {
            String error;
            Value alignObject;
            auto &log = context.getMessages();

            if (expr->tryConstantEvaluate(alignObject, error))
            {
                Value alignValue;

                if (alignObject.tryConvert(DataType::Uint32, alignValue))
                {
                    int32_t msb;

                    if ((Bin::bitScanReverse(alignValue.asUint32(), msb) == false) ||
                        (alignValue.asUint32() != (1u << msb)) ||
                        (alignValue.asUint32() < 2) ||
                        (alignValue.asUint32() > 0x10000))
                    {
                        log.appendError(getStart(),
                                        "Alignment values must be a"
                                        " whole power of 2 between 1 and 64K.");
                    }
                    else
                    {
                        _value = alignValue.asUint32();
                    }
                }
                else
                {
                    log.appendError(getStart(), "Alignment values must be a"
                                                " constant integer.");
                }
            }
            else
            {
                log.appendError(getStart(), "Alignment values must be a "
                                            "constant integer.");
            }
        }

        result = this;
        _state = State::Complete;
        context.restoreLexicalState(_oldLexerState);
    }

    return result;
}

// Inherited from ISyntaxNode.
void AlignDirectiveNode::recover(ParseContext &context, ISyntaxNode *node)
{
    _state = State::Complete;
    context.restoreLexicalState(_oldLexerState);

    StatementNode::recover(context, node);
}

// Inherited from StatmentNode.
Statement *AlignDirectiveNode::compile(Messages &/*output*/) const
{
    return new AlignStatement(_value);
}

}} // namespace Ag::Asm
////////////////////////////////////////////////////////////////////////////////

