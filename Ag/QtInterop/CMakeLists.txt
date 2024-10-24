# CMakeLists file for the QtInterop library.

set(QT_INTEROP_PATH "${AG_INCLUDE_DIR}/Ag/QtInterop")

add_ag_library(QtInterop    QT
                            FOLDER      "Core"
                            HEADERS     ${AG_INCLUDE_DIR}/Ag/QtInterop.hpp
                            SOURCES     ${QT_INTEROP_PATH}/CommandLineWrangler.hpp
                                        CommandLineWrangler.cpp
                                        ${QT_INTEROP_PATH}/Conversion.hpp
                                        Conversion.cpp
                            PUBLIC_LIBS AgCore Qt6::Core)