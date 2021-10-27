load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "bin",
    hdrs = [
        "CHuffmanTree.hpp", "CHuffmanTree.cc",
        "CParser.hpp", "CParser.cc",
        "CUpng.hpp", "CUpng.cc",
    ],
)

cc_binary(
    name = "example",
    srcs = ["example.cc"],
    deps = [
        ":bin"
    ],
)
