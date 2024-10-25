//! @file ArmDebugger/EmulatorSession.cpp
//! @brief The definition of an object which manages the running emulated machine.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "EmulatorSession.hpp"

#ifdef _MSC_VER
// Disable compiler warning C4127: conditional expression is constant.
// This is produced by including QtConcurrent.
#pragma warning(disable : 4127)
#endif

#include <QtConcurrent>
#include <QMessageBox>

#include "Ag/QtInterop/Conversion.hpp"
#include "ArmEmu/ArmSystemBuilder.hpp"
#include "ArmEmu/GuestEventQueue.hpp"

#include "DebuggerApp.hpp"
#include "MemcIOAdapter.hpp"
#include "Tools.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
struct EqualsBreakpointID
{
private:
    uint16_t _matchID;
public:
    EqualsBreakpointID(uint16_t id) : _matchID(id) {}

    bool operator()(const Breakpoint &rhs) const
    {
        return rhs.getBreakpointID() == _matchID;
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
constexpr uint16_t TempBreakpointID = 0xF000;
constexpr uint16_t SeedBreakpointID = 0xF001;

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
Arm::ExecutionMetrics runEmulator(Arm::IArmSystem *emulator)
{
    return emulator->run();
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// EmulatorSession Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
EmulatorSession::EmulatorSession(QObject *owner) :
    QObject(owner),
    _emulatorPollTimer(this),
    _emulatorWatcher(this),
    _state(EmulatorState::Uninitialised)
{
    connect(&_emulatorPollTimer, &QTimer::timeout,
            this, &EmulatorSession::onPollEmulator);
    connect(&_emulatorWatcher, &QEmulatorFutureWatcher::finished,
            this, &EmulatorSession::onExecutionComplete);

    _emulatorPollTimer.setSingleShot(false);
    _emulatorPollTimer.setInterval(1);

    // DEBUG: Preset a valid config for quick testing.
    auto sessionOptions = _settings.getEmulatorOptions();
    sessionOptions.setHardwareArchitecture(Arm::SystemModel::Archimedies);
    sessionOptions.setProcessorVariant(Arm::ProcessorModel::ARM2);
    sessionOptions.setProcessorSpeedMHz(8);
    sessionOptions.setRamSizeKb(1024);
    sessionOptions.setSystemRom(Arm::SystemROMPreset::Custom);

    Ag::Fs::PathBuilder romPath;
    romPath.assignProgramDirectory();
    romPath.popElement();
    romPath.pushElement("ArmEmu");
    romPath.pushElement("MemcTestRom.bin");

    sessionOptions.setCustomRom(romPath);

    _settings.setEmulatorOptions(sessionOptions);
}

EmulatorSession::~EmulatorSession()
{
    // Ensure any outstanding session is ended.
    destroy();
}

bool EmulatorSession::isInitialised() const { return static_cast<bool>(_emulator); }
EmulatorState EmulatorSession::getState() const { return _state; }

Arm::IArmSystem *EmulatorSession::getEmulator() { return _emulator.get(); }
const Arm::IArmSystem *EmulatorSession::getEmulator() const { return _emulator.get(); }
const Arm::Options &EmulatorSession::getSessionOptions() const { return _settings.getEmulatorOptions(); }

SessionSettings &EmulatorSession::getSettings() { return _settings; }
const SessionSettings &EmulatorSession::getSettings() const { return _settings; }

const BreakpointCollection &EmulatorSession::getBreakpoints() const
{
    return _breakpoints;
}

uint16_t EmulatorSession::addBreakpoint(uint32_t address, bool isLogicalAddress)
{
    uint16_t id = 0;

    if (_emulator)
    {
        // Allocate a breakpoint ID.
        std::vector<uint16_t> usedBreakpointIDs;
        usedBreakpointIDs.reserve(_breakpoints.size());

        for (const auto &bkpt : _breakpoints)
        {
            usedBreakpointIDs.push_back(bkpt.getBreakpointID());
        }

        std::sort(usedBreakpointIDs.begin(), usedBreakpointIDs.end());

        bool foundID = false;
        id = SeedBreakpointID - 1;

        do
        {
            ++id;

            foundID = std::binary_search(usedBreakpointIDs.begin(),
                                       usedBreakpointIDs.end(), id) == false;
        } while ((foundID == false) && (id > 0));

        if (id == 0)
        {
            QMessageBox::warning(nullptr, tr("Breakpoint Failure"),
                                 tr("Too many breakpoints set already."),
                                 QMessageBox::Ok);
        }
        else
        {
            Breakpoint breakpoint(_emulator.get(), address, id, isLogicalAddress);

            auto pos = std::lower_bound(_breakpoints.begin(), _breakpoints.end(),
                                        breakpoint, Breakpoint::CompareByAddress());

            if (pos != _breakpoints.end())
            {
                if (pos->getAddress() == address)
                {
                    // Ensure the breakpoint is unset before overwriting.
                    id = pos->getBreakpointID();
                }
                else
                {
                    // Insert the new breakpoint;
                    pos = _breakpoints.insert(pos, breakpoint);
                }

                pos->apply();
            }
            else
            {
                _breakpoints.push_back(breakpoint);

                _breakpoints.back().apply();
            }

            emit breakpointsChanged(this);
        }
    }

    return id;
}

void EmulatorSession::removeBreakpoint(uint16_t id)
{
    auto pos = std::find_if(_breakpoints.begin(), _breakpoints.end(),
                            EqualsBreakpointID(id));

    if (pos != _breakpoints.end())
    {
        pos->remove();
        _breakpoints.erase(pos);

        emit breakpointsChanged(this);
    }
}

bool EmulatorSession::toggleBreakpoint(uint32_t address, bool isLogicalAddress)
{
    bool isFound = false;
    Breakpoint key(nullptr, address, 0, isLogicalAddress);

    auto pos = std::lower_bound(_breakpoints.begin(), _breakpoints.end(),
                                key, Breakpoint::CompareByAddress());

    if ((pos != _breakpoints.end()) &&
        (pos->getAddress() == address) &&
        (pos->isLogicalAddress() == isLogicalAddress))
    {
        isFound = true;

        if (pos->isEnabled())
        {
            pos->remove();
            _breakpoints.erase(pos);
        }
        else
        {
            pos->apply();
        }

        emit breakpointsChanged(this);
    }
    else
    {
        addBreakpoint(address, isLogicalAddress);

        emit breakpointsChanged(this);

        isFound = true;
    }

    return isFound;
}

bool EmulatorSession::setBreakpointState(uint16_t id, bool isEnabled)
{
    bool isStateSet = false;

    auto pos = std::find_if(_breakpoints.begin(), _breakpoints.end(),
                            EqualsBreakpointID(id));

    if (pos != _breakpoints.end())
    {
        if (isEnabled)
        {
            isStateSet = pos->apply();
        }
        else
        {
            pos->remove();
            isStateSet = true;
        }

        if (isStateSet)
            emit breakpointsChanged(this);
    }

    return isStateSet;
}

bool EmulatorSession::tryFindBreakpoint(uint32_t address, bool isLogicalAddress,
                                        uint16_t &id) const
{
    bool isFound = false;
    Breakpoint key(nullptr, address, 0, isLogicalAddress);
    id = 0;

    auto pos = std::lower_bound(_breakpoints.begin(), _breakpoints.end(),
                                key, Breakpoint::CompareByAddress());

    if ((pos != _breakpoints.end()) &&
        (pos->getAddress() == address) &&
        (pos->isLogicalAddress() == isLogicalAddress))
    {
        isFound = true;
        id = pos->getBreakpointID();
    }

    return isFound;
}

void EmulatorSession::create(const Arm::Options &options)
{
    // Dispose of the previous emulator?
    destroy();

    try
    {
        Arm::ArmSystemBuilder builder(options);

        _emulator = builder.createSystem();
        _settings.setEmulatorOptions(options);
        _state = EmulatorState::Paused;

        switch (options.getHardwareArchitecture())
        {
        case Arm::SystemModel::Archimedies:
        case Arm::SystemModel::ASeries:
            _ioAdapter = std::make_unique<MemcIOAdapter>(_emulator.get(),
                                                         _settings.getEmulatorOptions());
            break;
        }

        emit sessionStarted(options, _emulator.get());
    }
    catch (const Ag::Exception &error)
    {
        reportError(nullptr, tr("Emulator Error"), error);
    }
}

void EmulatorSession::destroy()
{
    if (_emulator)
    {
        // Stop the running system, if any.
        if (_emulator->isRunning())
        {
            _emulator->raiseHostInterrupt();

            // Wait for emulator to stop running.
            _emulatorWatcher.waitForFinished();
        }

        _breakpoints.clear();

        // Inform listeners the session has ended.
        emit sessionEnded(_emulator.get());

        _state = EmulatorState::Uninitialised;
        _ioAdapter.reset();
        _emulator.reset();
    }
}

void EmulatorSession::restart()
{
    if (_emulator)
    {
        // Store the state to be inherited by the new session.
        auto previousOptions = _settings.getEmulatorOptions();
        BreakpointCollection oldBreakpoints(std::move(_breakpoints));

        // Dispose of the previous session.
        destroy();

        // Create a new session using the same options.
        create(previousOptions);

        // Restore the old breakpoints in the new system.
        _breakpoints.reserve(oldBreakpoints.size());

        for (const auto &oldBreakpoint : oldBreakpoints)
        {
            auto &bkpt = _breakpoints.emplace_back(_emulator.get(),
                                                   oldBreakpoint.getAddress(),
                                                   oldBreakpoint.getBreakpointID(),
                                                   oldBreakpoint.isLogicalAddress());
            if (oldBreakpoint.isEnabled() == false)
            {
                bkpt.remove();
            }
        }
    }
}

void EmulatorSession::step()
{
    if (_emulator && (_state == EmulatorState::Paused))
    {
        _emulator->runSingleStep();

        // Drain the guest event queue.
        onPollEmulator();

        emit sessionSingleStep(_emulator.get());
    }
}

void EmulatorSession::stepOver()
{
    if (_emulator && (_state == EmulatorState::Paused))
    {
        // TODO: Set a breakpoint at the next instruction and run until it is
        // triggered. Then remove the breakpoint and step the PC back.
    }
}

void EmulatorSession::stepOut()
{
    if (_emulator && (_state == EmulatorState::Paused))
    {
        // TODO: Determine the return address and set a breakpoint.
        // Run to the breakpoint, remove it and step the PC back.
    }
}

void EmulatorSession::resume()
{
    if (_emulator && (_state == EmulatorState::Paused))
    {
        emit sessionResumed(_emulator.get());

        // Determine if we are at a breakpoint which we need to step past.
        uint32_t currentPC = _emulator->getCoreRegister(Arm::CoreRegister::PC);
        size_t index;

        if (tryFindBreakpointIndex(currentPC, index) &&
            _breakpoints[index].isEnabled())
        {
            // We are on a breakpoint, restore the original instruction and
            // execute it before continuing.
            auto &currentBreakpoint = _breakpoints[index];
            currentBreakpoint.remove();

            _emulator->runSingleStep();
            currentBreakpoint.apply();
        }

        beginPollingEmulator();

        // Set the emulator to run without restriction.
        QEmulatorFuture pendingResult = QtConcurrent::run(runEmulator, _emulator.get());
        _emulatorWatcher.setFuture(pendingResult);
    }
}

void EmulatorSession::pause()
{
    if (_emulator && (_state == EmulatorState::Running))
    {
        // Raise a host interrupt in the emulator.
        _emulator->raiseHostInterrupt();

        // Wait for emulator to stop.
        _emulatorWatcher.waitForFinished();

        // Stop polling for event, but drain the guest event queue.
        endPollingEmulator();
        onPollEmulator();
    }
}

void EmulatorSession::stop()
{
    if (_emulator && (_state != EmulatorState::Stopped))
    {
        if (_state == EmulatorState::Running)
        {
            pause();
        }

        _state = EmulatorState::Stopped;

        // Undo all break points so that the memory is in the correct state.
        for (auto &breakpoint : _breakpoints)
        {
            breakpoint.remove();
        }
    }
}

void EmulatorSession::onPollEmulator()
{
    if (_emulator)
    {
        // Drain the emulator guest event queue.
        Arm::GuestEvent emulatorEvent;

        while (_emulator->tryGetNextMessage(emulatorEvent))
        {
            // Process guest event in the main thread.
            if (_ioAdapter)
                _ioAdapter->handleGuestEvent(emulatorEvent);
        }
    }
}

void EmulatorSession::onExecutionComplete()
{
    if (_emulator)
    {
        _state = EmulatorState::Paused;

        // Stop polling for emulator events and drain the queue.
        _emulatorPollTimer.stop();
        onPollEmulator();

        size_t breakpointIndex;
        uint32_t lastPC = _emulator->getCoreRegister(Arm::CoreRegister::PC) - 4;

        // If we stopped after executing a breakpoint, disable it and step
        // back so that next time we execute the actual instruction.
        if (tryFindBreakpointIndex(lastPC, breakpointIndex))
        {
            _breakpoints[breakpointIndex].remove();
            _emulator->setCoreRegister(Arm::CoreRegister::PC, lastPC);
        }
        else if (_stepBreakpoint.getAddress() == lastPC)
        {
            _emulator->setCoreRegister(Arm::CoreRegister::PC, lastPC);
        }

        // Ensure any temporary breakpoint applied for a step command
        // is disabled.
        _stepBreakpoint.remove();

        emit sessionPaused(_emulator.get());
    }
}

bool EmulatorSession::tryFindBreakpointIndex(uint32_t address,
                                             size_t &index) const
{
    bool isFound = false;
    Breakpoint key(nullptr, address, 0, true);

    // Try searching by logical address first.
    auto pos = std::lower_bound(_breakpoints.begin(), _breakpoints.end(),
                                key, Breakpoint::CompareByAddress());

    if ((pos != _breakpoints.end()) &&
        (pos->getAddress() == address) &&
        (pos->isLogicalAddress() == true))
    {
        isFound = true;
        index = std::distance(_breakpoints.begin(), pos);
    }
    else
    {
        // Try searching physical addresses.
        key = Breakpoint(nullptr, address, 0, false);

        pos = std::lower_bound(_breakpoints.begin(), _breakpoints.end(),
                               key, Breakpoint::CompareByAddress());

        if ((pos != _breakpoints.end()) &&
            (pos->getAddress() == address) &&
            (pos->isLogicalAddress() == false))
        {
            isFound = true;
            index = std::distance(_breakpoints.begin(), pos);
        }
    }

    return isFound;
}

void EmulatorSession::beginPollingEmulator()
{
    if (_emulator)
    {
        _emulatorPollTimer.start();
    }
}

void EmulatorSession::endPollingEmulator()
{
    _emulatorPollTimer.stop();
}

////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

