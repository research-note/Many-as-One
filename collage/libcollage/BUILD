load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "lib",
    hdrs = [
        "columns/BaseColumnCursor.hpp",
        "columns/ChunkedColumnCursor.hpp",
        "columns/ChunkedDictColumnCursor.hpp",
        "columns/GenericColumnCursor.hpp",
        "core/DBSchema.hpp",
        "core/Status.hpp",
        "filters/AndFilter.hpp",
        "filters/Filter.hpp",
        "filters/GreaterThanFilter.hpp",
        "filters/IsNullFilter.hpp",
        "filters/LessThanFilter.hpp",
        "filters/NotFilter.hpp",
        "tables/DBColumnBuilder.hpp",
        "tables/DBTable.hpp",
        "tables/FilterProjectTableCursor.hpp",
        "tables/ScanTableCursor.hpp",
        "tables/TableCursor.hpp",
    ],

    visibility = [
        "//visibility:public",
        # "//collage:__pkg__",
        # "//test:__pkg__",
    ],
)
