#ifndef CONV_FILTER_H
#define CONV_FILTER_H

#include <cmath>
#include <memory>
#include <execution>
#include <string>

#include "NDGrid.hpp"

/**
 * v w(x);
 */
using v = std::vector<float>;

/**
 * matrix w(x, v(y));
 */
using matrix = std::vector<std::vector<float>>;

/**
 * tensor w(x, matrix(y,v(z)));
 */
using tensor = std::vector<std::vector<std::vector<float>>>;

template <typename F, typename T>
T for_vector(F f, T v) {
    for_each(std::execution::par, v.begin(), v.end(), f);
    return v;
}

template <typename F, typename T>
T for_matrix(F f, T m) {
    for_each(std::execution::par,
        m.begin(), m.end(),
        [f](auto v) {
            for_vector(f, v);
        });
    
    return m;
}

template <typename F, typename T>
T for_tensor(F f, T t) {
    for_each(std::execution::par,
        t.begin(), t.end(),
        [f](auto m) {
            for_matrix(f, m);      
        });
    return t;
}

template <typename F, typename T>
T trans_vector(F f, T &v) {
    transform(std::execution::par, v.begin(), v.end(),
        v.begin(), [f](auto v) -> auto {
            return f(v);
        });
    return v;
}

template <typename F, typename T>
T trans_matrix(F f, T &m) {
    transform(std::execution::par, m.begin(), m.end(),
        m.begin(), [f](auto v) -> auto {
            return trans_vector(f, v);
        });
    return m;
}

/**
 * Tensor reference value. 
 */
template <typename F, typename T>
T trans_tensor(F f, T &t) {
    transform(std::execution::par, 
        t.begin(), t.end(),
        t.begin(), [f](auto m) -> auto {
            return trans_matrix(f, m);
        });
    return t;
}

template <typename T, typename Init>
auto sum_vector(T t, Init value) {
    return reduce(std::execution::par, t.begin(), t.end(), value);
}

template <typename F, typename T>
auto trans_reduce_vector(F f, T v) {
    return transform_reduce(std::execution::par, v.begin(), v.end(),
        0.0, std::plus<>(), f);
}

template <typename F, typename T>
auto trans_reduce_matrix(F f, T m) {
    return transform_reduce(std::execution::par, m.begin(), m.end(),
        0.0, std::plus<>(),
        [f](auto v) -> auto {
            return trans_reduce_vector(f, v);
        });
}

template <typename F, typename T>
auto trans_reduce_tensor(F f, T t) {
    return transform_reduce(std::execution::par, t.begin(), t.end(),
        0.0, std::plus<>(),
        [f](auto m) -> auto {
            return trans_reduce_matrix(f, m);
        });
}



// allocate memory for a tensor
tensor get_tensor(int x, int y, int z) {
    tensor w(x, matrix(y, v(z)));
    return w;
}

class filter {
public:
    tensor w;
    double b; // kernel matrix, bias term
    int window, depth;

    static const size_t kDefaultSize = 3;

    filter() 
    : window(kDefaultSize), depth(kDefaultSize) {
        tensor w(window, matrix(window, v(depth)));
    }

    filter(int size) 
    : window(size), depth(size) {
        tensor w(window, matrix(window, v(depth)));
    }

    filter(int _window, int _depth) 
    : window(_window), depth(_depth) {
        tensor w(window, matrix(window, v(depth)));
    }

    filter(tensor _w, int _window, int _depth, int _b = 0) 
    : w(_w), window(_window), depth(_depth), b(_b) {
        
    }

    virtual ~filter() = default;

    // normalize the tensor
    void normalize() {
        auto sum = trans_reduce_tensor([](auto v) -> auto {
                return std::abs(v);
            }, w);

        trans_tensor([sum](auto v) -> auto {
            return v / sum;
        }, w);
    }

};

#endif
