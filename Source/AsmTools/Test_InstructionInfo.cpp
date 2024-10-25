//! @file AsmTools/Test_InstructionInfo.cpp
//! @brief The definition of unit tests for the InstructionInfo class.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "Ag/GTest_Core.hpp"
#include "Ag/Core/Exception.hpp"
#include "AsmTools/InstructionInfo.hpp"

#include "TestTools.hpp"

namespace Mo {
namespace Asm {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
void accessParameters(InstructionInfo &instruction, OperationClass opClass)
{
    switch (opClass)
    {
    case OperationClass::None:
        // Do nothing.
        break;

    case OperationClass::CoreAlu:
        instruction.getCoreAluParamters().AffectsFlags = false;
        break;

    case OperationClass::CoreCompare:
        instruction.getCoreCompareParamters().OverwritesPSR = false;
        break;

    case OperationClass::CoreAddress:
        instruction.getCoreAddressParameters().Rd = CoreRegister::R0;
        break;

    case OperationClass::CoreMultiply:
        instruction.getCoreMultiplyParameters().AffectsFlags = false;
        break;

    case OperationClass::CoreDataTransfer:
        instruction.getCoreDataTransferParameters().UserPrivilages = false;
        break;

    case OperationClass::CoreMultiTransfer:
        instruction.getCoreMultiTransferParameters().Registers = 0;
        break;

    case OperationClass::Branch:
        instruction.getBranchParameters().Address = 0x8000;
        break;

    case OperationClass::SoftwareIrq:
        instruction.getSoftwareIrqParameters().Comment = 0xDEADBEEF;
        break;

    case OperationClass::Breakpoint:
        instruction.getBreakpointParameters().Comment = 0xBABE;
        break;

    case OperationClass::AtomicSwap:
        instruction.getCoreSwapParameters().IsByte = true;
        break;

    case OperationClass::MoveFromPSR:
        instruction.getMoveFromPsrParameters().IsCPSR = false;
        break;

    case OperationClass::MoveToPSR:
        instruction.getMoveToPsrParameters().IsCPSR = false;
        break;

    case OperationClass::BranchExchange: break;

    case OperationClass::LongMultiply:
        instruction.getLongMultiplyParameters().AffectsFlags = false;
        break;

    case OperationClass::CoProcDataTransfer:
        instruction.getCoProcDataProcessingParameters().OpCode1 = 1;
        break;

    case OperationClass::CoProcRegisterTransfer:
        instruction.getCoProcRegisterTransferParameters().OpCode1 = 1;
        break;

    case OperationClass::CoProcDataProcessing:
        instruction.getCoProcDataTransferParameters().CoProcessor = CoProcId::CP0;
        break;

    case OperationClass::FpaDataTransfer: break;
    case OperationClass::FpaMultiTransfer: break;
    case OperationClass::FpaDyadicOperation: break;
    case OperationClass::FpaMonadicOperation: break;
    case OperationClass::FpaRegisterTransfer: break;
    case OperationClass::FpaComparison: break;
    default:
        throw Ag::NotSupportedException("The required instruction class.");
    }
}

//! @brief Modifies a line of assembly language to modify the condition code
//! suffix to set HS to CS and LO to CC.
//! @param[in,out] statement The ARM assembly language instruction to modify.
void canonicaliseConditionCode(Ag::string_ref_t statement)
{
        // We need to fix up synonymous condition codes:
        //  HS => CS
        //  LS => CC
    std::string buffer = statement.toUtf8();
    size_t firstSpace = buffer.find(' ');

    if (firstSpace != std::string::npos)
    {
        size_t match = buffer.rfind("HS", firstSpace);

        if (match == std::string::npos)
        {
            match = buffer.rfind("LO", firstSpace);

            if (match != std::string::npos)
            {
                // Edit the string.
                buffer[match] = 'C';
                buffer[match + 1] = 'C';
                statement = buffer;
            }
        }
        else
        {
            buffer[match] = 'C';
            buffer[match + 1] = 'S';
            statement = buffer;
        }
    }
}

//! @brief Gets a core register value from an integer then increments it.
//! @param[in,out] baseReg The integer defining the register to use and
//! increment.
//! @return The original register value.
CoreRegister getAndIncCoreReg(uint8_t &baseReg)
{
    CoreRegister reg = Ag::fromScalar<CoreRegister>(baseReg & 0xFF);
    ++baseReg;

    baseReg &= 0x0F;

    return reg;
}
//! @brief Gets a co-processor register value from an integer then increments it.
//! @param[in,out] baseReg The integer defining the register to use and
//! increment.
//! @return The original register value.
CoProcRegister getAndIncCoProcReg(uint8_t &baseReg)
{
    CoProcRegister reg = Ag::fromScalar<CoProcRegister>(baseReg & 0xFF);
    ++baseReg;

    baseReg &= 0x0F;

    return reg;
}

////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
// * - Operation Class Specific
// Configure *
// Assemble
// Verify machine code and text
// Alter condition code
// Assemble
// Verify machine code and text
// Specify bad parameters - verify assembly failure *

class InstructionTestPoint : public BaseTestPoint
{
private:
    Ag::String _asmText;
    uint32_t _encoding;
public:
    InstructionTestPoint(const TestLocation &loc, const char *name,
                         const char *asmText, uint32_t encoding = 0) :
        BaseTestPoint(loc, name),
        _asmText(asmText),
        _encoding(encoding)
    {
    }

    Ag::string_cref_t getAssemblerText() const { return _asmText; }
    uint32_t getEncoding() const { return _encoding; }

    // Operations
    virtual void configure(InstructionInfo &instruction) const = 0;
};

class SwiTestPoint : public InstructionTestPoint
{
private:
    uint32_t _comment;
    ConditionCode _condition;
public:
    SwiTestPoint(const TestLocation &loc, const char *name,
                 const char *asmText, ConditionCode condition,
                 uint32_t comment, uint32_t encoding = 0u) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _comment(comment),
        _condition(condition)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        instruction.reset(InstructionMnemonic::Swi,
                          OperationClass::SoftwareIrq,
                          _condition);
        instruction.getSoftwareIrqParameters().Comment = _comment;
    }
};

class BkptTestPoint : public InstructionTestPoint
{
private:
    uint16_t _comment;
public:
    BkptTestPoint(const TestLocation &loc, const char *name,
                  const char *asmText, uint16_t comment,
                  uint32_t encoding = 0u) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _comment(comment)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        instruction.reset(InstructionMnemonic::Bkpt,
                          OperationClass::Breakpoint,
                          ConditionCode::Al);
        instruction.getBreakpointParameters().Comment = _comment;
    }
};

class BranchTestPoint : public InstructionTestPoint
{
private:
    uint32_t _addr;
    ConditionCode _condition;
    InstructionMnemonic _mnemonic;
public:
    BranchTestPoint(const TestLocation &loc, const char *name,
                    const char *asmText, ConditionCode condition,
                    InstructionMnemonic mnemonic,
                    uint32_t addr, uint32_t encoding = 0u) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _addr(addr),
        _condition(condition),
        _mnemonic(mnemonic)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        instruction.reset(_mnemonic,
                          OperationClass::Branch,
                          _condition);
        instruction.getBranchParameters().Address = _addr;
    }
};

class MultiplyTestPoint : public InstructionTestPoint
{
private:
    CoreRegister _baseRegister;
    ConditionCode _condition;
    InstructionMnemonic _mnemonic;
    bool _affectsFlags;
public:
    MultiplyTestPoint(const TestLocation &loc, const char *name,
                      const char *asmText, ConditionCode condition,
                      InstructionMnemonic mnemonic, bool affectsFlags,
                      CoreRegister baseReg, uint32_t encoding = 0u) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _baseRegister(baseReg),
        _condition(condition),
        _mnemonic(mnemonic),
        _affectsFlags(affectsFlags)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        if ((_mnemonic == InstructionMnemonic::Mul) ||
            (_mnemonic == InstructionMnemonic::Mla))
        {
            instruction.reset(_mnemonic,
                              OperationClass::CoreMultiply,
                              _condition);
            auto &params = instruction.getCoreMultiplyParameters();
            uint8_t base = Ag::toScalar(_baseRegister);

            params.Rd = getAndIncCoreReg(base);
            params.Rm = getAndIncCoreReg(base);
            params.Rs = getAndIncCoreReg(base);

            if (_mnemonic == InstructionMnemonic::Mla)
            {
                params.Rn = getAndIncCoreReg(base);
            }

            params.AffectsFlags = _affectsFlags;
        }
        else
        {
            // It's a long multiply operation.
            instruction.reset(_mnemonic,
                              OperationClass::LongMultiply,
                              _condition);
            auto &params = instruction.getLongMultiplyParameters();
            uint8_t base = Ag::toScalar(_baseRegister);

            params.RdLo = getAndIncCoreReg(base);
            params.RdHi = getAndIncCoreReg(base);
            params.Rm = getAndIncCoreReg(base);
            params.Rs = getAndIncCoreReg(base);

            params.AffectsFlags = _affectsFlags;
        }
    }
};

class AluInstructionTestPoint : public InstructionTestPoint
{
private:
    CoreRegister _baseRegister;
    ConditionCode _condition;
    InstructionMnemonic _mnemonic;
    ShifterMode _shifterMode;
    ShiftType _shiftType;
    bool _affectsFlags;
public:
    AluInstructionTestPoint(const TestLocation &loc, const char *name,
                            const char *asmText, ConditionCode condition,
                            InstructionMnemonic mnemonic, bool affectsFlags,
                            CoreRegister baseReg, ShifterMode shifterMode,
                            uint32_t encoding = 0u,
                            ShiftType shiftType = ShiftType::None) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _baseRegister(baseReg),
        _condition(condition),
        _mnemonic(mnemonic),
        _shifterMode(shifterMode),
        _shiftType(shiftType),
        _affectsFlags(affectsFlags)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        uint8_t baseReg = Ag::toScalar(_baseRegister);
        ShifterOperand *shifter = nullptr;

        if ((_mnemonic == InstructionMnemonic::Cmp) ||
            (_mnemonic == InstructionMnemonic::Cmn) ||
            (_mnemonic == InstructionMnemonic::Tst) ||
            (_mnemonic == InstructionMnemonic::Teq))
        {
            instruction.reset(_mnemonic, OperationClass::CoreCompare, _condition);
            auto &params = instruction.getCoreCompareParamters();
            params.OverwritesPSR = _affectsFlags;
            params.Rn = getAndIncCoreReg(baseReg);
            shifter = &params.Op2;
        }
        else
        {
            instruction.reset(_mnemonic, OperationClass::CoreAlu, _condition);
            auto &params = instruction.getCoreAluParamters();
            params.AffectsFlags = _affectsFlags;
            params.Rd = getAndIncCoreReg(baseReg);
            shifter = &params.Op2;

            if ((_mnemonic != InstructionMnemonic::Mov) &&
                (_mnemonic != InstructionMnemonic::Mvn))
            {
                params.Rn = getAndIncCoreReg(baseReg);
            }
        }

        if (shifter != nullptr)
        {
            shifter->Mode = _shifterMode;
            shifter->Shift = _shiftType;

            switch (_shifterMode)
            {
            case ShifterMode::ImmediateConstant:
                shifter->Immediate = baseReg;
                break;

            case ShifterMode::Register:
            case ShifterMode::RotateWithExtend:
                shifter->Rm = getAndIncCoreReg(baseReg);
                break;

            case ShifterMode::ShiftByRegister:
                shifter->Rm = getAndIncCoreReg(baseReg);
                shifter->Rs = getAndIncCoreReg(baseReg);
                break;

            case ShifterMode::ShiftByConstant:
                shifter->Rm = getAndIncCoreReg(baseReg);
                shifter->Immediate = baseReg;
                break;
            }
        }
    }
};

enum DataTranTestFlags
{
    DTTF_PCRelative             = 0x000,
    DTTF_OffsetTypeNone         = 0x001,
    DTTF_OffsetTypeConstant     = 0x002,
    DTTF_OffsetTypeReg          = 0x003,
    DTTF_OffsetTypeShiftedReg   = 0x004,

    DTTF_TypeMask               = 0x007,

    DTTF_Word                   = 0x000,
    DTTF_UByte                  = 0x008,
    DTTF_SByte                  = 0x010,
    DTTF_UHalfWord              = 0x018,
    DTTF_SHalfWord              = 0x020,
    DTTF_DataTypeMask           = 0x038,

    DTTF_SubtractOffset         = 0x040,
    DTTF_PreIndexed             = 0x080,
    DTTF_WritebackOrTran        = 0x100,
};

class DataTranInstructionTestPoint : public InstructionTestPoint
{
private:
    uint16_t _flags;
    uint8_t _baseRegister;
    ConditionCode _condition;
    InstructionMnemonic _mnemonic;
    ShiftType _shiftType;
public:
    DataTranInstructionTestPoint(const TestLocation &loc, const char *name,
                                 const char *asmText, ConditionCode condition,
                                 InstructionMnemonic mnemonic, uint8_t baseReg,
                                 uint16_t flags, uint32_t encoding = 0u,
                                 ShiftType shiftType = ShiftType::None) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _flags(flags),
        _baseRegister(baseReg),
        _condition(condition),
        _mnemonic(mnemonic),
        _shiftType(shiftType)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        uint8_t baseReg = _baseRegister;

        instruction.reset(_mnemonic, OperationClass::CoreDataTransfer,
                          _condition);

        auto &info = instruction.getCoreDataTransferParameters();

        info.Rd = getAndIncCoreReg(baseReg);

        switch (_flags & DTTF_TypeMask)
        {
        case DTTF_PCRelative:
            info.Addr.Rn = CoreRegister::R15;
            info.Addr.Offset.Mode = ShifterMode::ImmediateConstant;
            info.Addr.Offset.Immediate = static_cast<uint32_t>(baseReg) * 4;
            info.Addr.Flags |= AddrOperand::PreIndexed;
            break;

        case DTTF_OffsetTypeNone:
            info.Addr.Rn = getAndIncCoreReg(baseReg);
            info.Addr.Offset.Mode = ShifterMode::ImmediateConstant;
            info.Addr.Offset.Immediate = 0;
            break;

        case DTTF_OffsetTypeConstant:
            info.Addr.Rn = getAndIncCoreReg(baseReg);
            info.Addr.Offset.Mode = ShifterMode::ImmediateConstant;
            info.Addr.Offset.Immediate = static_cast<uint32_t>(baseReg) * 4;
            break;

        case DTTF_OffsetTypeReg:
            info.Addr.Rn = getAndIncCoreReg(baseReg);
            info.Addr.Offset.Mode = ShifterMode::Register;
            info.Addr.Offset.Rm = getAndIncCoreReg(baseReg);
            break;

        case DTTF_OffsetTypeShiftedReg:
            info.Addr.Rn = getAndIncCoreReg(baseReg);
            info.Addr.Offset.Mode = ShifterMode::ShiftByConstant;
            info.Addr.Offset.Rm = getAndIncCoreReg(baseReg);
            info.Addr.Offset.Shift = _shiftType;

            if (_shiftType == ShiftType::Rrx)
            {
                info.Addr.Offset.Mode = ShifterMode::RotateWithExtend;
            }
            else
            {
                info.Addr.Offset.Mode = ShifterMode::ShiftByConstant;
                info.Addr.Offset.Immediate = baseReg;
            }
            break;
        }

        switch (_flags & DTTF_DataTypeMask)
        {
        case DTTF_Word: info.DataType = TransferDataType::Word; break;
        case DTTF_UByte: info.DataType = TransferDataType::UnsignedByte; break;
        case DTTF_SByte: info.DataType = TransferDataType::SignedByte; break;
        case DTTF_UHalfWord: info.DataType = TransferDataType::UnsignedHalfWord; break;
        case DTTF_SHalfWord: info.DataType = TransferDataType::SignedHalfWord; break;
        }

        if (_flags & DTTF_SubtractOffset)
        {
            info.Addr.Flags |= AddrOperand::NegativeOffset;
        }

        if (_flags & DTTF_PreIndexed)
        {
            info.Addr.Flags |= AddrOperand::PreIndexed;

            if (_flags & DTTF_WritebackOrTran)
            {
                info.Addr.Flags |= AddrOperand::Writeback;
            }
        }
        else
        {
            if (_flags & DTTF_WritebackOrTran)
            {
                info.UserPrivilages = true;
            }
        }
    }
};

class MultiTransferTestPoint : public InstructionTestPoint
{
private:
    uint16_t _regSet;
    CoreRegister _destRegister;
    ConditionCode _condition;
    InstructionMnemonic _mnemonic;
    MultiTransferMode _mode;
    bool _writeback;
    bool _userModeRegs;
public:
    MultiTransferTestPoint(const TestLocation &loc, const char *name,
                           const char *asmText, ConditionCode condition,
                           InstructionMnemonic mnemonic,
                           MultiTransferMode mode,
                           bool writeback, bool userModeRegs,
                           CoreRegister destReg, uint16_t regSet,
                           uint32_t encoding = 0u) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _regSet(regSet),
        _destRegister(destReg),
        _condition(condition),
        _mnemonic(mnemonic),
        _mode(mode),
        _writeback(writeback),
        _userModeRegs(userModeRegs)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        instruction.reset(_mnemonic, OperationClass::CoreMultiTransfer,
                          _condition);
        auto &info = instruction.getCoreMultiTransferParameters();

        info.Registers = _regSet;
        info.Rd = _destRegister;
        info.Mode = _mode;
        info.Writeback = _writeback;
        info.UserModeRegs = _userModeRegs;
    }
};

class SwapTestPoint : public InstructionTestPoint
{
private:
    ConditionCode _condition;
    CoreRegister _baseReg;
    bool _isByte;
public:
    SwapTestPoint(const TestLocation &loc, const char *name,
                  const char *asmText, ConditionCode condition,
                  bool isByte, CoreRegister baseReg,
                  uint32_t encoding = 0u) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _condition(condition),
        _baseReg(baseReg),
        _isByte(isByte)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        instruction.reset(InstructionMnemonic::Swp,
                            OperationClass::AtomicSwap,
                            _condition);
        auto &params = instruction.getCoreSwapParameters();
        uint8_t base = Ag::toScalar(_baseReg);

        params.Rd = getAndIncCoreReg(base);
        params.Rm = getAndIncCoreReg(base);
        params.Rn = getAndIncCoreReg(base);
        params.IsByte = _isByte;
    }
};


class WriteStatusRegTestPoint : public InstructionTestPoint
{
private:
    MoveToPsr _params;
    ConditionCode _condition;
public:
    WriteStatusRegTestPoint(const TestLocation &loc, const char *name,
                            const char *asmText, uint32_t encoding,
                            ConditionCode condition, CoreRegister sourceReg,
                            bool isCPSR, uint8_t statusMask = 0x0F) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _condition(condition)
    {
        _params.SourceImmediate = 0;
        _params.SourceReg = sourceReg;
        _params.PsrComponents = statusMask & 0x0F;
        _params.IsCPSR = isCPSR;
        _params.IsSourceReg = true;
    }

    WriteStatusRegTestPoint(const TestLocation &loc, const char *name,
                            const char *asmText, uint32_t encoding,
                            ConditionCode condition, uint32_t sourceVal,
                            bool isCPSR, uint8_t statusMask = 0x0F) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _condition(condition)
    {
        _params.SourceImmediate = sourceVal;
        _params.SourceReg = CoreRegister::R0;
        _params.PsrComponents = statusMask & 0x0F;
        _params.IsCPSR = isCPSR;
        _params.IsSourceReg = false;
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        instruction.reset(InstructionMnemonic::Msr,
                          OperationClass::MoveToPSR,
                          _condition);
        instruction.getMoveToPsrParameters() = _params;
    }
};

class ReadStatusRegTestPoint : public InstructionTestPoint
{
private:
    MoveFromPsr _params;
    ConditionCode _condition;
public:
    ReadStatusRegTestPoint(const TestLocation &loc, const char *name,
                            const char *asmText, uint32_t encoding,
                            ConditionCode condition, CoreRegister destReg,
                            bool isCPSR) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _condition(condition)
    {
        _params.IsCPSR = isCPSR;
        _params.Rd = destReg;
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        instruction.reset(InstructionMnemonic::Mrs,
                          OperationClass::MoveFromPSR,
                          _condition);
        instruction.getMoveFromPsrParameters() = _params;
    }
};

class CoProcOpTestPoint : public InstructionTestPoint
{
private:
    InstructionMnemonic _mnemonic;
    ConditionCode _condition;
    CoProcId _cpId;
    CoProcRegister _baseReg;
    uint8_t _opCodeBase;
public:
    CoProcOpTestPoint(const TestLocation &loc, const char *name,
                                  const char *asmText, uint32_t encoding,
                                  InstructionMnemonic mnemonic,
                                  ConditionCode condition, CoProcId cpId,
                                  CoProcRegister baseReg, uint8_t opCodeBase) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _mnemonic(mnemonic),
        _condition(condition),
        _cpId(cpId),
        _baseReg(baseReg),
        _opCodeBase(opCodeBase)
    {
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        if (_mnemonic == InstructionMnemonic::Cdp)
        {
            instruction.reset(InstructionMnemonic::Cdp,
                              OperationClass::CoProcDataProcessing,
                              _condition);

            auto &params = instruction.getCoProcDataProcessingParameters();
            params.OpCode1 = _opCodeBase & 0x0F;
            params.OpCode2 = (_opCodeBase + 1) & 7;
            params.CoProcessor = _cpId;

            uint8_t base = Ag::toScalar(_baseReg);

            params.Rd = getAndIncCoProcReg(base);
            params.Rn = getAndIncCoProcReg(base);
            params.Rm = getAndIncCoProcReg(base);
        }
        else if ((_mnemonic == InstructionMnemonic::Mcr) ||
                 (_mnemonic == InstructionMnemonic::Mrc))
        {
            instruction.reset(_mnemonic,
                              OperationClass::CoProcRegisterTransfer,
                              _condition);

            auto &params = instruction.getCoProcRegisterTransferParameters();

            params.OpCode1 = _opCodeBase & 0x07;
            params.OpCode2 = (_opCodeBase + 1) & 7;
            params.CoProcessor = _cpId;

            uint8_t base = Ag::toScalar(_baseReg);

            params.Rd = getAndIncCoreReg(base);
            params.Rn = getAndIncCoProcReg(base);
            params.Rm = getAndIncCoProcReg(base);
        }
    }
};


class CoProcDataTransferTestPoint : public InstructionTestPoint
{
private:
    bool _isLoad;
    ConditionCode _condition;
    CoProcDataTransfer _params;
public:
    CoProcDataTransferTestPoint(const TestLocation &loc, const char *name,
                                const char *asmText, uint32_t encoding,
                                bool isLoad, bool isLong, ConditionCode condition,
                                CoProcId cpId, CoreRegister baseReg,
                                int32_t offset, bool isPreIndexed,
                                bool hasWriteback) :
        InstructionTestPoint(loc, name, asmText, encoding),
        _isLoad(isLoad),
        _condition(condition)
    {
        Ag::zeroFill(_params);

        _params.IsLong = isLong;
        _params.CoProcessor = cpId;
        uint8_t baseRegister = Ag::toScalar(baseReg);

        _params.Rd = getAndIncCoProcReg(baseRegister);
        _params.Addr.Rn = getAndIncCoreReg(baseRegister);

        if (isPreIndexed)
        {
            _params.Addr.Flags |= AddrOperand::PreIndexed;

            if (hasWriteback)
            {
                _params.Addr.Flags |= AddrOperand::Writeback;
            }
        }

        _params.Addr.Offset.Mode = ShifterMode::ImmediateConstant;

        if (offset < 0)
        {
            _params.Addr.Flags |= AddrOperand::NegativeOffset;

            uint32_t effectiveOffset = static_cast<uint32_t>(-offset);

            _params.Addr.Offset.Immediate = effectiveOffset;
        }
        else
        {
            _params.Addr.Offset.Immediate = static_cast<uint32_t>(offset);
        }
    }

    virtual void configure(InstructionInfo &instruction) const override
    {
        instruction.reset(_isLoad ? InstructionMnemonic::Ldc : InstructionMnemonic::Stc,
                          OperationClass::CoProcDataTransfer,
                          _condition);

        // Copy the parameters directly.
        instruction.getCoProcDataTransferParameters() = _params;
    }
};

template<typename TTestPoint>
class PositiveInstructionTest : public BaseTestFixture
{
private:
    TTestPoint _point;

    ::testing::AssertionResult noErrors(const Messages &log)
    {
        if (log.hasErrors())
        {
            auto result = testing::AssertionFailure();

            result << "Assembly failed unexpectedly\n";
            appendLog(result, log);

            return result;
        }
        else
        {
            return testing::AssertionSuccess();
        }
    }

public:
    PositiveInstructionTest(const TTestPoint &pt) :
        BaseTestFixture("InstructionInfo"),
        _point(pt)
    {
    }

    virtual void TestBody() override
    {
        InstructionInfo specimen;
        const uint32_t baseAddr = 0x10000;

        _point.configure(specimen);

        Ag::String text = specimen.toString(baseAddr, FormatterOptions::ShowOffsets |
                                                      FormatterOptions::UseBasicStyleHex);
        EXPECT_STRINGEQ(text, _point.getAssemblerText());

        Ag::String error;
        uint32_t objectCode = 0;

        ASSERT_TRUE(specimen.assemble(objectCode, baseAddr, error)) << error.getUtf8Bytes();
        EXPECT_TRUE(equalHex(_point.getEncoding(), objectCode));

        // Attempt to re-populated the object by disassembly.
        specimen.disassemble(objectCode, baseAddr,
                             InstructionInfo::ModelMask |
                             InstructionInfo::AllowFPA |
                             InstructionInfo::AllowThumb |
                             InstructionInfo::UseStackModesOnR13);
        Ag::String disText = specimen.toString(baseAddr, FormatterOptions::ShowOffsets |
                                                         FormatterOptions::UseBasicStyleHex);

        // Ensure the condition code from source matches that which was
        // interpreted through disassembly.
        canonicaliseConditionCode(text);

        // Ensure the text output from disassembly matches what was
        // originally expected.
        EXPECT_STRINGEQ(disText, text);
    }
};

template<typename TTestPoint>
class NegativeInstructionTest : public BaseTestFixture
{
private:
    TTestPoint _point;
public:
    NegativeInstructionTest(const TTestPoint &pt) :
        BaseTestFixture("InstructionInfo"),
        _point(pt)
    {
    }

    virtual void TestBody() override
    {
        InstructionInfo specimen;
        const uint32_t baseAddr = 0x10000;

        _point.configure(specimen);

        Ag::String text = specimen.toString(baseAddr, FormatterOptions::ShowOffsets |
                                                      FormatterOptions::UseBasicStyleHex);
        EXPECT_STRINGEQ(text, _point.getAssemblerText());

        Ag::String error;
        uint32_t objectCode = 0;
        EXPECT_FALSE(specimen.assemble(objectCode, baseAddr, error));
        EXPECT_FALSE(error.isEmpty());
    }
};


////////////////////////////////////////////////////////////////////////////////
// Unit Tests
////////////////////////////////////////////////////////////////////////////////
GTEST_TEST(InstructionInfo, CannotAccessWrongParameters)
{
    // Define short hands for convenience.
    using II = InstructionInfo;
    using IM = InstructionMnemonic;

    constexpr uint8_t c = 10;  Ag::toScalar(OperationClass::Max);

    for (uint8_t i = 1; i < c; ++i)
    {
        std::string trace;
        II s(IM::Abs, Ag::fromScalar<OperationClass>(i));

        for (uint8_t j = 1; j < c; ++j)
        {
            OperationClass opClass = Ag::fromScalar<OperationClass>(j);
            trace.clear();
            Ag::appendFormat("OpClass #{0} vs #{1}", trace, { i, j });
            SCOPED_TRACE(trace.c_str());

            if (i == j)
            {
                EXPECT_NO_THROW({ accessParameters(s, opClass); });
            }
            else
            {
                EXPECT_THROW({ accessParameters(s, opClass); }, Ag::OperationException);
            }
        }
    }
}

} // Anonymous namespace

void registerInstructionTests()
{
    SwiTestPoint swiSuccesses[] = {
        { LOC, "Swi_Simple", "SWI &DEAD", ConditionCode::Al, 0xDEAD, 0xEF00DEAD },
        { LOC, "Swi_Condition", "SWIVS &CAFEEE", ConditionCode::Vs, 0xCAFEEE, 0x6FCAFEEE },
    };

    registerTestPoints<PositiveInstructionTest<SwiTestPoint>>("Instruction_SWI",
                                                              swiSuccesses,
                                                              std::size(swiSuccesses));

    SwiTestPoint swiFail[] = {
        { LOC, "Swi_Simple", "SWI &DEADBEEF", ConditionCode::Al, 0xDEADBEEF },
        { LOC, "Swi_Condition", "SWIVS &CAFEBABE", ConditionCode::Vs, 0xCAFEBABE },
    };

    registerTestPoints<NegativeInstructionTest<SwiTestPoint>>("Instruction_SWI_Fail",
                                                              swiFail,
                                                              std::size(swiFail));

    BkptTestPoint bkptSuccesses[] = {
        { LOC, "Bkpt_Simple", "BKPT &DEAD", 0xDEAD, 0xE12DEA7D },
        { LOC, "Bkpt_Zero", "BKPT &00", 0x00, 0xE1200070 },
    };

    registerTestPoints<PositiveInstructionTest<BkptTestPoint>>("Instruction_BKPT",
                                                               bkptSuccesses,
                                                               std::size(bkptSuccesses));

    BranchTestPoint branchSuccesses[] = {
        { LOC, "Branch_Simple", "B $+&18", ConditionCode::Al, InstructionMnemonic::B, 0x10018, 0xEA000004 },
        { LOC, "Branch_Condition", "BLT $+&18", ConditionCode::Lt, InstructionMnemonic::B, 0x10018, 0xBA000004 },
    };

    registerTestPoints<PositiveInstructionTest<BranchTestPoint>>(
        "Instruction_Branch", branchSuccesses, std::size(branchSuccesses));

    BranchTestPoint branchFail[] = {
        { LOC, "Branch_OffsetTooHigh", "BGE $+&4000000", ConditionCode::Ge, InstructionMnemonic::B, 0x4010000u },
        { LOC, "Branch_OffsetTooLow", "BLHI $-&4000000", ConditionCode::Hi, InstructionMnemonic::Bl, static_cast<uint32_t>(0x10000 - 0x4000000) },
    };

    registerTestPoints<NegativeInstructionTest<BranchTestPoint>>(
        "Instruction_Branch_Fail", branchFail, std::size(branchFail));

    MultiplyTestPoint mulSuccesses[] = {
        { LOC, "Mul_Simple", "MUL R1, R2, R3", ConditionCode::Al, InstructionMnemonic::Mul, false, CoreRegister::R1, 0xE0010392 },
        { LOC, "Mul_Condition", "MULLT R13, R14, R15", ConditionCode::Lt, InstructionMnemonic::Mul, false, CoreRegister::R13, 0xB00D0F9E },
        { LOC, "Mul_Psr", "MULS R4, R5, R6", ConditionCode::Al, InstructionMnemonic::Mul, true, CoreRegister::R4, 0xE0140695 },

        { LOC, "Mla_Simple", "MLA R1, R2, R3, R4", ConditionCode::Al, InstructionMnemonic::Mla, false, CoreRegister::R1, 0xE0214392 },
        { LOC, "Mla_Condition", "MLAPL R13, R14, R15, R0", ConditionCode::Pl, InstructionMnemonic::Mla, false, CoreRegister::R13, 0x502D0F9E },
        { LOC, "Mla_Psr", "MLAS R7, R8, R9, R10", ConditionCode::Al, InstructionMnemonic::Mla, true, CoreRegister::R7, 0xE037A998 },

        { LOC, "Umull_Simple", "UMULL R1, R2, R3, R4", ConditionCode::Al, InstructionMnemonic::Umull, false, CoreRegister::R1, 0xE0821493 },
        { LOC, "Umull_Condition", "UMULLPL R13, R14, R15, R0", ConditionCode::Pl, InstructionMnemonic::Umull, false, CoreRegister::R13, 0x508ED09F },
        { LOC, "Umull_Psr", "UMULLS R7, R8, R9, R10", ConditionCode::Al, InstructionMnemonic::Umull, true, CoreRegister::R7, 0xE0987A99 },

        { LOC, "Umlal_Simple", "UMLAL R1, R2, R3, R4", ConditionCode::Al, InstructionMnemonic::Umlal, false, CoreRegister::R1, 0xE0A21493 },
        { LOC, "Umlal_Condition", "UMLALPL R13, R14, R15, R0", ConditionCode::Pl, InstructionMnemonic::Umlal, false, CoreRegister::R13, 0x50AED09F },
        { LOC, "Umlal_Psr", "UMLALS R7, R8, R9, R10", ConditionCode::Al, InstructionMnemonic::Umlal, true, CoreRegister::R7, 0xE0B87A99 },

        { LOC, "Smull_Simple", "SMULL R1, R2, R3, R4", ConditionCode::Al, InstructionMnemonic::Smull, false, CoreRegister::R1, 0xE0C21493 },
        { LOC, "Smull_Condition", "SMULLPL R13, R14, R15, R0", ConditionCode::Pl, InstructionMnemonic::Smull, false, CoreRegister::R13, 0x50CED09F },
        { LOC, "Smull_Psr", "SMULLS R7, R8, R9, R10", ConditionCode::Al, InstructionMnemonic::Smull, true, CoreRegister::R7, 0xE0D87A99 },

        { LOC, "Smlal_Simple", "SMLAL R1, R2, R3, R4", ConditionCode::Al, InstructionMnemonic::Smlal, false, CoreRegister::R1, 0xE0E21493 },
        { LOC, "Smlal_Condition", "SMLALPL R13, R14, R15, R0", ConditionCode::Pl, InstructionMnemonic::Smlal, false, CoreRegister::R13, 0x50EED09F },
        { LOC, "Smlal_Psr", "SMLALS R7, R8, R9, R10", ConditionCode::Al, InstructionMnemonic::Smlal, true, CoreRegister::R7, 0xE0F87A99 },
    };

    registerTestPoints<PositiveInstructionTest<MultiplyTestPoint>>(
        "Instruction_Multiply", mulSuccesses, std::size(mulSuccesses));

    AluInstructionTestPoint aluSuccess[] = {
        { LOC, "Add_Immediate", "ADD R1, R2, #3", ConditionCode::Al, InstructionMnemonic::Add, false, CoreRegister::R1, ShifterMode::ImmediateConstant, 0xE2821003 },
        { LOC, "And_Register", "AND R1, R2, R3", ConditionCode::Al, InstructionMnemonic::And, false, CoreRegister::R1, ShifterMode::Register, 0xE0021003 },
        { LOC, "Adc_LSL_Register", "ADCNES R9, R10, R11, LSL R12", ConditionCode::Ne, InstructionMnemonic::Adc, true, CoreRegister::R9, ShifterMode::ShiftByRegister, 0x10BA9C1B, ShiftType::Lsl },
        { LOC, "Sub_LSR_Immediate", "SUBCS R4, R5, R6, LSR #7", ConditionCode::Cs, InstructionMnemonic::Sub, false, CoreRegister::R4, ShifterMode::ShiftByConstant, 0x204543A6, ShiftType::Lsr },
        { LOC, "Rsb_ASR_Immediate", "RSBHSS R9, R10, R11, ASR #12", ConditionCode::Hs, InstructionMnemonic::Rsb, true, CoreRegister::R9, ShifterMode::ShiftByConstant, 0x207A964B, ShiftType::Asr },
        { LOC, "Rsc_ROR_Immediate", "RSCS R14, R15, R0, ROR #1", ConditionCode::Al, InstructionMnemonic::Rsc, true, CoreRegister::R14, ShifterMode::ShiftByConstant, 0xE0FFE0E0, ShiftType::Ror },
        { LOC, "Bic_RRX", "BICVSS R9, R10, R11, RRX", ConditionCode::Vs, InstructionMnemonic::Bic, true, CoreRegister::R9, ShifterMode::RotateWithExtend, 0x61DA906B, ShiftType::Rrx },
        { LOC, "Eor_Register", "EOR R15, R0, R1", ConditionCode::Al, InstructionMnemonic::Eor, false, CoreRegister::R15, ShifterMode::Register, 0xE020F001 },
        { LOC, "Sub_Register_Prs", "SUBS R9, R10, R11", ConditionCode::Al, InstructionMnemonic::Sub, true, CoreRegister::R9, ShifterMode::Register, 0xE05A900B },
        { LOC, "Mov_Register", "MOV R6, R7", ConditionCode::Al, InstructionMnemonic::Mov, false, CoreRegister::R6, ShifterMode::Register, 0xE1A06007 },
        { LOC, "Mvn_Register_Psr", "MVNS R2, R3", ConditionCode::Al, InstructionMnemonic::Mvn, true, CoreRegister::R2, ShifterMode::Register, 0xE1F02003 },
        { LOC, "Cmp_Register", "CMP R5, R6", ConditionCode::Al, InstructionMnemonic::Cmp, false, CoreRegister::R5, ShifterMode::Register, 0xE1550006 },
        { LOC, "Teq_Register_Psr", "TEQP R13, R14", ConditionCode::Al, InstructionMnemonic::Teq, true, CoreRegister::R13, ShifterMode::Register, 0xE13DF00E },
    };

    registerTestPoints<PositiveInstructionTest<AluInstructionTestPoint>>(
        "Instruction_ALU", aluSuccess, std::size(aluSuccess));

    DataTranInstructionTestPoint dtrSuccess[] = {
        { LOC, "Ldr_PreImmed", "LDR R0, [R1, #8]", ConditionCode::Al, InstructionMnemonic::Ldr, 0, DTTF_OffsetTypeConstant | DTTF_PreIndexed, 0xE5910008, ShiftType::None },
        { LOC, "Str_PreReg", "STREQ R9, [R10, R11]", ConditionCode::Eq, InstructionMnemonic::Str, 9, DTTF_OffsetTypeReg | DTTF_PreIndexed, 0x078A900B, ShiftType::None },
        { LOC, "Ldr_PreLsl", "LDRB R4, [R5, R6, LSL #7]", ConditionCode::Al, InstructionMnemonic::Ldr, 4, DTTF_OffsetTypeShiftedReg | DTTF_PreIndexed | DTTF_UByte, 0xE7D54386, ShiftType::Lsl },
        { LOC, "Str_PreLsr", "STRHI R1, [R2, R3, LSR #4]", ConditionCode::Hi, InstructionMnemonic::Str, 1, DTTF_OffsetTypeShiftedReg | DTTF_PreIndexed, 0x87821223, ShiftType::Lsr },
        { LOC, "Ldr_PreAsr", "LDRNE R7, [R8, R9, ASR #10]", ConditionCode::Ne, InstructionMnemonic::Ldr, 7, DTTF_OffsetTypeShiftedReg | DTTF_PreIndexed, 0x17987549, ShiftType::Asr },
        { LOC, "Str_PerRor", "STRVC R14, [R15, R0, ROR #1]", ConditionCode::Vc, InstructionMnemonic::Str, 14, DTTF_OffsetTypeShiftedReg | DTTF_PreIndexed, 0x778FE0E0, ShiftType::Ror },
        { LOC, "Ldr_PreRrx", "LDR R10, [R11, R12, RRX]", ConditionCode::Al, InstructionMnemonic::Ldr, 10, DTTF_OffsetTypeShiftedReg | DTTF_PreIndexed, 0xE79BA06C, ShiftType::Rrx },
        { LOC, "Str_PreWriteBack", "STRB R2, [R3, #16]!", ConditionCode::Al, InstructionMnemonic::Str, 2, DTTF_OffsetTypeConstant | DTTF_PreIndexed | DTTF_UByte | DTTF_WritebackOrTran, 0xE5E32010, ShiftType::None },
        { LOC, "Ldr_NoOffset", "LDRVS R9, [R10]", ConditionCode::Vs, InstructionMnemonic::Ldr, 9, DTTF_OffsetTypeNone | DTTF_PreIndexed, 0x659A9000, ShiftType::None },
        { LOC, "Str_PostImmed", "STR R3, [R4], #20", ConditionCode::Al, InstructionMnemonic::Str, 3, DTTF_OffsetTypeConstant, 0xE4843014, ShiftType::None },
        { LOC, "Ldr_PostReg", "LDR R5, [R6], R7", ConditionCode::Al, InstructionMnemonic::Ldr, 5, DTTF_OffsetTypeReg, 0xE6965007, ShiftType::None },
        { LOC, "Str_PostLsl", "STRT R6, [R7], R8, LSL #9", ConditionCode::Al, InstructionMnemonic::Str, 6, DTTF_OffsetTypeShiftedReg | DTTF_WritebackOrTran, 0xE6A76488, ShiftType::Lsl },
        { LOC, "Ldr_PostLsr", "LDRB R0, [R1], R2, LSR #3", ConditionCode::Al, InstructionMnemonic::Ldr, 0, DTTF_OffsetTypeShiftedReg | DTTF_UByte, 0xE6D101A2, ShiftType::Lsr },
        { LOC, "Str_PostAsr", "STRBT R3, [R4], R5, ASR #6", ConditionCode::Al, InstructionMnemonic::Str, 3, DTTF_OffsetTypeShiftedReg | DTTF_UByte | DTTF_WritebackOrTran, 0xE6E43345, ShiftType::Asr },
        { LOC, "Ldr_PostRor", "LDRNV R2, [R3], R4, ROR #5", ConditionCode::Nv, InstructionMnemonic::Ldr, 2, DTTF_OffsetTypeShiftedReg, 0xF69322E4, ShiftType::Ror },
        { LOC, "Str_PostRrx", "STRLO R1, [R2], R3, RRX", ConditionCode::Lo, InstructionMnemonic::Str, 1, DTTF_OffsetTypeShiftedReg, 0x36821063, ShiftType::Rrx },
        { LOC, "Ldr_PreNegImmed", "LDR R0, [R1, #-8]", ConditionCode::Al, InstructionMnemonic::Ldr, 0, DTTF_OffsetTypeConstant | DTTF_PreIndexed | DTTF_SubtractOffset, 0xE5110008 },
        { LOC, "Str_PreNegReg", "STREQ R9, [R10, -R11]", ConditionCode::Eq, InstructionMnemonic::Str, 9, DTTF_OffsetTypeReg | DTTF_PreIndexed | DTTF_SubtractOffset, 0x070A900B },
        { LOC, "Ldr_PostNegReg", "LDR R5, [R6], -R7", ConditionCode::Al, InstructionMnemonic::Ldr, 5, DTTF_OffsetTypeReg | DTTF_SubtractOffset, 0xE6165007 },
        { LOC, "Str_PostNegImmed", "STR R3, [R4], #-20", ConditionCode::Al, InstructionMnemonic::Str, 3, DTTF_OffsetTypeConstant | DTTF_SubtractOffset, 0xE4043014 },
    };

    registerTestPoints<PositiveInstructionTest<DataTranInstructionTestPoint>>(
        "Instruction_DataTransfer", dtrSuccess, std::size(dtrSuccess));

    MultiTransferTestPoint mtrSuccess[] = {
        { LOC, "Ldm_IA", "LDMIA R0, {R1}", ConditionCode::Al, InstructionMnemonic::Ldm, MultiTransferMode::IncrementAfter, false, false, CoreRegister::R0, 0x0002u, 0xE8900002u },
        { LOC, "Stm_IB", "STMNEIB R6, {R0-R4}", ConditionCode::Ne, InstructionMnemonic::Stm, MultiTransferMode::IncrementBefore, false, false, CoreRegister::R6, 0x001Fu, 0x1986001Fu },
        { LOC, "Ldm_DA", "LDMCCDA R8, {R10,R12-R15}", ConditionCode::Cc, InstructionMnemonic::Ldm, MultiTransferMode::DecrementAfter, false, false, CoreRegister::R8, 0xF400u, 0x3818F400u },
        { LOC, "Stm_DB", "STMVSDB R1, {R0,R2-R7}", ConditionCode::Vs, InstructionMnemonic::Stm, MultiTransferMode::DecrementBefore, false, false, CoreRegister::R1, 0x00FDu, 0x690100FDu },
        { LOC, "Ldm_FA", "LDMFA R13, {R8-R11}", ConditionCode::Al, InstructionMnemonic::Ldm, MultiTransferMode::FullAscending, false, false, CoreRegister::R13, 0x0F00u, 0xE81D0F00u },
        { LOC, "Stm_FA", "STMFA R13!, {R0-R4}", ConditionCode::Al, InstructionMnemonic::Stm, MultiTransferMode::FullAscending, true, false, CoreRegister::R13, 0x001Fu, 0xE9AD001Fu },
        { LOC, "Ldm_FD", "LDMFD R13!, {R8-R11}", ConditionCode::Al, InstructionMnemonic::Ldm, MultiTransferMode::FullDescending, true, false, CoreRegister::R13, 0x0F00u, 0xE8BD0F00u },
        { LOC, "Stm_FD", "STMFD R13, {R0-R4}^", ConditionCode::Al, InstructionMnemonic::Stm, MultiTransferMode::FullDescending, false, true, CoreRegister::R13, 0x001Fu, 0xE94D001Fu },
        { LOC, "Ldm_EA", "LDMEA R13!, {R8-R11}", ConditionCode::Al, InstructionMnemonic::Ldm, MultiTransferMode::EmptyAscending, true, false, CoreRegister::R13, 0x0F00u, 0xE93D0F00u },
        { LOC, "Stm_EA", "STMEA R13, {R0-R4}", ConditionCode::Al, InstructionMnemonic::Stm, MultiTransferMode::EmptyAscending, false, false, CoreRegister::R13, 0x001Fu, 0xE88D001Fu },
        { LOC, "Ldm_ED", "LDMED R13, {R8-R11}", ConditionCode::Al, InstructionMnemonic::Ldm, MultiTransferMode::EmptyDescending, false, false, CoreRegister::R13, 0x0F00u, 0xE99D0F00u },
        { LOC, "Stm_ED", "STMED R13!, {R0-R4}^", ConditionCode::Al, InstructionMnemonic::Stm, MultiTransferMode::EmptyDescending, true, true, CoreRegister::R13, 0x001Fu, 0xE86D001Fu },
    };

    registerTestPoints<PositiveInstructionTest<MultiTransferTestPoint>>(
        "Instruction_MultiTransfer", mtrSuccess, std::size(mtrSuccess));

    SwapTestPoint swapSuccess[] = {
        { LOC, "SwapWord", "SWP R0, R1, [R2]", ConditionCode::Al, false, CoreRegister::R0, 0xE1020091u },
        { LOC, "SwapByte", "SWPB R3, R4, [R5]", ConditionCode::Al, true, CoreRegister::R3, 0xE1453094u },
        { LOC, "SwapCondition", "SWPEQ R9, R10, [R11]", ConditionCode::Eq, false, CoreRegister::R9, 0x010B909Au },
        { LOC, "SwapByteCondition", "SWPLTB R15, R0, [R1]", ConditionCode::Lt, true, CoreRegister::R15, 0xB141F090u },
    };

    registerTestPoints<PositiveInstructionTest<SwapTestPoint>>(
        "Instruction_Swap", swapSuccess, std::size(swapSuccess));

    WriteStatusRegTestPoint msrSuccess[] = {
        { LOC, "Msr_RegCPSR", "MSR CPSR, R11", 0xE12FF00Bu, ConditionCode::Al, CoreRegister::R11, true, 0x0F },
        { LOC, "Msr_RegSPSR", "MSR SPSR, R4", 0xE16FF004u, ConditionCode::Al, CoreRegister::R4, false, 0x0F },
        { LOC, "Msr_RegCPSRFlg", "MSR CPSR_F, R11", 0xE128F00Bu, ConditionCode::Al, CoreRegister::R11, true, 0x08 },
        { LOC, "Msr_RegSPSRFlg", "MSR SPSR_F, R4", 0xE168F004u, ConditionCode::Al, CoreRegister::R4, false, 0x08 },
        { LOC, "Msr_RegCPSRCtl", "MSR CPSR_C, R11", 0xE121F00Bu, ConditionCode::Al, CoreRegister::R11, true, 0x01 },
        { LOC, "Msr_RegSPSRCtl", "MSR SPSR_C, R4", 0xE161F004u, ConditionCode::Al, CoreRegister::R4, false, 0x01 },

        { LOC, "Msr_ImmedCPSR", "MSR CPSR, #&0B", 0xE32FF00Bu, ConditionCode::Al, 11, true, 0x0F },
        { LOC, "Msr_ImmedSPSR", "MSR SPSR, #&04", 0xE36FF004u, ConditionCode::Al, 4, false, 0x0F },
        { LOC, "Msr_ImmedCPSRFlg", "MSR CPSR_F, #&0B", 0xE328F00Bu, ConditionCode::Al, 11, true, 0x08 },
        { LOC, "Msr_ImmedSPSRFlg", "MSRLS SPSR_F, #&04", 0x9368F004u, ConditionCode::Ls, 4, false, 0x08 },
        { LOC, "Msr_ImmedCPSRCtl", "MSRNE CPSR_C, #&0B", 0x1321F00Bu, ConditionCode::Ne, 11, true, 0x01 },
        { LOC, "Msr_ImmedSPSRCtl", "MSR SPSR_C, #&04", 0xE361F004u, ConditionCode::Al, 4, false, 0x01 },
    };

    registerTestPoints<PositiveInstructionTest<WriteStatusRegTestPoint>>(
        "Instruction_Msr", msrSuccess, std::size(msrSuccess));

    ReadStatusRegTestPoint mrsSuccess[] = {
        { LOC, "Msr_CPSR", "MRS R11, CPSR", 0xE10FB000u, ConditionCode::Al, CoreRegister::R11, true },
        { LOC, "Msr_SPSR", "MRS R4, SPSR",  0xE14F4000u, ConditionCode::Al, CoreRegister::R4, false },
        { LOC, "Msr_CondCPSR", "MRSMI R2, CPSR", 0x410F2000u, ConditionCode::Mi, CoreRegister::R2, true },
        { LOC, "Msr_CondSPSR", "MRSPL R14, SPSR",  0x514FE000u, ConditionCode::Pl, CoreRegister::R14, false },
    };

    registerTestPoints<PositiveInstructionTest<ReadStatusRegTestPoint>>(
        "Instruction_Mrs", mrsSuccess, std::size(mrsSuccess));

    CoProcOpTestPoint cdpSuccess[] = {
        { LOC, "Cdp_Simple", "CDP CP11, 1, CR4, CR5, CR6, 2", 0xEE154B46u, InstructionMnemonic::Cdp, ConditionCode::Al, CoProcId::CP11, CoProcRegister::CR4, 1 },
        { LOC, "Cdp_Cond", "CDPPL CP0, 14, CR2, CR3, CR4, 7", 0x5EE320E4u, InstructionMnemonic::Cdp, ConditionCode::Pl, CoProcId::CP0, CoProcRegister::CR2, 14 },

        { LOC, "Mcr_Simple", "MCR CP11, 1, R4, CR5, CR6, 2", 0xEE254B56u, InstructionMnemonic::Mcr, ConditionCode::Al, CoProcId::CP11, CoProcRegister::CR4, 1 },
        { LOC, "Mcr_Cond", "MCRVS CP0, 6, R2, CR3, CR4, 7",  0x6EC320F4u, InstructionMnemonic::Mcr, ConditionCode::Vs, CoProcId::CP0, CoProcRegister::CR2, 14 },

        { LOC, "Mrc_Simple", "MRC CP11, 1, R4, CR5, CR6, 2", 0xEE354B56u, InstructionMnemonic::Mrc, ConditionCode::Al, CoProcId::CP11, CoProcRegister::CR4, 1 },
        { LOC, "Mrc_Cond", "MRCVS CP0, 6, R2, CR3, CR4, 7",  0x6ED320F4u, InstructionMnemonic::Mrc, ConditionCode::Vs, CoProcId::CP0, CoProcRegister::CR2, 14 },
    };

    registerTestPoints<PositiveInstructionTest<CoProcOpTestPoint>>(
        "Instruction_Cdp", cdpSuccess, std::size(cdpSuccess));

    CoProcDataTransferTestPoint cpDataSuccess[] = {
        { LOC, "Stc_NoOffset", "STC CP12, CR0, [R1]",              0xED810C00, false, false, ConditionCode::Al, CoProcId::CP12, CoreRegister::R0, 0, true, false },
        { LOC, "Ldc_NoOffset", "LDCL CP9, CR4, [R5]",              0xEDD54900, true, true, ConditionCode::Al, CoProcId::CP9, CoreRegister::R4, 0, true, false },
        { LOC, "Stc_PreOffset", "STCL CP2, CR0, [R1, #12]",         0xEDC10203, false, true, ConditionCode::Al, CoProcId::CP2, CoreRegister::R0, 12, true, false },
        { LOC, "Ldc_PreOffset", "LDC CP11, CR4, [R5, #96]",         0xED954B18, true, false, ConditionCode::Al, CoProcId::CP11, CoreRegister::R4, 96, true, false },
        { LOC, "Stc_PreNegOffset", "STCL CP2, CR0, [R1, #-12]",     0xED410203, false, true, ConditionCode::Al, CoProcId::CP2, CoreRegister::R0, -12, true, false },
        { LOC, "Ldc_PreNegOffset", "LDC CP11, CR4, [R5, #-96]",     0xED154B18, true, false, ConditionCode::Al, CoProcId::CP11, CoreRegister::R4, -96, true, false },
        { LOC, "Stc_PreWriteBack", "STCEQ CP2, CR0, [R1, #8]!",     0x0DA10202, false, false, ConditionCode::Eq, CoProcId::CP2, CoreRegister::R0, 8, true, true },
        { LOC, "Ldc_PreWriteBack", "LDCGT CP11, CR4, [R5, #52]!",   0xCDB54B0D, true, false, ConditionCode::Gt, CoProcId::CP11, CoreRegister::R4, 52, true, true },
        { LOC, "Stc_PostOffset", "STCL CP2, CR0, [R1], #12",        0xECC10203, false, true, ConditionCode::Al, CoProcId::CP2, CoreRegister::R0, 12, false, false },
        { LOC, "Ldc_PostOffset", "LDC CP11, CR4, [R5], #96",        0xEC954B18, true, false, ConditionCode::Al, CoProcId::CP11, CoreRegister::R4, 96, false, false },
        { LOC, "Stc_PostNegOffset", "STCLTL CP2, CR0, [R1], #-12",  0xBC410203, false, true, ConditionCode::Lt, CoProcId::CP2, CoreRegister::R0, -12, false, false },
        { LOC, "Ldc_PostNegOffset", "LDCLSL CP11, CR4, [R5], #-96", 0x9C554B18, true, true, ConditionCode::Ls, CoProcId::CP11, CoreRegister::R4, -96, false, false },
    };

    registerTestPoints<PositiveInstructionTest<CoProcDataTransferTestPoint>>(
        "Instruction_CpLdcStc", cpDataSuccess, std::size(cpDataSuccess));
}

}} // namespace Mo::Asm
////////////////////////////////////////////////////////////////////////////////

