# The CMake configuration for the Silver (Ag) runtime utility library.
set(AG_VERSION "0.0.1.0")

if (PROJECT_IS_TOP_LEVEL)
    project(Silver
            VERSION "${AG_VERSION}"
            DESCRIPTION "An cross platform runtime library."
            HOMEPAGE_URL "https://github.com/GiantRobotLemur/Ag"
            LANGUAGES CXX)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/cmake/ConfigureBuild.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/Utilities.cmake")


if (PROJECT_IS_TOP_LEVEL)
    set(ExtFolderName "3rdParty")

    message(STATUS "Obtaining Google Test...")
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
    set_target_properties(gtest PROPERTIES FOLDER "${ExtFolderName}")
    set_target_properties(gtest_main PROPERTIES FOLDER "${ExtFolderName}")
    set_target_properties(gmock PROPERTIES FOLDER "${ExtFolderName}")
    set_target_properties(gmock_main PROPERTIES FOLDER "${ExtFolderName}")

    # message(STATUS "Obtaining glm...")
    # FetchContent_MakeAvailable(glm)
    # set_target_properties(glm PROPERTIES FOLDER "${ExtFolderName}")
    # target_link_libraries(${myTarget} PRIVATE glm::glm)
endif()

# Add the location of public header files.
set(AG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")

# Underlying runtime components.
add_subdirectory(SymbolPackager)
add_subdirectory(Core)

if ("${Qt6_FOUND}")
    add_subdirectory(QtInterop)
endif()