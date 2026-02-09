# CMakeLists file for the Mighty Oak emulator application.

ag_add_gui_app( MightyOak
                NAME        "MightyOak"
                DESCRIPTION "An Archimedes and RiscPC emulator."
                VERSION     "${PROJECT_VERSION}"
                FOLDER      ARM
                SOURCES     "Main.cpp"
                LIBS        MightyOakLib)

# Add the current directory so that the PCH can be found when referenced by
# PCH-generating source files in the build tree.
target_include_directories(MightyOak PRIVATE "${MO_INCLUDE_DIR}")