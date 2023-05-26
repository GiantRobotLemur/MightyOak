# CMakeLists file for the EmuUi library.

add_ag_library(EmuUI FOLDER         "Emulator"
                     HEADERS        ${MO_INCLUDE_DIR}/EmuUI.hpp
                     SOURCES        ${MO_INCLUDE_DIR}/EmuUI/MainWindow.hpp
                                    MainWindow.cpp
                     PUBLIC_LIBS    AgCore SdlTools ObjectGL)