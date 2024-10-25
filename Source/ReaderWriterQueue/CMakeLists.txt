# CMakeLists for ReaderWriterQueue header-only library.
# Qudos to cameron@moodycamel.com for the code.
# Github repository: https://github.com/cameron314/readerwriterqueue
# Original article: https://moodycamel.com/blog/2013/a-fast-lock-free-queue-for-c++
add_library(ReaderWriterQueue INTERFACE)

target_include_directories(ReaderWriterQueue INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")