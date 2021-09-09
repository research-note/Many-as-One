/*
 * Build obj linking with cc_library file example.
 *
 * Copyright Bazel organization
 *
 */

#ifndef NN_SIMPLENET_LIB_SIGMOID_H_
#define NN_SIMPLENET_LIB_SIGMOID_H_

#include <iostream>
#include <vector>
#include <numeric>

using namespace std;

template <typename T>
T sigmoid(const T& x);

#include "sigmoid.cc"

#endif  // NN_SIMPLENET_LIB_SIGMOID_H_
