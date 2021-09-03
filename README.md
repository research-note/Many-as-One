# Many as One

We want supercomputed neural network that parallel(core per multithread, SIMD or AVX vectorized instructions), heterogeneous(FPGA or GPU accelations)-based distributed network processing on superclusters.

There's a lot of AI framework for parallel & heterogeneous-based distributed computing. but this framework needed specific machine that vecdor dependent software. otherwise, we need to provide an environment where low-performance machines and high-performance machines can coexist and cooperative each other in distributed neural network. futher more, user frontend should be able to utilize a high-performance backend in a convenient web-based interface on any playform.

Also, we need container, k8s friendly cloud native framework.

## Webapp - Frontend

Web application visualize your model graph and provide interface that communicate with neural network handle each layer or unit.

system monitoring each computing nodes that have unit are also planned.

## Neural Network - Backend

You can scale out nerual network. this loadbalance your learning data, and do reduce learning time.

![independent from machine vendors](https://raw.githubusercontent.com/research-note/Many-as-One/master/Linus-Torvalds-Fuck-You-Nvidia.jpg?sanitize=true)

### Tensor programming

![SYCL](https://raw.githubusercontent.com/research-note/Many-as-One/master/SYCL_logo.svg.png?sanitize=true)

First, we don't need `CUDA`, no more `.cl` codes for type safe coding.

![sycl](https://raw.githubusercontent.com/research-note/Many-as-One/master/2020-05-sycl-landing-page-01_3.jpg?sanitize=true)

Then, we select `Eigen` tensor that have implemented SYCL backend.

![Eigen](https://raw.githubusercontent.com/research-note/Many-as-One/master/Eigen_Silly_Professor_135x135.png?sanitize=true)

### Handle datas with Colomnar DB format

We need superfast, GPU accelated size optimized colmnar Format.

![Apache Arrow](https://raw.githubusercontent.com/research-note/Many-as-One/master/arrow-inverse.png?sanitize=true)

### boost::asio based event driven network

We can maximize network communication performance using boost::asio event driven programming model in distributed neural network.

### Distributed build system

We compile system for fast build and unit test future situation.

![Bazel](https://raw.githubusercontent.com/research-note/Many-as-One/master/xenonstack-advantages-of-bazel.png?sanitize=true)

## Reference

[SYCL](https://en.wikipedia.org/wiki/SYCL)

[Apache Arrow](https://en.wikipedia.org/wiki/Apache_Arrow)

[Eigen](https://en.wikipedia.org/wiki/Eigen_(C%2B%2B_library))

[boost C++ Libraries](https://www.boost.org/)

[Bazel](https://en.wikipedia.org/wiki/Bazel_(software))
