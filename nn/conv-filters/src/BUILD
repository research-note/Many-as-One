load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "bin",
    hdrs = [
        "conv2d_layer.hpp",
        "filter.hpp",
        "NDGrid.hpp", "NDGrid.cc",
    ],
)

# cc_test(
#     name = "test",
#     srcs = ["test/test.cc"],
#     deps = [
#         "@gtest//:gtest_main", # gtest_main already contains gtest
#         "//lib:lib",
#     ],
# )

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":bin",
        # "//lib:lib",
    ],
)
