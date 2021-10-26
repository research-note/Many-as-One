load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "parser",
    hdrs = ["CParser.hpp", "CParser.cc"],
    deps = [":upng"]
)

cc_library(
    name = "upng",
    hdrs = ["CUpng.hpp", "CUpng.cc"],
    deps = [":huffman_tree"]
)

cc_library(
    name = "huffman_tree",
    hdrs = ["CHuffmanTree.hpp", "CHuffmanTree.cc"]
)

cc_binary(
    name = "example",
    srcs = ["example.cc"],
    deps = [
        ":parser"
    ]
)
