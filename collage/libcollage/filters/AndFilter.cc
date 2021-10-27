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

#include <arrow/api.h>

#include "filters/AndFilter.hpp"

using namespace db;

AndFilter::AndFilter(std::shared_ptr<Filter> left, std::shared_ptr<Filter> right)
        : _left(left), _right(right)
{

}

void
AndFilter::initialize(TableCursor& table_cursor) {
    _left->initialize(table_cursor);
    _right->initialize(table_cursor);
}

bool
AndFilter::evaluate()
{
    // short circuit explicitly
    if (_left->evaluate()) {
        return _right->evaluate();
    } else {
        return false;
    }
}
