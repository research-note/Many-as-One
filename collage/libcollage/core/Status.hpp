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

#ifndef STATUS_HPP
#define STATUS_HPP

#include <cstdint>

//
// All contents adapted from Arrow's own Status class
//


// Return the given status if it is not OK.
#define DB_RETURN_NOT_OK(s)              \
  do {                                   \
    ::db::Status _s = (s);               \
    if (!_s.ok()) {                      \
      return _s;                         \
    }                                    \
  } while (false)

namespace db {

    enum class StatusCode : std::uint8_t {
        OK = 0,
        NotFound = 1,
        InternalError = 2,
        OutOfMemory = 3,
        UnevenArgLists = 4,
        InvalidColumn = 5
    };

    class Status {

    public:

        Status(StatusCode code) : _code(code) {}

        static Status OK() { return Status(StatusCode::OK); }

        bool ok() const { return (_code == StatusCode::OK); }

        bool notFound() const { return (_code == StatusCode::NotFound); }

        bool internalError() const { return (_code == StatusCode::InternalError); }

        bool outOfMemory() const { return (_code == StatusCode::OutOfMemory); }

        bool unevenArgLists() const { return (_code == StatusCode::UnevenArgLists); }

        bool invalidColumn() const { return (_code == StatusCode::InvalidColumn); }


        StatusCode code() const { return _code; }

    private:

        StatusCode _code;

    };

};

#endif //STATUS_HPP
