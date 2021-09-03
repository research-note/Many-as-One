# BUILD

this BUILD file is in a subdirectory called lib. In Bazel, subdirectories containing BUILD files are known as packages. The new property ```visibility``` will tell Bazel which package(s) can reference this target, in this case the ```//main``` package can use ```bin``` library. 

```
cc_library(
    name = "lib",
    srcs = [/* ... */],
    hdrs = ["Cuboid.hpp", "Cuboid.cc", /* ... */],
    visibility = ["//main:__pkg__"],
)
```

To use our ```lib``` libary, an extra dependency is added in the form of //path/to/package:target_name, in this case, it's ```//lib:lib```

```
cc_binary(
    name = "bin",
    srcs = [/* ... */],
    hdrs = ["Grid.hpp", "Grid.cc", /* ... */],
    deps = [
        ":bin",
        "//lib:lib",
    ],
)
```

To build this example you use (notice that 3 slashes are required in windows)
```
bazel build --config=clang_config //main:main

# In Windows, note the three slashes

bazel build --config=clang_config ///main:main
```

## Compile a Standalone Static Executable with flags

Use the following flags for linking

`-static -static-libgcc -static-libstdc++`

Use these three flags to link against the static versions of all dependencies (assuming gcc).

Note, that in certain situation you don't necessarily need all three flags,
but they don't "hurt" either. Therefore just turn on all three.

Check if it actually worked
Make sure that there is really no dynamic linkage

```bash
$ ldd yourexecutable
```

should return `not a dynamic executable` or something equivalent.

Make sure that there are no unresolved symbols left.

```bash
$ nm yourexecutable | grep " U "
```

The list should be empty or should contain only some special kernel-space symbols like

`U __tls_get_addr`

Finally, check if you can actually execute your executable

(compile-a-standalone-static-executable)[https://stackoverflow.com/questions/3283021/compile-a-standalone-static-executable]
