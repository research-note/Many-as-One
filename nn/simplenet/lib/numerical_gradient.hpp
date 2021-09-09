/*
 * Build numerical gradient lib.
 *
 * Copyright Paran Lee
 *
 */

#ifndef NN_SIMPLENET_LIB_NUMERICAL_GRADIENT_HPP_
#define NN_SIMPLENET_LIB_NUMERICAL_GRADIENT_HPP_

template <typename F, typename T>
T numerical_gradient(F f, T x);

#include "numerical_gradient.cc"

#endif  // NN_SIMPLENET_LIB_NUMERICAL_GRADIENT_HPP_
