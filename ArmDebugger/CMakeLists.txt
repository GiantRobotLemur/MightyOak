# CMakeLists file for the Mighty Oak interactive debugger application project.

add_gui_app(ArmDebugger QT
                        NAME        ArmDebugger
                        DESCRIPTION "An interactive debugger for the Mighty Oak Archimedes and RiscPC emulator."
                        VERSION     ${PROJECT_VERSION}
                        FOLDER      ARM
                        SOURCES     Main.cpp
                                    Tools.cpp
                                    Tools.hpp
                                    CommandLineOptions.cpp
                                    CommandLineOptions.hpp
                                    ActionManager.cpp
                                    ActionManager.hpp
                                    DebuggerActions.hpp
                                    DebuggerApp.cpp
                                    DebuggerApp.hpp
                                    Breakpoint.cpp
                                    Breakpoint.hpp
                                    EmulatorIOAdapter.hpp
                                    MemcIOAdapter.cpp
                                    MemcIOAdapter.hpp
                                    EmulatorSession.cpp
                                    EmulatorSession.hpp
                                    DebuggerWindow.cpp
                                    DebuggerWindow.hpp
                                    UI/AboutDialog.ui
                                    UI/AboutDialog.cpp
                                    UI/AboutDialog.hpp
                                    UI/NewSessionDialog.ui
                                    UI/NewSessionDialog.cpp
                                    UI/NewSessionDialog.hpp
                                    UI/MemoryBlockView.cpp
                                    UI/MemoryBlockView.hpp
                                    UI/CodeBlockView.cpp
                                    UI/CodeBlockView.hpp
                                    UI/UnmappedBlockView.cpp
                                    UI/UnmappedBlockView.hpp
                                    UI/UniformBlockView.cpp
                                    UI/UniformBlockView.hpp
                                    UI/DataBlockView.cpp
                                    UI/DataBlockView.hpp
                                    UI/LabelBlockView.cpp
                                    UI/LabelBlockView.hpp
                                    UI/MemoryViewWidget.cpp
                                    UI/MemoryViewWidget.hpp
                                    UI/MemoryViewBar.cpp
                                    UI/MemoryViewBar.hpp
                                    UI/RegisterViewWidget.cpp
                                    UI/RegisterViewWidget.hpp
                                    UI/RegisterViewWidget.ui
                                    UI/OutputViewWidget.cpp
                                    UI/OutputViewWidget.hpp
                                    UI/EditOrdinalMappingDialog.cpp
                                    UI/EditOrdinalMappingDialog.hpp
                                    UI/EditOrdinalMappingDialog.ui
                                    UI/EditDisplayOptionsDialog.cpp
                                    UI/EditDisplayOptionsDialog.hpp
                                    UI/EditDisplayOptionsDialog.ui
                                    MemoryViewOptions.cpp
                                    MemoryViewOptions.hpp
                                    SessionSettings.cpp
                                    SessionSettings.hpp
                                    Annotations/Annotation.cpp
                                    Annotations/Annotation.hpp
                                    Annotations/DataReplacement.cpp
                                    Annotations/DataReplacement.hpp
                                    Annotations/InstructionFormat.cpp
                                    Annotations/InstructionFormat.hpp
                                    Annotations/TypeOverride.cpp
                                    Annotations/TypeOverride.hpp

                        LIBS        QtInterop ArmEmu AsmTools Qt::Widgets Qt::Concurrent)

# Specifically allow files in sub-folders to resolve include files above.
target_include_directories(ArmDebugger PRIVATE "${CMAKE_CURRENT_SRC_DIR}")

# Group the files into folders in the IDE.
source_group(Application FILES  Main.cpp
                                CommandLineOptions.cpp
                                CommandLineOptions.hpp
                                ActionManager.cpp
                                ActionManager.hpp
                                DebuggerActions.hpp
                                DebuggerApp.cpp
                                DebuggerApp.hpp
                                Breakpoint.cpp
                                Breakpoint.hpp
                                EmulatorIOAdapter.hpp
                                MemcIOAdapter.cpp
                                MemcIOAdapter.hpp
                                EmulatorSession.cpp
                                EmulatorSession.hpp
                                MemoryViewOptions.cpp
                                MemoryViewOptions.hpp
                                SessionSettings.cpp
                                SessionSettings.hpp)

source_group(UI FILES   DebuggerWindow.cpp
                        DebuggerWindow.hpp
                        UI/AboutDialog.ui
                        UI/AboutDialog.cpp
                        UI/AboutDialog.hpp
                        UI/NewSessionDialog.ui
                        UI/NewSessionDialog.cpp
                        UI/NewSessionDialog.hpp
                        UI/MemoryBlockView.cpp
                        UI/MemoryBlockView.hpp
                        UI/CodeBlockView.cpp
                        UI/CodeBlockView.hpp
                        UI/DataBlockView.cpp
                        UI/DataBlockView.hpp
                        UI/LabelBlockView.cpp
                        UI/LabelBlockView.hpp
                        UI/UnmappedBlockView.cpp
                        UI/UnmappedBlockView.hpp
                        UI/UniformBlockView.cpp
                        UI/UniformBlockView.hpp
                        UI/MemoryViewWidget.cpp
                        UI/MemoryViewWidget.hpp
                        UI/MemoryViewBar.cpp
                        UI/MemoryViewBar.hpp
                        UI/RegisterViewWidget.cpp
                        UI/RegisterViewWidget.hpp
                        UI/RegisterViewWidget.ui
                        UI/OutputViewWidget.cpp
                        UI/OutputViewWidget.hpp
                        UI/EditOrdinalMappingDialog.cpp
                        UI/EditOrdinalMappingDialog.hpp
                        UI/EditOrdinalMappingDialog.ui
                        UI/EditDisplayOptionsDialog.cpp
                        UI/EditDisplayOptionsDialog.hpp
                        UI/EditDisplayOptionsDialog.ui)

source_group(Misc FILES Tools.cpp
                        Tools.hpp)

source_group(Annotations FILES  Annotations/Annotation.cpp
                                Annotations/Annotation.hpp
                                Annotations/DataReplacement.cpp
                                Annotations/DataReplacement.hpp
                                Annotations/InstructionFormat.cpp
                                Annotations/InstructionFormat.hpp
                                Annotations/TypeOverride.cpp
                                Annotations/TypeOverride.hpp)

qt_add_resources(ArmDebugger images
                 PREFIX /
                 FILES  images/ArmDebugger.svg
                        images/NewSession.svg
                        images/OpenSession.svg
                        images/SaveSession.svg
                        images/SaveSessionAs.svg
                        images/Exit.svg
                        images/ResumeSession.svg
                        images/PauseSession.svg
                        images/StopSession.svg
                        images/RestartSession.svg
                        images/ExecInto.svg
                        images/ExecOut.svg
                        images/ExecOver.svg
                        images/GotoPC.svg
                        images/AddItem.svg
                        images/RemoveItem.svg)

source_group(Resources FILES    images/ArmDebugger.svg
                                images/NewSession.svg
                                images/OpenSession.svg
                                images/SaveSession.svg
                                images/SaveSessionAs.svg
                                images/Exit.svg
                                images/ResumeSession.svg
                                images/PauseSession.svg
                                images/StopSession.svg
                                images/RestartSession.svg
                                images/ExecInto.svg
                                images/ExecOut.svg
                                images/ExecOver.svg
                                images/GotoPC.svg
                                images/AddItem.svg
                                images/RemoveItem.svg)

if (DEFINED WIN32)
    target_sources(ArmDebugger PRIVATE IconResource.rc Images/ArmDebugger.ico)
    source_group(Resources FILES IconResource.rc Images/ArmDebugger.ico)
endif()

