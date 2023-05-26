# CMakeLists configuration for a OpenGL C++ 17 language bindings library.

# Define the library.
add_ag_library(GLAPI FOLDER      Graphics
                     SOURCES     ${AG_INCLUDE_DIR}/Ag/GLAPI/Platform.hpp
                                 Platform.cpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/BaseTypes.hpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/EnumTypes.hpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/APIs.hpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/Identifiers.hpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/CommandSets.hpp
                                 APIs.cpp
                                 Identifiers.cpp
                                 CommandSets.cpp
                                 ExtensionSet.cpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/ExtensionSet.hpp
                                 APIResolvers.cpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/APIResolvers.hpp
                                 OpenGLException.cpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/OpenGLException.hpp
                     WIN_SOURCES WGL_API.cpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/WGL_API.hpp
                                 WGL_CommandSets.cpp
                                 ${AG_INCLUDE_DIR}/Ag/GLAPI/WGL_CommandSets.hpp
                    HEADERS      ${AG_INCLUDE_DIR}/Ag/GLAPI.hpp)
