/*
 * Build obj linking with cc_library Util Class.
 *
 * Copyright Bazel organization
 *
 */

#ifndef COLLAGE_UTIL_HPP_
#define COLLAGE_UTIL_HPP_

#include <vector>
#include <optional>
#include <cstddef>

using namespace std;

template <typename T>
class Util
{
public:
	explicit Util();
	virtual ~Util() = default;

private:

};

#include "Util.cc"

#endif  // COLLAGE_UTIL_HPP_
