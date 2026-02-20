# CMakeLists file for the Mighty Oak emulator application.

ag_add_gui_app( MightyOak
                NAME        "MightyOak"
                DESCRIPTION "An Archimedes and RiscPC emulator."
                AUTHOR      "${MO_AUTHOR}"
                COPYRIGHT   "${MO_COPYRIGHT}"
                VERSION     "${PROJECT_VERSION}"
                FOLDER      ARM
                SOURCES     "Main.cpp"
                LIBS        MightyOakLib)

# Add the current directory so that the PCH can be found when referenced by
# PCH-generating source files in the build tree.
target_include_directories(MightyOak PRIVATE "${MO_INCLUDE_DIR}")

# Copy ROM files to the output directory alongside the executable.
set(MO_ROM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../MightyOakLib/Roms")

file(GLOB MO_ROM_FILES "${MO_ROM_SOURCE_DIR}/*.rom")

if(MO_ROM_FILES)
    add_custom_command(TARGET MightyOak POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:MightyOak>/ROMs"
        COMMENT "Creating ROMs directory...")

    foreach(ROM_FILE ${MO_ROM_FILES})
        get_filename_component(ROM_FILENAME "${ROM_FILE}" NAME)

        add_custom_command(TARGET MightyOak POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${ROM_FILE}"
                    "$<TARGET_FILE_DIR:MightyOak>/ROMs/${ROM_FILENAME}"
            COMMENT "Copying ${ROM_FILENAME}...")
    endforeach()
endif()