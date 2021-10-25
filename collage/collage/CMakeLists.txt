project(collage)

set(EDGE_STORE_ROOT "" CACHE PATH "Edge Store root directory")

find_package(Arrow REQUIRED)

# find_library(
#   arrow_shared
#   NAMES libarrow.so
#   HINTS /usr/lib/x86_64-linux-gnu
#   REQUIRED)

add_executable(main main.cc)

target_link_libraries(main collage arrow_shared)
