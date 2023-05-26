# CMakeLists file for the SdlTools library.

add_ag_library(SdlTools FOLDER      "Core"
                        HEADERS     ${AG_INCLUDE_DIR}/Ag/SdlTools.hpp
                        SOURCES     ${AG_INCLUDE_DIR}/Ag/SdlTools/WrapperTypes.hpp
                                    WrapperTypes.cpp
                                    ${AG_INCLUDE_DIR}/Ag/SdlTools/ApiException.hpp
                                    ApiException.cpp
                                    ${AG_INCLUDE_DIR}/Ag/SdlTools/Initialiser.hpp
                                    Initialiser.cpp
                                    ${AG_INCLUDE_DIR}/Ag/SdlTools/InputManager.hpp
                                    InputManager.cpp
                                    ${AG_INCLUDE_DIR}/Ag/SdlTools/OpenGL.hpp
                                    OpenGL.cpp
                        PUBLIC_LIBS AgCore SDL2::SDL2)