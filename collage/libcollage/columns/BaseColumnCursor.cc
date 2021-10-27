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

#include <memory>

#include "columns/BaseColumnCursor.hpp"
#include "core/DBSchema.hpp"
#include "columns/ChunkedColumnCursor.hpp"
#include "columns/ChunkedDictColumnCursor.hpp"


namespace db {

template<typename T>
BaseColumnCursor<T>::BaseColumnCursor(TableCursor &table_cursor)
: GenericColumnCursor(table_cursor) {}

template<typename T>
std::shared_ptr<BaseColumnCursor<T>>
BaseColumnCursor<T>::makeCursor(
        std::shared_ptr<arrow::ChunkedArray> column, ColumnEncoding encoding, TableCursor &table_cursor)
{
    switch (encoding) {
        case db::ColumnEncoding::PLAIN: {
            return std::make_shared<ChunkedColumnCursor<T>>(column, table_cursor);
        }
        case db::ColumnEncoding::DICT: {
            return std::make_shared<ChunkedDictColumnCursor<T>>(column, table_cursor);
        }
        default:
            return std::make_shared<ChunkedColumnCursor<T>>(column, table_cursor);
    }
}

}

template class db::BaseColumnCursor<db::LongType>;
template class db::BaseColumnCursor<db::DoubleType>;
template class db::BaseColumnCursor<db::StringType>;