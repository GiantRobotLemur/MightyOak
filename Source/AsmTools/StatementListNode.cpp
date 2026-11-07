//! @file StatementListNode.cpp
//! @brief The definition of an ISyntaxNode implementation which holds the
//! top level collection of statements.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2021-2026
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "Ag/Core/Utils.hpp"

#include "ExprContexts.hpp"
#include "IncludeStatement.hpp"
#include "LabelStatement.hpp"
#include "LexicalAnalysers.hpp"
#include "ParseContext.hpp"
#include "Parser.hpp"
#include "SimpleInstructionStatements.hpp"
#include "ProcStatements.hpp"
#include "StatementListNode.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
//! @brief An object representing a pre-assembled block of code.
class AssembledBlock : public StatementBlock
{
private:
    // Internal Fields
    std::vector<uint8_t> _objectCode;

public:
    //! @brief Constructs a block of pre-assembled object code.
    //! @param[in] startOffset The byte offset of the first byte of the block
    //! within the overall object code.
    AssembledBlock(uint32_t startOffset) :
        StatementBlock(startOffset, true)
    {
    }

    virtual ~AssembledBlock() = default;

    // Operations
    //! @brief Appends a block of object code to the block.
    //! @param[in] objectCode The object code to append.
    //! @throws Ag::ArgumentException Thrown if the start offset of objectCode
    //! is not the end offset of the current block.
    void append(const ObjectCodeBuilder &objectCode)
    {
        uint32_t endOffset = getAssemblyOffset() + getSize();

        if (objectCode.getInitialOffset() != endOffset)
        {
            // The new object code cannot be appended to the end of the
            // current block.
            throw Ag::ArgumentException("objectCode");
        }

        size_t sizeHint = _objectCode.size() + objectCode.getSize();

        if (_objectCode.capacity() < sizeHint)
        {
            _objectCode.reserve((sizeHint * 110) / 100);
        }

        // Copy the object code bytes into the block.
        std::copy(objectCode.getCode(),
                  objectCode.getCode() + objectCode.getSize(),
                  std::back_inserter(_objectCode));
    }

    // Overrides
    // Inherited from StatementBlock.
    virtual uint32_t getSize() const override
    {
        return static_cast<uint32_t>(_objectCode.size());
    }

    // Inherited from StatementBlock.
    virtual void appendObjectCode(ObjectCodeBuilder &objectCode) const override
    {
        if (objectCode.getCurrentOffset() != getAssemblyOffset())
        {
            // The objectCode object will not place the code we have at the
            // correct location.
            throw Ag::ArgumentException("objectCode");
        }

        // Simply copy the already assembled object code.
        objectCode.writeBytes(_objectCode.data(), _objectCode.size());
    }

    // Inherited from StatementBlock.
    virtual void finalise() override
    {
        // Prevent the block from being closed.
    }
};

using AssembledBlockUPtr = std::unique_ptr<AssembledBlock>;

//! @brief Holds a collection of assembly language statements which can only
//! produce object code once the entire source code has been parsed.
class DeferredBlock : public StatementBlock
{
private:
    // Internal Fields
    std::vector<StatementUPtr> _statements;
    AssemblyStateSPtr _state;
    IScopedContextSPtr _scope;
    uint32_t _assembledSize;

public:
    //! @brief Creates a block of statements which require deferred processing
    //! in order to produce object code.
    //! @param[in] state The state of the assembler when the statement was
    //! encountered.
    //! @param[in] scope The scope in which the deferred statements should be
    //! evaluated.
    //! @param[in] startOffset The offset of the first bytes of object code of
    //! the first statement when it is finally assembled.
    DeferredBlock(const AssemblyStateSPtr &state,
                  const IScopedContextSPtr &scope,
                  uint32_t startOffset) :
        StatementBlock(startOffset, false),
        _state(state),
        _scope(scope),
        _assembledSize(0)
    {
    }

    virtual ~DeferredBlock() = default;

    // Accessors

    //! @brief Determines whether an assembly state and scope matches that of
    //! the current block.
    //! @param[in] state The assembly state to match.
    //! @param[in] scope The assembly scope to match.
    //! @retval true The state and scope of the block match the parameters.
    //! @retval false Either of the input parameters don't match the block.
    bool isMatching(const AssemblyStateSPtr &state,
                    const IScopedContextSPtr &scope) const
    {
        return (state == _state) && (scope == _scope);
    }

    // Operations
    //! @brief Takes ownership of an assembly language statement which requires
    //! deferred processing.
    //! @param[in] statement The statement to take ownership of.
    //! @param[in] assembledSize The predicted size of the object code the
    //! statement will eventually produce.
    void appendStatement(StatementUPtr &&statement, uint32_t assembledSize)
    {
        if (isClosed())
        {
            throw Ag::OperationException("Appending statements to a closed block.");
        }
        else
        {
            _statements.push_back(std::move(statement));
            _assembledSize += assembledSize;
        }
    }

    // Overrides
    // Inherited from StatementBlock.
    virtual uint32_t getSize() const override
    {
        return _assembledSize;
    }

    // Inherited from StatementBlock.
    virtual void appendObjectCode(ObjectCodeBuilder &objectCode) const override
    {
        if (objectCode.getCurrentOffset() != getAssemblyOffset())
        {
            // The objectCode object will not place the code we have at the
            // correct location.
            throw Ag::ArgumentException("objectCode");
        }

        // Process the deferred statements to produce object code.
        IScopedContext *context = _scope.get();
        AssemblyState *state = _state.get();
        bool allowStuffing = false;

        for (const auto &statementPtr : _statements)
        {
            // Update the current assembly offset before deferred evaluation.
            _scope->setAssemblyOffset(objectCode.getCurrentOffset());

            if (statementPtr->assemble(*state, context, objectCode) == false)
            {
                // Assume the statement issued a message due to being unable
                // to produce object code on the final pass.
                allowStuffing = true;
            }
        }

        // Ensure the object code is stuffed to compensate for
        // the missing object code.
        if (objectCode.getCurrentOffset() < getEndOffset())
        {
            if (allowStuffing)
            {
                uint32_t padding = getEndOffset() - objectCode.getCurrentOffset();
                objectCode.writeZeros(padding);
            }
            else
            {
                throw Ag::OperationException("One or more deferred assembly "
                                             "statements did not produce the "
                                             "amount of object code expected.");
            }
        }
    }
};

using DeferredBlockUPtr = std::unique_ptr<DeferredBlock>;

////////////////////////////////////////////////////////////////////////////////
// EmptyStatement Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs an object representing an empty assembly language statement.
//! @param[in] context The current state of the parsing process.
//! @param[in] position The position in source code of the beginning/end of the
//! empty statement.
EmptyStatement::EmptyStatement(ParseContext &context, const Location &position) :
    StatementNode(context, position)
{
}

// Inherited from ISyntaxNode.
bool EmptyStatement::isComplete() const { return true; }

// Inherited from ISyntaxNode.
bool EmptyStatement::isValid() const { return true; }

// Inherited from ISyntaxNode.
ISyntaxNode *EmptyStatement::applyToken(ParseContext & /* context */,
                                        const Token & /* token */)
{
    // The node cannot be extended.
    return nullptr;
}

// Inherited from ISyntaxNode.
ISyntaxNode *EmptyStatement::applyNode(ParseContext & /* context */,
                                       ISyntaxNode * /* childNode */)
{
    // The node cannot be extended.
    return nullptr;
}

// Inherited from ISyntaxNode.
void EmptyStatement::recover(ParseContext & /* context */,
                             ISyntaxNode *node)
{
    Ag::safeDelete(node);
}

// Inherited from StatementNode.
Statement *EmptyStatement::compile(Messages & /* output */) const
{
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// StatementBlock Member Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Constructs a base class for a block of statements.
//! @param[in] offset The offset from the beginning of the object code at
//! which the block appears.
//! @param[in] isPreAssembled Indicates whether the block contains object
//! code which is already assembled.
StatementBlock::StatementBlock(uint32_t offset, bool isPreAssembled) :
    _offset(offset),
    _isPreAssembled(isPreAssembled),
    _isClosed(false)
{
}

//! @brief Determines if the block can no longer receive updates.
//! @retval true The block cannot have more appended to it.
//! @retval false The block can have more appended to it.
bool StatementBlock::isClosed() const
{
    return _isClosed;
}

//! @brief Determines whether the object represents pre-assembled object
//! code or statements stored for deferred processing.
//! @retval true The object contains binary object code.
//! @retval false The object contains statements which require deferred
//! processing.
bool StatementBlock::isPreAssembled() const { return _isPreAssembled; }

//! @brief Gets the offset from the beginning of the object code at
//! which the block appears.
uint32_t StatementBlock::getAssemblyOffset() const { return _offset; }

//! @brief Gets the offset of the next byte of object code appearing after
//! the current block.
uint32_t StatementBlock::getEndOffset() const { return _offset + getSize(); }

//! @brief Marks the block as closed.
void StatementBlock::finalise()
{
    _isClosed = true;
}

////////////////////////////////////////////////////////////////////////////////
// StatementListNode Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
//! @brief Creates a root parse node which holds all completed assembly language
//! statements.
//! @param[in] initialState The initial state of the assembler when parsing of
//! the first statement begins.
//! @param[in] scope The variable evaluation stop at the top level.
//! @param[in] baseAddress The load address of the object code.
//! @param[in] assemblyOffset An offset of the first byte from the beginning of
//! object code, produced by the current object.
StatementListNode::StatementListNode(const AssemblyStateSPtr &initialState,
                                     const IScopedContextSPtr &scope,
                                     uint32_t baseAddress,
                                     uint32_t assemblyOffset /*= 0*/) :
    _currentState(initialState),
    _baseAddress(baseAddress),
    _initialAssemblyOffset(assemblyOffset)
{
    if (scope)
    {
        _scopeStack.push_back(scope);
    }
}

//! @brief Assembles the blocks of statements into their final object code.
//! @param[in] messages The collection of assembly messages to append to
//! if errors or warnings occur.
ObjectCode StatementListNode::assemble(Messages &messages) const
{
    // Calculate the final size of the object code.
    size_t predictedSize = 0;

    for (const auto &block : _blocks)
    {
        predictedSize += block->getSize();
    }

    ObjectCodeBuilder builder(messages, _baseAddress, 0);
    builder.reserve(predictedSize);
    builder.beginFinalPass();

    // Append the contents of each block to the builder, merely copying some
    // and actually assembling others.
    for (const auto &block : _blocks)
    {
        block->appendObjectCode(builder);
    }

    // Now that all messages have been logged, sort them.
    messages.sort();

    if (_scopeStack.empty() == false)
    {
        const RootEvalContext *rootContext =
            dynamic_cast<const RootEvalContext *>(_scopeStack.front().get());

        if (rootContext != nullptr)
        {
            // Annotate the object code with symbols, if we can.
            return builder.createObjectCode(rootContext->getSymbols());
        }
    }

    return builder.createObjectCode();
}

// Inherited from ISyntaxNode.
bool StatementListNode::isComplete() const
{
    return true;
}

// Inherited from ISyntaxNode.
bool StatementListNode::isValid() const
{
    return true;
}

// Inherited from ISyntaxNode.
ISyntaxNode *StatementListNode::applyToken(ParseContext & /* context */,
                                           const Token &token)
{
    return (token.getClass() == TokenClass::StatementTerminator) ? this : nullptr;
}

// Inherited from ISyntaxNode.
ISyntaxNode *StatementListNode::applyNode(ParseContext &context,
                                          ISyntaxNode *childNode)
{
    // We'll claim the node rightly or wrongly because this is the root of the
    // syntax tree and there is nowhere else to go. Ensure it is deleted.
    std::unique_ptr<ISyntaxNode> deleteNode(childNode);

    StatementNode *statementNode = nullptr;
    ISyntaxNode *result = this;

    if (Ag::tryCast(childNode, statementNode))
    {
        if (statementNode->isComplete() && statementNode->isValid())
        {
            processStatementNode(context, statementNode);
        }
    }

    return result;
}

// Inherited from ISyntaxNode.
void StatementListNode::recover(ParseContext & /* context */, ISyntaxNode *node)
{
    Ag::safeDelete(node);
}

//! @brief Calculates the offset of the next byte of object code after the last
//! one assembled.
uint32_t StatementListNode::getAssemblyOffset() const
{
    uint32_t offset = 0;

    if (_blocks.empty() == false)
    {
        offset = _blocks.back()->getEndOffset();
    }

    return offset;
}

//! @brief Calculates the address of the next byte of object code after the last
//! one assembled given the expected load address of the code.
uint32_t StatementListNode::getAssemblyAddress() const
{
    uint32_t offset = 0;

    if (_blocks.empty() == false)
    {
        offset = _blocks.back()->getEndOffset();
    }

    return offset + _baseAddress;
}

//! @brief Processes statement syntax node which is complete and valid.
//! @param[in] context The state of the current parsing process.
//! @param[in] parsedStatment A pointer to the syntax node which represent the
//! fully parsed and valid statement.
void StatementListNode::processStatementNode(ParseContext &context,
                                             StatementNode *parsedStatment)
{
    // Compile the syntax tree into a statement object.
    StatementUPtr statement(parsedStatment->compile(context.getMessages()));

    if (statement)
    {
        switch (statement->getType())
        {
        case StatementType::Empty:
            // Allow the statement to be automatically deleted rather than
            // added to the statement list.
            break;

        case StatementType::PreAssembledData:
            // TODO: Append the data to the next AssembledBlock.
            break;

        case StatementType::DataDirective:
        case StatementType::Instruction: {
            // The statement will produce object code, either now or once
            // the parsing process is complete.
            ObjectCodeBuilder builder(context.getMessages(),
                                      _baseAddress,
                                      getAssemblyOffset());

            // Ensure a symbol context is set to the current assembly position
            // before attempting to assemble the instruction or directive.
            IScopedContext *currentContext = getScope();
            currentContext->setAssemblyOffset(getAssemblyOffset());

            if (statement->assemble(*_currentState, currentContext, builder))
            {
                // The code was successfully assembled on the first pass.
                appendObjectCode(builder);
            }
            else
            {
                // Assembly needs to be deferred, keep the statement for
                // later processing.
                uint32_t predictedSize = statement->calculateObjectCodeSize(currentContext);

                deferAssembly(std::move(statement), predictedSize);
            }
        } break;

        case StatementType::AssemblyDirective: {
            // Clone the current state.
            AssemblyStateSPtr updatedState = std::make_shared<AssemblyState>(*_currentState);

            if (statement->updateAssemblyState(*updatedState))
            {
                // The state has been modified.
                if (_blocks.empty() == false)
                {
                    // Prevent further state-dependent instructions from being added
                    // to the current block, a new one will be created using the
                    // new state.
                    _blocks.back()->finalise();
                }

                // Update the state used by new blocks.
                _currentState = updatedState;
            }
        } break;

        case StatementType::Label:
            processLabel(context.getMessages(), std::move(statement));
            break;

        case StatementType::Include: {
            // Parse the new input source and append the statement blocks it
            // produces to the current collection.
            IncludeStatement *include = nullptr;

            if ((context.getInputSource() != nullptr) &&
                Ag::tryCast(statement.get(), include))
            {
                // Parse the specified file.
                processIncludedFile(context,
                                    include->getFileReference(),
                                    include->getLocation());
            }
        } break;

        case StatementType::MacroInvocation:
            // TODO: Look-up the macro name and re-play its captured
            // statement syntax nodes in the current context to produce
            // statements for assembly.

        case StatementType::MacroStart:
            // TODO: Begin recording of a named macro.

        case StatementType::MacroEnd:
            // TODO: End recording of a named macro.

        case StatementType::ProcedureStart:
            processSubroutineStart(context.getMessages(), statement.get());
            break;

        case StatementType::ProcedureEnd:
            processSubroutineEnd(context.getMessages(), statement.get());
            break;

        default:
            // Allow the statement to be silently disposed of.
            break;
        }
    }
}

//! @brief Parses an included input source and adds the resultant statements to
//! the internal list.
//! @param[in] parentContext The state of the parser in which the include
//! directive was encountered.
//! @param[in] inputSourceId The identifier of the new input source as
//! referenced in the source code.
//! @param[in] includedFrom The location of the include directive in the
//! source code being parsed.
void StatementListNode::processIncludedFile(ParseContext &parentContext,
                                            Ag::string_cref_t inputSourceId,
                                            const Location &includedFrom)
{
    // Parse the specified file.
    IInputSet *currentInput = parentContext.getInputSource();
    Messages &message = parentContext.getMessages();

    IInputSetUPtr includedInput;
    Ag::String errorOrId;

    if ((currentInput != nullptr) &&
        currentInput->tryGetInputSource(inputSourceId,
                                        includedInput,
                                        errorOrId))
    {
        // Ensure we don't parse an input source multiple times or
        // end up in a recursive loop.
        auto &visitedSources = parentContext.getVisitedSources();

        if (visitedSources.find(errorOrId) == visitedSources.end())
        {
            // Create a context to parse the included file.
            Ag::String error;
            ParseContext nestedContext(visitedSources,
                                       std::move(includedInput),
                                       message);

            if (nestedContext.tryBegin(getStatementLexer(), error))
            {
                StatementListNode includedStatements(_currentState,
                                                     _scopeStack.back(),
                                                     _baseAddress,
                                                     getAssemblyOffset());
                parseSourceCode(nestedContext, &includedStatements);

                // Ensure we don't have to keep re-allocating the vector when
                // appending the included statement blocks.
                size_t sizeHint = includedStatements._blocks.size();
                sizeHint += _blocks.size();

                if (sizeHint > _blocks.size())
                {
                    _blocks.reserve((sizeHint * 110) / 100);
                }

                std::move(includedStatements._blocks.begin(),
                          includedStatements._blocks.end(),
                          std::back_inserter(_blocks));
            }
            else
            {
                // Failed to open included file.
                message.appendError(includedFrom, error);
            }
        }
    }
    else
    {
        // Pass the error message along.
        message.appendError(includedFrom, errorOrId);
    }
}

//! @brief Processes the appearance of an assembly label statement.
//! @param[in] messages The collection to add errors and warnings to.
//! @param[in] statement The statement being processed.
void StatementListNode::processLabel(Messages &messages, StatementUPtr &&statement)
{
    // Add the label definition to the current context.
    LabelStatement *label = static_cast<LabelStatement *>(statement.get());
    IScopedContext *currentContext = getScope();
    Ag::String existingScope;
    Location existingLocation;

    // Determine if the label is already defined.
    if (currentContext->isSymbolDefined(label->getID(),
                                        existingScope,
                                        existingLocation) &&
        (currentContext->getScopeName() == existingScope))
    {
        Ag::String message =
            Ag::String::format("Symbol '{0}' was already defined in the "
                               "current scope at {1}({2})",
                               { label->getID(),
                                 existingLocation.FileName,
                                 existingLocation.LineNo });

        messages.appendError(label->getSourcePosition(), message);
    }
    else
    {
        if (label->getValueExpr() == nullptr)
        {
            // Tag the current assembly position with the label.
            currentContext->defineSymbol(label->getID(),
                                         label->getSourcePosition(),
                                         Value(getAssemblyAddress()),
                                         true);
        }
        else
        {
            Ag::String error;
            Value result;

            if (label->getValueExpr()->tryEvaluate(currentContext,
                                                   result,
                                                   error))
            {
                // Define the symbol with an explicit value.
                currentContext->defineSymbol(label->getID(),
                                             label->getSourcePosition(),
                                             result, false);
            }
            else
            {
                // Perhaps we could evaluate the expression on the
                // final pass.
                deferAssembly(std::move(statement), 0);
            }
        }
    }
}

//! @brief Processes the statement marking the beginning of a subroutine.
//! @param[in] messages A collection of messages which can be appended to.
//! @param[in] parsedStatement The statement marking the start of the subroutine.
void StatementListNode::processSubroutineStart(Messages &messages,
                                               Statement *parsedStatement)
{
    ProcStatement *statement;

    if (!Ag::tryCast(parsedStatement, statement))
        return;

    IScopedContext *currentSymbolScope = getScope();
    Value existingValue;

    if (currentSymbolScope->tryLookupSymbol(statement->getId(), existingValue))
    {
        std::string message;
        message.assign("The subroutine cannot be defined as a symbol '");
        Ag::appendAgString(message, statement->getId());
        message.append("' already exists.");

        messages.appendError(statement->getLocation(), message);
        return;
    }

    AssemblyStateSPtr updatedState = std::make_shared<AssemblyState>(*_currentState);

    if (updatedState->tryBeginSubroutine(messages, statement->getLocation(),
                                         statement->getId(), _currentState))
    {
        // Update the assembly state to indicate that we have started defining
        // a subroutine.
        _currentState = updatedState;

        // Define a symbol for the subroutine visible in the current scope.
        currentSymbolScope->defineSymbol(statement->getId(), statement->getLocation(),
                                         Value(getAssemblyAddress()),
                                         true);

        // Create a scope for the contents of the subroutine.
        IScopedContextSPtr procScope = std::make_shared<InnerEvalContext>(currentSymbolScope,
                                                                          statement->getId());
        procScope->setAssemblyOffset(getAssemblyOffset());
        _scopeStack.push_back(procScope);
    }
}

//! @brief Processes the statement marking the end of a subroutine.
//! @param[in] messages A collection of messages which can be appended to.
//! @param[in] parsedStatement The statement marking the end of the subroutine.
void StatementListNode::processSubroutineEnd(Messages &messages,
                                             Statement *parsedStatement)
{
    EndProcStatement *statement;

    if (!Ag::tryCast(parsedStatement, statement))
        return;

    // Stop recording a procedure.
    if (_currentState->tryEndSubroutine(messages, statement->getLocation()))
    {
        // Remove the symbol scope from the stack.
        auto subroutineScope = _scopeStack.back();
        _scopeStack.pop_back();

        // Promote the subroutine symbols into the parent scope, suitably
        // prefixed to make them inaccessible to code.
        IScopedContext *parentContext = _scopeStack.back().get();
        parentContext->promoteSymbols(subroutineScope->getScopeName(),
                                      subroutineScope->getSymbols());

        // Restore the assembly state to that which it was when the subroutine
        // definition started.
        _currentState = _currentState->getParentState();
    }
}

//! @brief Append a block of pre-assembled source code to the object code stream.
//! @param[in] objectCode The assembled object code.
void StatementListNode::appendObjectCode(const ObjectCodeBuilder &objectCode)
{
    if (objectCode.isEmpty() == false)
    {
        if (_blocks.empty() ||
            _blocks.back()->isClosed() ||
            (_blocks.back()->isPreAssembled() == false))
        {
            // Create a new pre-assembled block to add the object code to.
            _blocks.push_back(std::make_unique<AssembledBlock>(objectCode.getInitialOffset()));
        }

        // Get the last block in the collection.
        AssembledBlock *block = static_cast<AssembledBlock *>(_blocks.back().get());

        // Append the new object code.
        block->append(objectCode);
    }
}

//! @brief Stores a statement which cannot be assembled yet.
//! @param[in] statement The statement to take ownership of.
//! @param[in] predictedSize The count of object code bytes the statement is
//! predicted to produce.
void StatementListNode::deferAssembly(StatementUPtr &&statement,
                                      uint32_t predictedSize)
{
    if (statement)
    {
        DeferredBlock *lastBlock;
        bool startNewDeferredBlock = false;

        if (_blocks.empty() || _blocks.back()->isPreAssembled() ||
            _blocks.back()->isClosed() ||
            (Ag::tryCast(_blocks.back().get(), lastBlock) == false))
        {
            startNewDeferredBlock = true;
        }
        else
        {
            // Ensure the statement to defer is being assembled in the same
            // environment as previously deferred statements.
            startNewDeferredBlock = lastBlock->isMatching(_currentState,
                                                          _scopeStack.back());
        }

        if (startNewDeferredBlock)
        {
            // Create a new deferred statement block and add it to the end
            // of the collection.
            _blocks.push_back(std::make_unique<DeferredBlock>(_currentState,
                                                              _scopeStack.back(),
                                                              getAssemblyOffset()));
        }

        DeferredBlock *block = static_cast<DeferredBlock *>(_blocks.back().get());

        block->appendStatement(std::move(statement), predictedSize);
    }
}

//! @brief Gets the variable scope at the top of the scope stack.
IScopedContext *StatementListNode::getScope()
{
    if (_scopeStack.empty() || !_scopeStack.back())
    {
        throw Ag::OperationException("Attempted to get the current assembly "
                                     "scope when the stack was empty.");
    }

    return _scopeStack.back().get();
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////

