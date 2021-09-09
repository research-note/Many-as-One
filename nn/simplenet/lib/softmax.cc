/*
 * Build softmax lib.
 *
 * Copyright Paran Lee
 *
 */

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "lib/softmax.hpp"

template <typename T>
T softmax(T x) {
    auto max = std::max_element(x.begin(), x.end());

    std::transform(std::execution::par, x.begin(), x.end(),
        x.begin(), [max](auto v) -> auto {
            return exp(v - max);
        });

    const auto sum = std::reduce(std::execution::par,
        x.cbegin(), x.cend());

    std::transform(std::execution::par, x.begin(), x.end(),
        x.begin(), [sum](auto v) -> auto {
            return v / sum;
        });

    std::cout << "softmax(x) :" << std::endl;
    std::for_each(x.begin(), x.end(),
        [](auto v) { std::cout << v << ' '; });
    std::cout << std::endl;

    return x;
}
