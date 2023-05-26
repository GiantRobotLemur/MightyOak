# CMakeLists configuration for the Ag::Core library

# Define the library.
add_ag_library(AgCore SOURCES   CoreInternal.hpp
                                CoreInternal.cpp
                                Binary.cpp
                                ByteOrder.cpp
                                CodePoint.cpp
                                Utf.cpp
                                InlineMemory.cpp
                                StringPrivate.hpp
                                StringPrivate.cpp
                                String.cpp
                                StackTrace.cpp
                                Exception.cpp
                                ErrorGuard.cpp
                                ScalarParser.cpp
                                Stream.cpp
                                VariantType.cpp
                                VariantTypes.cpp
                                Variant.cpp
                                Format.cpp
                                CommandLineSchema.cpp
                                ProgramArguments.cpp
                                App.cpp
                                FsPathSchema.cpp
                                FsPathSchema.hpp
                                FsPath.cpp
                                FsSearchPathList.cpp
                                FsDirectory.cpp
                                Uri.cpp
                                Timer.cpp
                                Version.cpp
                                Platform.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Configuration.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/EnumInfo.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Utils.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/CollectionTools.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/LinearSortedSet.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/LinearSortedMap.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Binary.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/ByteOrder.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/CodePoint.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Utf.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/InlineMemory.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/StackTrace.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Exception.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/ErrorGuard.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/ScalarParser.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/String.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Stream.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/VariantType.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/VariantTypes.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Variant.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Format.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/CommandLineSchema.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/ProgramArguments.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/App.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/FsPath.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/FsSearchPathList.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/FsDirectory.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Uri.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Timer.hpp
                                ${AG_INCLUDE_DIR}/Ag/Core/Version.hpp
                    WIN_SOURCES Win32API.hpp
                                Win32API.cpp
                                Core.natvis
                                cpp.hint
                    POSIX_SOURCES PosixAPI.hpp
                                  PosixAPI.cpp
                    HEADERS     ${AG_INCLUDE_DIR}/Ag/Core.hpp
                                ${AG_INCLUDE_DIR}/Ag/GTest_Core.hpp
                    FOLDER      Core)

# Define the unit test harness.
add_ag_test_app(Core_Tests TEST_LIB AgCore
                           SOURCES  Test_Binary.cpp
                                    Test_Utils.cpp
                                    Test_EnumInfo.cpp
                                    Test_LinearSortedSet.cpp
                                    Test_LinearSortedMap.cpp
                                    Test_Utf.cpp
                                    Test_String.cpp
                                    Test_StackTrace.cpp
                                    Test_Exception.cpp
                                    Test_PackedFieldHelper.cpp
                                    Test_ErrorGuard.cpp
                                    Test_Variant.cpp
                                    Test_Format.cpp
                                    Test_ScalarParser.cpp
                                    Test_CommandLine.cpp
                                    Test_App.cpp
                                    Test_FileSystem.cpp
                                    Test_Uri.cpp
                                    Test_Timer.cpp
                                    Test_Version.cpp)

