# CMakeLists configuration for a C++ 17 OpenGL wrapper library.

# Define the library.
add_ag_library(ObjectGL FOLDER      Graphics
                        SOURCES     DisplayFormat.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/DisplayFormat.hpp
                                    ContextOptions.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/ContextOptions.hpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/Driver.hpp
                                    DisplayContextPrivate.cpp
                                    DisplayContextPrivate.hpp
                                    DisplayContext.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/DisplayContext.hpp
                                    RenderContextPrivate.cpp
                                    RenderContextPrivate.hpp
                                    RenderContext.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/RenderContext.hpp
                                    SDL2Driver.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/SDL2Driver.hpp
                                    ResourceWrappers.cpp
                                    ResourceWrappers.hpp
                                    Shader.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/Shader.hpp
                                    Program.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/Program.hpp
                                    ProgramBuilder.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/ProgramBuilder.hpp
                                    Buffer.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/Buffer.hpp
                                    VertexSchema.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/VertexSchema.hpp
                                    VertexBuffer.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/VertexBuffer.hpp
                                    IndexBuffer.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/IndexBuffer.hpp
                                    VertexArrayObject.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/VertexArrayObject.hpp
                        WIN_SOURCES ${AG_INCLUDE_DIR}/Ag/ObjectGL/Win32API.hpp
                                    WGL_Tools.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/WGL_Tools.hpp
                                    WGLDriver.cpp
                                    ${AG_INCLUDE_DIR}/Ag/ObjectGL/WGLDriver.hpp
                        HEADERS     ${AG_INCLUDE_DIR}/Ag/ObjectGL.hpp
                        PUBLIC_LIBS GLAPI glm::glm SDL2::SDL2)
