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

#include <stdexcept>
#include <utility>
#include <random>

template <typename T>
Simplenet<T>::Simplenet(size_t width, size_t height)
    : mWidth(width)
    , mHeight(height)
{
    mCells.resize(mWidth);
    for (auto& column : mCells) {
        column.resize(mHeight);
    }
}

template <typename T>
void randomize()
{
    random_device seeder;
    const auto seed = seeder.entropy() ? seeder() : time(nullptr);
    mt19937 eng(static_cast<mt19937::result_type>(seed));
    normal_distribution<T> dist(0.0, 1.0);
    auto gen = bind(dist, eng);
    // for (auto& column : mCells) {
    //     for (auto& row : column) {
    //         row = gen();
    //         std::cout << ' ' << row << std::end;
    //     }
    //     std::cout << std::end;
    // }
}

template <typename T>
void Simplenet<T>::verifyCoordinate(size_t x, size_t y) const
{
    if (x >= mWidth || y >= mHeight) {
        throw std::out_of_range("");
    }
}

template <typename T>
const std::optional<T>& Simplenet<T>::at(size_t x, size_t y) const
{
    verifyCoordinate(x, y);
    return mCells[x][y];
}

template <typename T>
std::optional<T>& Simplenet<T>::at(size_t x, size_t y)
{
    return const_cast<std::optional<T>&>(std::as_const(*this).at(x, y));
}