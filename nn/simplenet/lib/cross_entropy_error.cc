/*
 * Build cross_entropy_error lib.
 *
 * Copyright Paran Lee
 *
 */

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
