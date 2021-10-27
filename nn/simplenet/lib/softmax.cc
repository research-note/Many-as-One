/* Copyright 2021 The Many as One Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <iostream>
#include <algorithm>
#include <numeric>
#include <complex>
#include <functional>

#include "lib/softmax.hpp"

template <typename T>
T softmax(T x) {
    auto max = std::max_element(x.begin(), x.end());
    auto maxsub = [max](auto v) -> auto {
        return v - *max;
    };
    std::transform(std::execution::par, x.begin(), x.end(), 
        x.begin(), [maxsub](auto v) -> auto {
            return std::exp(maxsub(v));
        });

    auto sum = std::reduce(std::execution::par,
        x.cbegin(), x.cend(), 0.0);
    std::transform(std::execution::par, x.begin(), x.end(), 
        x.begin(), [sum](auto v) {
            return v / sum;
        });

    std::cout << "softmax(x) :" << std::endl;
    std::for_each(x.begin(), x.end(),
        [](auto v) { std::cout << v << ' '; });
    std::cout << std::endl;

    return x;
}
