/*
 * Build cross_entropy_error lib.
 *
 * Copyright Paran Lee
 *
 */

#ifndef NN_SIMPLENET_LIB_ENTROPY_ERROR_H_
#define NN_SIMPLENET_LIB_ENTROPY_ERROR_H_

template <typename Y, typename T>
auto cross_entropy_error(Y y, T t);

#include "cross_entropy_error.cc"

#endif  // NN_SIMPLENET_LIB_ENTROPY_ERROR_H_
