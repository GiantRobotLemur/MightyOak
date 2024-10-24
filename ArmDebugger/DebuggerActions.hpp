//! @file ArmDebugger/DebuggerActions.hpp
//! @brief The declaration of identifiers of actions used defined by the
//! application.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_ACTIONS_HPP__
#define __ARM_DEBUGGER_ACTIONS_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief Defines identifies for actions in the application.
enum class Action : uint32_t
{
    Exit,
    NewSession,
    OpenSession,
    SaveSession,
    SaveSessionAs,

    EditLabels,
    EditSWIs,

    EditMemoryDisplayOptions,

    ResumeSession,
    PauseSession,
    StopSession,
    RestartSession,

    ExecOver,
    ExecInto,
    ExecOut,

    GotoPC,

    About,
};

//! @brief Defines groups for actions in the application.
enum class ActionGroup : uint32_t
{
    Application,
    Session,
    Debug,
    CodeView,
    Help,
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