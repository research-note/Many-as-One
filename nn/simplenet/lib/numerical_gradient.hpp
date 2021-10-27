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

#ifndef NN_SIMPLENET_LIB_NUMERICAL_GRADIENT_HPP_
#define NN_SIMPLENET_LIB_NUMERICAL_GRADIENT_HPP_

template <typename F, typename T>
T numerical_gradient(F f, T x);

#include "numerical_gradient.cc"

#endif  // NN_SIMPLENET_LIB_NUMERICAL_GRADIENT_HPP_
