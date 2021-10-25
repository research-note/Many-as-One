
project(libcollage)

MESSAGE(STATUS "Arrow root = " ${ARROW_ROOT})

find_package(Threads REQUIRED)

set(ARROW_LIBS libarrow.a)

include_directories(${ARROW_INCLIDE_DIRS})

link_directories(${ARROW_LINK_DIRS})

set(SOURCE_FILES
    columns/BaseColumnCursor.hpp columns/BaseColumnCursor.cc
    columns/ChunkedColumnCursor.hpp columns/ChunkedColumnCursor.cc
    columns/ChunkedDictColumnCursor.hpp columns/ChunkedDictColumnCursor.cc
    columns/GenericColumnCursor.hpp columns/GenericColumnCursor.cc

    core/DBSchema.hpp core/DBSchema.cc core/Status.hpp

    filters/AndFilter.hpp filters/AndFilter.cc
    filters/Filter.hpp
    filters/GreaterThanFilter.hpp filters/GreaterThanFilter.cc
    filters/IsNullFilter.hpp filters/IsNullFilter.cc
    filters/LessThanFilter.hpp filters/LessThanFilter.cc
    filters/NotFilter.hpp filters/NotFilter.cc

    tables/DBColumnBuilder.hpp tables/DBColumnBuilder.cc
    tables/DBTable.hpp tables/DBTable.cc
    tables/FilterProjectTableCursor.hpp tables/FilterProjectTableCursor.cc
    tables/ScanTableCursor.hpp tables/ScanTableCursor.cc
    tables/TableCursor.hpp tables/TableCursor.cc
    )

add_library(collage STATIC ${SOURCE_FILES})

target_link_libraries(collage ${ARROW_LIBS} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(collage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
