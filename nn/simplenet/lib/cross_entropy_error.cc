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

#include <vector>
#include <functional>
#include <iostream>
#include <numeric>
#include <execution>

#include <cmath>

#include "lib/cross_entropy_error.hpp"

template <typename Y, typename T>
auto cross_entropy_error(Y y, T t) {
    const auto delta = 1e-7;
    const auto result =  std::transform_reduce(std::execution::par,
        y.begin(), y.end(), t.begin(),
        0.0, std::plus<>(),
        [delta](auto yv, auto tv) -> auto {
            return -log(yv + delta) * tv;
        });
    std::cout << "cross_entropy_error(y, t) :" << result << std::endl;
    return result;
}
