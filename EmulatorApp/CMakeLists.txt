# CMakeLists file for the Mighty Oak emulator application project.

add_gui_app(EmuApp NAME         "MightyOak"
                   DESCRIPTION  "An Archimedes and RiscPC emulator."
                   VERSION      "${PROJECT_VERSION}"
                   FOLDER       "Emulator"
                   SOURCES      Main.cpp
                                CommandLineOptions.cpp
                                CommandLineOptions.hpp
                                App.cpp
                                App.hpp
                   LIBS         AgCore EmuUI)