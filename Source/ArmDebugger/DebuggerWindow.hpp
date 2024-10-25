//! @file ArmDebugger/UI/DebuggerWindow.hpp
//! @brief The declaration of an object representing the main application window.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_WINDOW_HPP__
#define __ARM_DEBUGGER_WINDOW_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QMainWindow>

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
class QComboBox;
class QLineEdit;

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
class DebuggerApp;
class MemoryViewWidget;
class RegisterViewWidget;
class OutputViewWidget;

namespace Arm {
class IArmSystem;
}

//! @brief An object representing the main application window.
class DebuggerWindow : public QMainWindow
{
Q_OBJECT
public:
    // Construction/Destruction
    DebuggerWindow();
    virtual ~DebuggerWindow() = default;

    // Accessors

    // Operations

    // Overrides
protected:
    virtual void closeEvent(QCloseEvent *args) override;

private slots:
    // Signal Handlers
    void onSessionStarted(const Arm::Options &options, Arm::IArmSystem *emulator);
    void onSessionEnded(Arm::IArmSystem *emulator);
    void onMemoryRegionSelected(int regionIndex);
    //void onRegisterVisibilityToggled(bool isShown);
    //void onOutputVisibilityToggled(bool isShown);
private:
    // Internal Types

    // Internal Functions
    void setupWindow();
    void setupWidgets(DebuggerApp *app);
    void setupMenus(DebuggerApp *app);

    // Internal Fields
    QDockWidget *_registersDock;
    QDockWidget *_outputDock;
    MemoryViewWidget *_memoryView;
    RegisterViewWidget *_registersView;
    OutputViewWidget *_outputView;
    QComboBox *_memoryRegionList;
    QLineEdit *_gotoAddrField;
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
