# CMakeLists configuration for the ARM Assembler Tools library

# Define the library.
add_ag_library(AsmTools FOLDER      ARM
                        SOURCES     AsmTools.cpp
                                    AsmEnums.hpp
                                    ${MO_INCLUDE_DIR}/AsmTools/Options.hpp
                                    Options.cpp
                                    ${MO_INCLUDE_DIR}/AsmTools/Messages.hpp
                                    Messages.cpp
                                    ${MO_INCLUDE_DIR}/AsmTools/ObjectCode.hpp
                                    ObjectCode.cpp
                                    ${MO_INCLUDE_DIR}/AsmTools/InstructionInfo.hpp
                                    InstructionInfo.cpp
                                    Disassembly.hpp
                                    Disassembly.cpp
                                    Assembly.hpp
                                    Assembly.cpp
                                    FormatInstruction.hpp
                                    FormatInstruction.cpp
                                    InputSource.hpp
                                    InputSource.cpp
                                    InputSet.hpp
                                    InputSet.cpp
                                    Token.hpp
                                    Token.cpp
                                    InputContext.hpp
                                    InputContext.cpp
                                    SyntaxNode.hpp
                                    LexicalContext.hpp
                                    ParseContext.hpp
                                    ParseContext.cpp
                                    LexicalAnalysers.hpp
                                    LexicalAnalysers.cpp
                                    Parser.hpp
                                    Parser.cpp
                                    ExpressionNode.hpp
                                    ExpressionNode.cpp
                                    Value.cpp
                                    Value.hpp
                                    ConstantSet.cpp
                                    ConstantSet.hpp
                                    SymbolTable.cpp
                                    SymbolTable.hpp
                                    Expr.cpp
                                    Expr.hpp
                                    ExprContexts.cpp
                                    ExprContexts.hpp
                                    BaseSyntaxNode.cpp
                                    BaseSyntaxNode.hpp
                                    AssemblyState.cpp
                                    AssemblyState.hpp
                                    ObjectCodeBuilder.cpp
                                    ObjectCodeBuilder.hpp
                                    Statement.cpp
                                    Statement.hpp
                                    StatementListNode.hpp
                                    StatementListNode.cpp
                                    LabelStatement.hpp
                                    LabelStatement.cpp
                                    DataDirectiveStatement.hpp
                                    DataDirectiveStatement.cpp
                                    AssemblyDirectiveStatement.hpp
                                    AssemblyDirectiveStatement.cpp
                                    IncludeStatement.hpp
                                    IncludeStatement.cpp
                                    InstructionStatement.hpp
                                    InstructionStatement.cpp
                                    SimpleInstructionStatements.hpp
                                    SimpleInstructionStatements.cpp
                                    AluInstructionNode.hpp
                                    AluInstructionNode.cpp
                                    ShifterOperandNode.hpp
                                    ShifterOperandNode.cpp
                                    AddressOperandNode.hpp
                                    AddressOperandNode.cpp
                                    DataTransferInstructionNode.hpp
                                    DataTransferInstructionNode.cpp
                                    RegisterListNode.hpp
                                    RegisterListNode.cpp
                                    MultiTransferInstructionNode.hpp
                                    MultiTransferInstructionNode.cpp
                                    AddressDirectiveNode.hpp
                                    AddressDirectiveNode.cpp
                                    AtomicSwapInstructionNode.hpp
                                    AtomicSwapInstructionNode.cpp
                                    CoProcInstructionStatements.cpp
                                    CoProcInstructionStatements.hpp
                        HEADERS     ${MO_INCLUDE_DIR}/AsmTools.hpp
                        PUBLIC_LIBS AgCore)

source_group(Assembly FILES
    AsmTools.cpp
    ${MO_INCLUDE_DIR}/AsmTools.hpp
    AsmEnums.hpp
    ${MO_INCLUDE_DIR}/AsmTools/Options.hpp
    Options.cpp
    ${MO_INCLUDE_DIR}/AsmTools/Messages.hpp
    Messages.cpp
    ${MO_INCLUDE_DIR}/AsmTools/ObjectCode.hpp
    ObjectCode.cpp
    ObjectCodeBuilder.cpp
    ObjectCodeBuilder.hpp
    ${MO_INCLUDE_DIR}/AsmTools/InstructionInfo.hpp
    InstructionInfo.cpp
    Disassembly.hpp
    Disassembly.cpp
    Assembly.hpp
    Assembly.cpp
    FormatInstruction.hpp
    FormatInstruction.cpp
)

source_group("Parser" FILES
    InputSource.hpp
    InputSource.cpp
    InputSet.hpp
    InputSet.cpp
    Token.hpp
    Token.cpp
    InputContext.hpp
    InputContext.cpp
    SyntaxNode.hpp
    LexicalContext.hpp
    ParseContext.hpp
    ParseContext.cpp
    LexicalAnalysers.hpp
    LexicalAnalysers.cpp
    Parser.hpp
    Parser.cpp)

source_group("Expressions" FILES
    ExpressionNode.hpp
    ExpressionNode.cpp
    Value.cpp
    Value.hpp
    ConstantSet.cpp
    ConstantSet.hpp
    SymbolTable.cpp
    SymbolTable.hpp
    Expr.cpp
    Expr.hpp
    ExprContexts.cpp
    ExprContexts.hpp
)

source_group("Syntax Tree" FILES
    BaseSyntaxNode.cpp
    BaseSyntaxNode.hpp
    AssemblyState.cpp
    AssemblyState.hpp
    Statement.cpp
    Statement.hpp
    StatementListNode.hpp
    StatementListNode.cpp
    LabelStatement.hpp
    LabelStatement.cpp
    IncludeStatement.hpp
    IncludeStatement.cpp
    DataDirectiveStatement.hpp
    DataDirectiveStatement.cpp
    AssemblyDirectiveStatement.hpp
    AssemblyDirectiveStatement.cpp
)


source_group("Instructions" FILES
    InstructionStatement.hpp
    InstructionStatement.cpp
    SimpleInstructionStatements.hpp
    SimpleInstructionStatements.cpp
    AluInstructionNode.hpp
    AluInstructionNode.cpp
    ShifterOperandNode.hpp
    ShifterOperandNode.cpp
    AddressOperandNode.hpp
    AddressOperandNode.cpp
    DataTransferInstructionNode.hpp
    DataTransferInstructionNode.cpp
    RegisterListNode.hpp
    RegisterListNode.cpp
    MultiTransferInstructionNode.hpp
    MultiTransferInstructionNode.cpp
    AddressDirectiveNode.hpp
    AddressDirectiveNode.cpp
    AtomicSwapInstructionNode.hpp
    AtomicSwapInstructionNode.cpp
    CoProcInstructionStatements.cpp
    CoProcInstructionStatements.hpp
)

# Define the unit test harness.
add_ag_test_app(AsmTools_Tests TEST_LIB AsmTools
                               SOURCES  Test_Main.cpp
                                        TestTools.cpp
                                        TestTools.hpp
                                        Test_Token.cpp
                                        Test_InputSource.cpp
                                        Test_InputContext.cpp
                                        Test_StatementLexer.cpp
                                        Test_ExpressionLexer.cpp
                                        Test_CoreRegListLexer.cpp
                                        Test_PsrLexer.cpp
                                        Test_ExpressionParser.cpp
                                        Test_Value.cpp
                                        Test_SymbolTable.cpp
                                        Test_Expr.cpp
                                        Test_ExprContexts.cpp
                                        Test_DataDirective.cpp
                                        Test_AssemblyLabel.cpp
                                        Test_InstructionInfo.cpp
                                        Test_SimpleInstructions.cpp)

add_cli_app(AAsm FOLDER ARM
                 NAME AAsm
                 DESCRIPTION "A 32-bit ARM assembler."
                 VERSION "${PROJECT_VERSION}"
                 SOURCES AAsm_Main.cpp
                 LIBS AgCore AsmTools)

add_cli_app(ADisasm FOLDER ARM
                    NAME ADisasm
                    DESCRIPTION "A 32-bit ARM disassembler."
                    VERSION "${PROJECT_VERSION}"
                    SOURCES ADisasm_Main.cpp
                    LIBS AgCore AsmTools)

# add_gui_app(target ...)
# Arguments: target                  - The mandatory target name
#            FOLDER <folder name>    - The name of the project folder to add the item to.
#            NAME <app name>         - The base name of the application program file.
#            DESCRIPTION <desc>      - A description of the application's purpose.
#            VERSION <version>       - The version to embed in the application.
#            SOURCES <files>         - Internal source and header files.
#            WIN_SOURCES <files>     - Win32-specific source and header files.
#            POSIX_SOURCES <files>   - Linux-specific source and header files.
#            LIBS                    - The libraries the application should link to.