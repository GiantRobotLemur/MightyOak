//! @file AsmTools/InstructionInfo.hpp
//! @brief The declaration of data structures which describe any assembly
//! language instruction.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ASM_TOOLS_INSTRUCTION_INFO_HPP__
#define __ASM_TOOLS_INSTRUCTION_INFO_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include "Ag/Core/String.hpp"

namespace Mo {
namespace Asm {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Identifies a core register.
enum class CoreRegister : uint8_t
{
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    CPSR, // ARMv3+
    SPSR, // ARMv3+
};

//! @brief Expresses a register in the Floating Point Accelerator co-processor.
enum class FpaRegister : uint8_t
{
    F0 = 0,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7
};

//! @brief Expresses the identifier of a co-processor.
enum class CoProcId : uint8_t
{
    CP0 = 0,
    CP1,
    CP2,
    CP3,
    CP4,
    CP5,
    CP6,
    CP7,
    CP8,
    CP9,
    CP10,
    CP11,
    CP12,
    CP13,
    CP14,
    CP15
};

//! @brief Expresses native co-processor register identifiers.
enum class CoProcRegister : uint8_t
{
    CR0 = 0,
    CR1,
    CR2,
    CR3,
    CR4,
    CR5,
    CR6,
    CR7,
    CR8,
    CR9,
    CR10,
    CR11,
    CR12,
    CR13,
    CR14,
    CR15
};

//! @brief Expresses the condition code used to annotate an instruction.
enum class ConditionCode : uint8_t
{
    Eq = 0,
    Ne,
    Cs, // Shares ordinal with HS.
    Cc, // Shares ordinal with LO.
    Mi,
    Pl,
    Vs,
    Vc,
    Hi,
    Ls,
    Ge,
    Lt,
    Gt,
    Le,
    Al,
    Nv,
    Hs, // Does not correspond with the actual condition code value (2).
    Lo, // Does not correspond with the actual condition code value (3).

    Max, // Only used for bounds checking.
};

//! @brief Expresses the different sets of parameters which can be consumed
//! by the barrel shifter operand.
enum class ShifterMode : uint8_t
{
    ImmediateConstant,
    Register,
    ShiftByRegister,
    ShiftByConstant,
    RotateWithExtend, // RRX
};

//! @brief Expresses the type of shift implemented.
enum class ShiftType : uint8_t
{
    Lsl = 0, // As they are encoded
    Lsr,
    Asr,
    Ror,
    Rrx, // Encoded as Reg shift ROR #0

    None, // Makes the type nullable.
};

//! @brief Expresses the mnemonic identifier of an instruction token.
enum class InstructionMnemonic : uint8_t
{
    And = 0, // Core ALU ordinals ordered as encoded.
    Eor,
    Sub,
    Rsb,
    Add,
    Adc,
    Sbc,
    Rsc,
    Tst,
    Teq,
    Cmp,
    Cmn,
    Orr,
    Mov,
    Bic,
    Mvn,

    Mul,
    Mla,

    B,
    Bl,
    Bx,

    Ldr,
    Str,

    Ldm,
    Stm,

    Swi,
    Swp,

    // Co-processor instructions
    Mcr,
    Mrc,
    Cdp,
    Ldc,
    Stc,

    // ARMv3+
    Mrs,
    Msr,

    // ARMv4+ long multiply
    Umull,
    Umlal,
    Smull,
    Smlal,

    // ARMv5+ but useful for debugging/testing on earlier models.
    Bkpt,

    // FPA co-processor instructions.
    Ldf,
    Stf,
    Lfm,
    Sfm,

    Adf, // Ordered as encoded, this mnemonic is dyadic FPA op #0.
    Muf,
    Suf,
    Rsf,
    Dvf,
    Rdf,
    Pow,
    Rpw,
    Rmf,
    Fml,
    Fdv,
    Frd,
    Pol,
    FpaUndefined_01,
    FpaUndefined_02,
    FpaUndefined_03,

    Mvf, // Ordered as encoded, this mnemonic is monadic FPA op #0.
    Mnf,
    Abs,
    Rnd,
    Sqt,
    Log,
    Lgn,
    Exp,
    Sin,
    Cos,
    Tan,
    Asn,
    Acs,
    Atn,
    Urd,
    Nrm,

    Flt, // Ordered as encoded for FPA reg transfer op #1
    Fix,
    Wfs,
    Rfs,
    Wfc,
    Rfc,

    Cmf, // Ordered as encoded for FPA reg comparison op #4
    Cmfe,
    Cnf,
    Cnfe,

    // Pseudo instructions and directives.
    Adr,
    Align,

    MaxMnemonic,
};

//! @brief Expresses the components of the ARMv3+ PSR register.
//! @note This is a bit-field enumeration, values fit the pattern they would
//! be encoded as in an instruction bit-field.
enum class PsrComponent : uint8_t
{
    None = 0x00,
    Control = 0x01,
    Extension = 0x02,
    Status = 0x04,
    Flags = 0x08,
    All = 0x0F,
};

//! @brief Describe the data type of a value moved.
enum class TransferDataType : uint8_t
{
    Word,
    SignedByte,
    UnsignedByte,
    SignedHalfWord,
    UnsignedHalfWord,
};

//! @brief Expresses the addressing mode of a multi-register
//! data transfer operation.
enum class MultiTransferMode : uint8_t
{
    EmptyAscending,
    EmptyDescending,
    FullAscending,
    FullDescending,
    IncrementAfter,
    IncrementBefore,
    DecrementAfter,
    DecrementBefore,

    Max,
};

//! @brief Defines how many instruction words should be used to encode a
//! pseudo-instruction.
enum class MultiWordEncoding : uint8_t
{
    //! @brief The encoding will be a single 32-bit word (default).
    Single,

    //! @brief The encoding will be a pair of 32-bit words.
    Long,

    //! @brief The encoding will be three 32-bit words.
    Extended,
};

//! @brief Expresses the precision of FPA data operations.
enum class FpaPrecision : uint8_t
{
    Single,
    Double,
    Extended,
    Packed,
};

//! @brief Expresses the rounding mode of FPA data operations.
enum class FpaRoundMode : uint8_t
{
    Nearest,
    Plus,
    Minus,
    Zero,
};

//! @brief Identifies classes of instructions grouped in terms of their
//! descriptions.
enum class OperationClass : uint8_t
{
    None,

    CoreAlu,
    CoreCompare,
    CoreAddress,
    CoreMultiply,
    CoreDataTransfer,
    CoreMultiTransfer,
    Branch,
    SoftwareIrq,
    Breakpoint,
    AtomicSwap,

    MoveFromPSR,
    MoveToPSR,

    BranchExchange,
    LongMultiply,

    CoProcDataTransfer,
    CoProcRegisterTransfer,
    CoProcDataProcessing,

    FpaDataTransfer,
    FpaMultiTransfer,
    FpaDyadicOperation,
    FpaMonadicOperation,
    FpaRegisterTransfer,
    FpaComparison,

    Max,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Parameters for the barrel shifter operand to ALU and data transfer
//! instructions.
struct ShifterOperand
{
    ShifterMode Mode;
    ShiftType Shift;
    CoreRegister Rm;
    CoreRegister Rs;
    uint32_t Immediate;
};

//! @brief Parameters of an address operand calculated from base and offset.
struct AddrOperand
{
    enum AOFlags
    {
        PreIndexed      = 0x01,
        Writeback       = 0x02,
        NegativeOffset  = 0x04,
    };

    ShifterOperand Offset;
    CoreRegister Rn;
    uint8_t Flags;
};

//! @brief The parameters to core ALU instructions.
struct CoreAlu
{
    ShifterOperand Op2;
    CoreRegister Rd;
    CoreRegister Rn;
    bool AffectsFlags;
};

//! @brief The parameters to core ALU comparison instructions.
struct CoreCompare
{
    ShifterOperand Op2;
    CoreRegister Rn;
    bool OverwritesPSR;
};

//! @brief The parameters for a core multiplication instruction.
struct CoreMultiply // <mnemonic>[S] Rd,Rm,Rs[,Rn]
{
    CoreRegister Rd;
    CoreRegister Rn;
    CoreRegister Rm;
    CoreRegister Rs;
    bool AffectsFlags;
};


//! @brief The parameters for a long multiplication instruction.
struct LongMultiply // <mnemonic>[S] RdLo,RdHi,Rm,Rs
{
    CoreRegister RdLo;
    CoreRegister RdHi;
    CoreRegister Rm;
    CoreRegister Rs;
    bool AffectsFlags;
};

//! @brief The parameters of a branch instruction.
struct Branch
{
    //! @brief The absolute address of the target of the branch operation.
    uint32_t Address;
};

//! @brief The parameters of an SWI instruction.
struct SoftwareIrq
{
    //! @brief The 24-bit comment field to encode in the instruction.
    uint32_t Comment;
};

//! @brief The parameters of the MRS instruction.
struct MoveFromPsr
{
    //! @brief The register to write to.
    CoreRegister Rd;

    //! @brief The status register to read, true for CPSR, false for SPSR.
    bool IsCPSR;
};

//! @brief The parameters of the MSR instruction.
struct MoveToPsr
{
    //! @brief The immediate constant to write to the PSR, if !IsSourceReg.
    uint32_t SourceImmediate;

    //! @brief The register to write to the PSR if IsSourceReg == true.
    CoreRegister SourceReg;

    //! @brief The components of the PSR to write expressed as a mask of
    //! PsrComponent enum values.
    uint8_t PsrComponents;

    //! @brief Indicate which status register is being updated, CPSR or SPSR.
    bool IsCPSR;

    //! @brief Indicates if the source value is defined by SoruceReg rather
    //! than SourceImmediate.
    bool IsSourceReg;
};

//! @brief The parameters of an BKPT instruction.
struct Breakpoint
{
    //! @brief The 16-bit comment field to encode in the instruction.
    uint16_t Comment;
};

//! @brief The parameters of the ADR pseudo-instruction.
struct EffectiveAddress
{
    //! @brief The absolute address to load into the destination register.
    uint32_t Address;

    //! @brief The register to receive the address.
    CoreRegister Rd;

    //! @brief The encoding used to define the PC-relative offset.
    MultiWordEncoding Encoding;
};

//! @brief The parameters of a single-register data transfer instruction.
struct CoreDataTransfer
{
    AddrOperand Addr;
    CoreRegister Rd;
    TransferDataType DataType;
    bool UserPrivilages;
};

//! @brief The parameters of a multi-register data transfer instruction.
struct CoreMultiTransfer
{
    uint16_t Registers;
    CoreRegister Rd;
    MultiTransferMode Mode;
    bool Writeback;
    bool UserModeRegs;
};

//! @brief The parameters for an atomic swap operation.
struct CoreSwap
{
    CoreRegister Rd;
    CoreRegister Rm;
    CoreRegister Rn;
    bool IsByte;
};

//! @brief The parameters of a generic co-processor data processing instruction.
struct CoProcDataProcessing
{
    CoProcId CoProcessor;
    CoProcRegister Rd;
    CoProcRegister Rn;
    CoProcRegister Rm;
    uint8_t OpCode1;
    uint8_t OpCode2;
};

//! @brief The parameters of a generic co-processor register transfer instruction.
struct CoProcRegisterTransfer
{
    CoProcId CoProcessor;
    CoreRegister Rd;
    CoProcRegister Rn;
    CoProcRegister Rm;
    uint8_t OpCode1;
    uint8_t OpCode2;
};

//! @brief The parameters of a generic co-processor data transfer instruction.
struct CoProcDataTransfer
{
    AddrOperand Addr;
    CoProcId CoProcessor;
    CoProcRegister Rd;
    bool IsLong;
};

//! @brief Allows access to the raw bits of the InstructionParams union as
//! 32-bit values for zero-initialisation.
struct InstructionParamsBits
{
    uint32_t Bits[1];
};

//! @brief Overlays all possible classes of instruction parameter data structure.
union InstructionParams
{
    CoreAlu CoreAluOp;
    CoreCompare CoreCmpOp;
    EffectiveAddress CoreAdr;
    CoreMultiply CoreMulOp;
    LongMultiply LongMulOp;
    Branch BranchOp;
    SoftwareIrq SoftwareIrqOp;
    MoveFromPsr MoveFromPsrOp;
    MoveToPsr MoveToPsrOp;
    Breakpoint BreakpointOp;
    CoreDataTransfer DataTransOp;
    CoreMultiTransfer MultiTransOp;
    CoreSwap AtomicSwapOp;
    CoProcDataProcessing CoProcDataProcOp;
    CoProcRegisterTransfer CoProcRegTransOp;
    CoProcDataTransfer CoProcDataTransferOp;
    InstructionParamsBits Bits;

    InstructionParams();
    void reset();
};

//! @brief An interface to an object which determines how an instruction
//! should be formatted as text.
class FormatterOptions
{
public:
    // Public Types
    //! @brief Defines options to apply when formatting instructions.
    enum Flags
    {
        //! @brief Indicates branch and PC-relative locations should be
        //! formatted as signed offsets from the current address as
        //! opposed to absolute addresses.
        ShowOffsets             = 0x01,

        //! @brief Indicates whether basic aliases should be used for
        //! core register names.
        //! @details If enabled R13 will be formatted as SP, R14 as Link,
        //! and R15 as PC.
        UseCoreRegAliases       = 0x02,

        //! @brief Indicates whether APCS-R register names should be used
        //! for expressing core register identifiers.
        //! @details This means a1-a4 for R0-R3, v1-v6 for R4-R9,
        //! sl = R10, fp= R11, ip = R12, sp = R13, lk = R14, pc = R15.
        UseAPCSRegAliases       = 0x04,

        //! @brief Indicates that immediate constant values should be
        //! formatted as base-10 rather than base-16 values.
        UseDecimalImmediates    = 0x08,

        //! @brief Indicates that memory offset values should be formatted as
        //! base-10 rather than base-16 values.
        UseDecimalOffsets       = 0x10,

        //! @brief Indicates that comment values embedded in SWI and BKPT
        //! instructions should be formatted as base-10 rather than
        //! base-16 values.
        UseDecimalComments      = 0x20,
    };

    // Construction/Destruction
    FormatterOptions();
    FormatterOptions(uint32_t address, uint32_t flags);
    virtual ~FormatterOptions() = default;

    // Accessors
    uint32_t getFlags() const;
    void setFlags(uint32_t flags);
    uint32_t getInstructionAddress() const;
    void setInstructionAddress(uint32_t addr);

    // Operations
    virtual void appendAddressSymbol(uint32_t address, std::string &buffer) const;
    virtual void appendSwiComment(uint32_t value, std::string &buffer) const;
    virtual void appendBreakPointComment(uint32_t value, std::string &buffer) const;
private:
    // Internal Fields
    uint32_t _address;
    uint32_t _flags;
};

//! @brief A data structure which describe any assembly language instruction.
class InstructionInfo
{
public:
    // Public Types
    //! @brief Defines bits used with the flags parameter to the disassemble()
    //! member function.
    enum DisasmBits
    {
        //! @brief Used with ModelMask to encode the highest instruction set
        //! variant to understand is the original ARM2.
        ARMv2       = 0x01,

        //! @brief Used with ModelMask to encode the highest instruction set
        //! variant to understand is the original ARM3.
        ARMv2a      = 0x02,

        //! @brief Used with ModelMask to encode the highest instruction set
        //! variant to understand is the ARMv3 architecture.
        ARMv3       = 0x03,

        //! @brief Used with ModelMask to encode the highest instruction set
        //! variant to understand is the ARMv4 architecture.
        ARMv4       = 0x04,

        //! @brief Used with ModelMask to encode the highest instruction set
        //! variant to understand is the ARMv5 architecture.
        ARMv5       = 0x05,

        //! @brief Used with ModelMask to encode the highest instruction set
        //! variant to understand is the ARMv6 architecture.
        ARMv6       = 0x06,

        //! @brief To extract the highest allow instruction set architecture
        //! or that ARMv7 instructions should be interpreted.
        ModelMask   = 0x07,

        //! @brief Indicates that Floating Point Accelerator co-processor
        //! instructions should be understood as more than just generic
        //! co-processor instructions.
        AllowFPA    = 0x08,

        //! @brief Indicates that thumb code-related 32-bit instructions
        //! should be interpreted.
        AllowThumb  = 0x10,

        //! @brief Indicates that LDM/STM instruction transfer modes should
        //! be interpreted in terms of stack operations when the destination
        //! register is R13 (SP).
        UseStackModesOnR13  = 0x20,

        //! @brief Indicates that LDM/STM instruction transfer modes should
        //! always be interpreted in terms of stack operations.
        UseStackModesAlways = 0x40,

        //! @brief Enables all instruction interpretation.
        All                 = 0x7F,
    };

    // Construction/Destruction
    InstructionInfo();
    InstructionInfo(InstructionMnemonic mnemonic, OperationClass opClass,
                    ConditionCode condition = ConditionCode::Al);
    ~InstructionInfo() = default;

    // Accessors
    bool isEmpty() const;
    ConditionCode getCondition() const;
    void setConditionCode(ConditionCode condition);
    InstructionMnemonic getMnemonic() const;
    OperationClass getOperationClass() const;

    const CoreAlu &getCoreAluParamters() const;
    CoreAlu &getCoreAluParamters();
    const CoreCompare &getCoreCompareParamters() const;
    CoreCompare &getCoreCompareParamters();
    const EffectiveAddress &getCoreAddressParameters() const;
    EffectiveAddress &getCoreAddressParameters();

    const CoreMultiply &getCoreMultiplyParameters() const;
    CoreMultiply &getCoreMultiplyParameters();
    const LongMultiply &getLongMultiplyParameters() const;
    LongMultiply &getLongMultiplyParameters();
    const Branch &getBranchParameters() const;
    Branch &getBranchParameters();
    const SoftwareIrq &getSoftwareIrqParameters() const;
    SoftwareIrq &getSoftwareIrqParameters();
    const MoveFromPsr &getMoveFromPsrParameters() const;
    MoveFromPsr &getMoveFromPsrParameters();
    const MoveToPsr &getMoveToPsrParameters() const;
    MoveToPsr &getMoveToPsrParameters();

    const Breakpoint &getBreakpointParameters() const;
    Breakpoint &getBreakpointParameters();

    const CoreDataTransfer &getCoreDataTransferParameters() const;
    CoreDataTransfer &getCoreDataTransferParameters();
    const CoreMultiTransfer &getCoreMultiTransferParameters() const;
    CoreMultiTransfer &getCoreMultiTransferParameters();
    const CoreSwap &getCoreSwapParameters() const;
    CoreSwap &getCoreSwapParameters();

    const CoProcDataProcessing &getCoProcDataProcessingParameters() const;
    CoProcDataProcessing &getCoProcDataProcessingParameters();
    const CoProcRegisterTransfer &getCoProcRegisterTransferParameters() const;
    CoProcRegisterTransfer &getCoProcRegisterTransferParameters();
    const CoProcDataTransfer &getCoProcDataTransferParameters() const;
    CoProcDataTransfer &getCoProcDataTransferParameters();

    // Operations
    bool assemble(uint32_t &instruction, uint32_t loadAddr,
                  Ag::string_ref_t error) const;
    size_t assemble(uint32_t *instructions, uint32_t loadAddr, size_t maxCount,
                    Ag::string_ref_t error) const;
    bool disassemble(uint32_t instruction, uint32_t loadAddress, uint32_t flags = 0x3F);
    uint8_t disassemble(const uint32_t *instructions, uint8_t count,
                        uint32_t loadAddress, uint32_t flags = 0x3F);
    Ag::String toString(const FormatterOptions *options = nullptr) const;
    Ag::String toString(uint32_t loadAddr, uint32_t formatterOptionsFlags) const;

    void clear();
    void reset(InstructionMnemonic mnemonic, OperationClass opClass,
               ConditionCode condition = ConditionCode::Al);
private:
    // Internal Functions
    void validateInstructionClass(OperationClass classification) const;

    // Internal Fields
    InstructionParams _params;
    ConditionCode _condition;
    InstructionMnemonic _mnemonic;
    OperationClass _opClass;
};

}} // namespace Mo::Asm

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
