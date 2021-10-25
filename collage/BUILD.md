# BUILD

In this stage we step it up and showcase how to integrate multiple ```cc_library``` targets from different packages.

BUILD file is in a subdirectory called lib. In Bazel, subdirectories containing BUILD files are known as packages. The new property ```visibility``` will tell Bazel which package(s) can reference this target, in this case the ```//main``` package can use ```lib``` library. 

```
cc_library(
    name = "bin",
    hdrs = ["util.hpp"],
    // srcs = ["util.cc"],
    visibility = ["//main:__pkg__"],
)
```

To use our ```lib``` libary, an extra dependency is added in the form of //path/to/package:target_name, in this case, it's ```//lib:lib```

```
cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":bin",
        "//lib:lib",
    ],
)
```

To build this example you use (notice that 3 slashes are required in windows)
```
bazel build --config=clang_config //collage:main

# In Windows, note the three slashes

bazel build --config=clang_config ///collage:main
```
