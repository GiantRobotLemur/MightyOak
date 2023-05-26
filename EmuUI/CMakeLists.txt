# CMakeLists file for the EmuUi library.

add_ag_library(EmuUI FOLDER         "Emulator"
                     HEADERS        ${MO_INCLUDE_DIR}/EmuUi.hpp
                     SOURCES        ${MO_INCLUDE_DIR}/EmuUi/MainWindow.hpp
                                    MainWindow.cpp
                     PUBLIC_LIBS    AgCore SdlTools ObjectGL)