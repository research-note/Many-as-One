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

#ifndef NN_SIMPLENET_MAIN_SIMPLENET_H_
#define NN_SIMPLENET_MAIN_SIMPLENET_H_

#include <vector>
#include <optional>
#include <cstddef>

using namespace std;

template <typename T>
class Simplenet
{
public:
	explicit Simplenet(size_t width = kDefaultWidth, 
		size_t height = kDefaultHeight);
	virtual ~Simplenet() = default;

	// 복제 생성자와 대입 연산자를 명시적으로 디폴트로 지정한다.
	Simplenet(const Simplenet& src) = default;
	Simplenet<T>& operator=(const Simplenet& rhs) = default;

	// 이동 생성자와 대입 연산자를 명시적으로 디폴트로 지정한다.
	Simplenet(Simplenet&& src) = default;
	Simplenet<T>& operator=(Simplenet&& rhs) = default;

	std::optional<T>& at(size_t x, size_t y);
	const std::optional<T>& at(size_t x, size_t y) const;

	size_t getHeight() const { return mHeight; }
	size_t getWidth() const { return mWidth; }

	void randomize();

	static const size_t kDefaultWidth = 10;
	static const size_t kDefaultHeight = 10;

private:
	void verifyCoordinate(size_t x, size_t y) const;

	std::vector<std::vector<std::optional<T>>> mCells;
	size_t mWidth, mHeight;
};

#include "Simplenet.cc"

#endif  // NN_SIMPLENET_MAIN_SIMPLENET_H_
