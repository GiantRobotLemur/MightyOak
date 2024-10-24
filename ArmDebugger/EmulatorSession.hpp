//! @file ArmDebugger/EmulatorSession.hpp
//! @brief The declaration of an object which manages the running emulated machine.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_EMULATOR_SESSION_HPP__
#define __ARM_DEBUGGER_EMULATOR_SESSION_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QFutureWatcher>
#include <QObject>
#include <QTimer>

#include "ArmEmu/EmuOptions.hpp"
#include "ArmEmu/ArmSystem.hpp"

#include "Breakpoint.hpp"
#include "EmulatorIOAdapter.hpp"
#include "SessionSettings.hpp"

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines the current state of the emulated system.
enum class EmulatorState
{
    //! @brief No emulated system has been created, nor is one configured.
    Uninitialised,

    //! @brief The emulated system has been created, but isn't currently running.
    Paused,

    //! @brief The emulated system has been created and is running.
    Running,

    //! @brief The emulated system is created but has stopped, it could be
    //! re-created.
    Stopped,
};

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which manages the running emulated machine.
class EmulatorSession : public QObject
{
Q_OBJECT
public:
    // Construction/Destruction
    EmulatorSession(QObject *owner);
    virtual ~EmulatorSession();

    // Accessors
    bool isInitialised() const;
    EmulatorState getState() const;
    Arm::IArmSystem *getEmulator();
    const Arm::IArmSystem *getEmulator() const;
    const Arm::Options &getSessionOptions() const;
    SessionSettings &getSettings();
    const SessionSettings &getSettings() const;
    const BreakpointCollection &getBreakpoints() const;

    // Operations
    uint16_t addBreakpoint(uint32_t address, bool isLogicalAddress);
    void removeBreakpoint(uint16_t id);
    bool toggleBreakpoint(uint32_t address, bool isLogicalAddress);
    bool setBreakpointState(uint16_t id, bool isEnabled);
    bool tryFindBreakpoint(uint32_t address, bool isLogicalAddress, uint16_t &id) const;

    void create(const Arm::Options &options);

public slots:
    void destroy();
    void restart();
    void step();
    void stepOver();
    void stepOut();
    void resume();
    void pause();
    void stop();

signals:
    void sessionStarted(const Arm::Options &options, Arm::IArmSystem *emulator);
    void sessionEnded(Arm::IArmSystem *emulator);
    void sessionPaused(Arm::IArmSystem *emulator);
    void sessionResumed(Arm::IArmSystem *emulator);
    void sessionSingleStep(Arm::IArmSystem *emulator);
    void breakpointsChanged(const EmulatorSession *session);

private slots:
    void onPollEmulator();
    void onExecutionComplete();
private:
    // Internal Types
    using QEmulatorFuture = QFuture<Arm::ExecutionMetrics>;
    using QEmulatorFutureWatcher = QFutureWatcher<Arm::ExecutionMetrics>;

    // Internal Functions
    bool tryFindBreakpointIndex(uint32_t address, size_t &index) const;
    void beginPollingEmulator();
    void endPollingEmulator();

    // Internal Fields
    QTimer _emulatorPollTimer;
    QEmulatorFutureWatcher _emulatorWatcher;
    Arm::IArmSystemUPtr _emulator;
    IEmulatorIOAdapterUPtr _ioAdapter;
    BreakpointCollection _breakpoints;
    SessionSettings _settings;
    EmulatorState _state;
    Breakpoint _stepBreakpoint;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
