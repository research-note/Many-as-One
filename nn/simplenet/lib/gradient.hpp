/*
 * Build gradient lib.
 *
 * Copyright Paran Lee
 *
 */

#ifndef NN_SIMPLENET_LIB_GRADIENT_HPP_
#define NN_SIMPLENET_LIB_GRADIENT_HPP_

template <typename F, typename T>
T gradient(F f, T x);

#include "gradient.cc"

#endif  // NN_SIMPLENET_LIB_GRADIENT_HPP_
