# A standard CMake configuration for C++ which can be used by all consuming projects.

if (NOT DEFINED AG_BUILD_CONFIGURED)
    set(AG_BUILD_CONFIGURED 1)

    macro(ag_configure_build)
        enable_testing()

        set_property(GLOBAL PROPERTY USE_FOLDERS ON)

        set(CMAKE_CXX_STANDARD          17)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
        set(CMAKE_CXX_EXTENSIONS        ON)

        get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

        if(isMultiConfig)
            # Limit build configurations to Release with Debug Info and Debug.
            set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo")
            message(STATUS "Configuring multi-configuration build...")
        endif()

        # Configure global compiler options.
        if (DEFINED MSVC)
            add_compile_options("/W4")
        else()
            add_compile_options(-Wall -Wextra -pedantic -Wshadow)

            # Ensure _DEBUG is defined for debug builds, this is done automatically
            # for MSVC.
            add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
        endif()

        include(FetchContent)

        FetchContent_Declare(googletest
                             GIT_REPOSITORY https://github.com/google/googletest.git
                             GIT_TAG release-1.12.1
                             CMAKE_CACHE_ARGS -Dgtest_force_shared_crt:BOOL=ON -DBUILD_GMOCK:BOOL=OFF -DINSTALL_GTEST:BOOL=OFF
                             FIND_PACKAGE_ARGS NAMES GTest
                             )

        FetchContent_Declare(glm
                             GIT_REPOSITORY https://github.com/g-truc/glm.git
                             GIT_TAG 1.0.1
                             CMAKE_CACHE_ARGS -DGLM_BUILD_TESTS:BOOL=OFF -DBUILD_SHARED_LIBS:BOOL=OFF
                             FIND_PACKAGE_ARGS NAMES glm
                             )
    endmacro()

endif()