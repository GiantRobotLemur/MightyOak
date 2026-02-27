# CMakeLists configuration for the ARM Debugger CLI application.

ag_add_cli_app(ArmDbgCli  FOLDER      ARM
                           NAME        "ArmDbg"
                           DESCRIPTION "A scriptable ARM emulator debugger."
                           AUTHOR      "${MO_AUTHOR}"
                           COPYRIGHT   "${MO_COPYRIGHT}"
                           VERSION     "${PROJECT_VERSION}"
                           SOURCES     "ArmDbgCli_Main.cpp"
                           LIBS        ArmDbg)
