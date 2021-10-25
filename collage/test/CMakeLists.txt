project(test)

set(GTEST_ROOT "" CACHE PATH "GoogleTest root directory")

find_package(Arrow REQUIRED)

# find_library(
#   arrow_shared
#   NAMES libarrow.so
#   HINTS /usr/lib/x86_64-linux-gnu
#   REQUIRED)

add_executable(testdb TableTest.cc main.cc Tables.cc)

target_link_libraries(testdb libgtest.a collage gtest arrow_shared)

install(TARGETS testdb DESTINATION bin)