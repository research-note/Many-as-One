load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "parser",
    hdrs = ["Parser.hpp", "Parser.cc"],
)

cc_binary(
    name = "example",
    srcs = ["example.cc"],
    deps = [
        ":parser"
    ]
)
