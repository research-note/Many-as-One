# Deep Learning From Scratch Using Modern C++

## Preinstall

Use `std::execution::par` parallel execution. They are used to specify the execution policy of parallel algorithms. so, install tbb!

Use boost::beast as fast async event driven server framework.

```console
foo@bar:~$ sudo apt -y install libtbb-dev libboost-system-dev
```

## Build

Build with Clang-12.

```console
foo@bar:template/for-class$ bazel build --config=clang_config  //main:hello-world
```

## Test

Test with [uftrace](https://github.com/namhyung/uftrace) and [valgrind](https://sourceware.org/git/valgrind.git).

Install uftrace and valgrind in ubuntu.

```console
foo@bar:~$ sudo apt -y install uftrace valgrind
```

### uftrace

Test hello-world program with uftrace in terminal.

```console
foo@bar:template/for-class$ uftrace ./bazel-bin/main/hello-world
```
### valgrind

Test hello-world program with valgrind in terminal.

```console
foo@bar:template/for-class$ valgrind ./bazel-bin/main/hello-world
```

## lint

Install [cpplint](https://github.com/cpplint/cpplint).

```console
foo@bar:~$ sudo apt -y install python3  && pip install cpplint
foo@bar:~$ echo "export PATH=\$PATH:\$HOME/.local/bin" >> .bashrc && source .bashrc
```

And do lint.

```console
foo@bar:template/for-class$ cpplint main/hello-world.cc
```

# Reference

[『Deep Learning From Scratch』 (Hanbit media), Modern C++ ≥ 17 migration](https://github.com/research-note/deep-learning-from-scratch-using-modern-cpp)

[Installing Bazel on Ubuntu](https://docs.bazel.build/versions/main/install-ubuntu.html)

[Bazel Tutorial: Build a C++ Project](https://docs.bazel.build/versions/main/tutorial/cpp.html)

[Bazel Tutorial: Configure C++ Toolchains](https://docs.bazel.build/versions/main/user-manual.html)

[Bazel command line options](https://docs.bazel.build/versions/main/user-manual.html)
