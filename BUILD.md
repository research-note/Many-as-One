# BUILD

To build this example you use (notice that 3 slashes are required in windows)
```
bazel build --config=clang_config //main:main

# In Windows, note the three slashes

bazel build --config=clang_config ///main:main
```

## Compile a Standalone Static Executable with flags

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
