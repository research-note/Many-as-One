/*
 * Build softmax lib.
 *
 * Copyright Paran Lee
 *
 */

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
