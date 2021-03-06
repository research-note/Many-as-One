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

#include "filters/GreaterThanFilter.hpp"

namespace db {

template <typename T>
GreaterThanFilter<T>::GreaterThanFilter(std::string column_name, typename T::ElementType value)
        : _column_name(column_name), _value(value)
{

}

template <>
void
GreaterThanFilter<db::LongType>::initialize(TableCursor& table_cursor) {
    _cursor = table_cursor.getLongColumn(_column_name);
}

template <>
void
GreaterThanFilter<db::DoubleType>::initialize(TableCursor& table_cursor) {
    _cursor = table_cursor.getDoubleColumn(_column_name);
}

template <typename T>
bool
GreaterThanFilter<T>::evaluate()
{
    return _cursor->get() > _value;
}

};

template class db::GreaterThanFilter<db::LongType>;
template class db::GreaterThanFilter<db::DoubleType>;