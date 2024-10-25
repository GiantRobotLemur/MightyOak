# The CMake configuration for documentation related tasks.

# Generate Documentation from code.
if (DEFINED DOXYGEN_FOUND AND DEFINED DOC_SRCS)
    set(DOXYGEN_GENERATE_HTML YES)

    # HTML Help CHM file generation options.
    list(APPEND DOXYGEN_PREDEFINED DO_NOT_DOCUMENT "STRUCT_ALIGN_8="
                                   "STRUCT_ALIGN_16=" "STRUCT_ALIGN_32=")
    list(APPEND DOXYGEN_EXCLUDE_PATTERNS "*/Template.cpp"
                                         "*/Template.hpp"
                                         "*/Test_Template.cpp")

    set(DOXYGEN_WARN_IF_DOC_ERROR YES)
    set(DOXYGEN_INHERIT_DOCS YES)
    set(DOXYGEN_HIDE_UNDOC_CLASSES YES)
    set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
    set(DOXYGEN_SORT_BRIEF_DOCS YES)
    set(DOXYGEN_SORT_MEMBER_DOCS YES)
    set(DOXYGEN_SORT_MEMBERS_CTORS_1ST YES)
    set(DOXYGEN_GENERATE_TODOLIST YES)
    set(DOXYGEN_ENABLE_PREPROCESSING YES)
    set(DOXYGEN_EXPAND_ONLY_PREDEF YES)

    # Find HTML Help Compiler to produce .CHM files under Windows.
    include(FindHTMLHelp)

    if(DEFINED HTML_HELP_COMPILER)
       set(DOXYGEN_GENERATE_HTMLHELP YES)
       set(DOXYGEN_SEARCHENGINE NO)
       set(DOXYGEN_CHM_FILE "${CMAKE_BINARY_DIR}/MightyOak.chm")
       set(DOXYGEN_HHC_LOCATION "${HTML_HELP_COMPILER}")
    else()
        # If HTML Help not available, consider:
        set(DOXYGEN_GENERATE_TREEVIEW YES)
        set(DOXYGEN_SEARCHENGINE YES)
    endif()

    doxygen_add_docs(Documentation "${DOC_SRCS}"
                     WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                     COMMENT "Generating documentation...")
endif()