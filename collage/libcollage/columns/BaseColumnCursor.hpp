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

#ifndef BASE_COLUMN_CURSOR_HPP
#define BASE_COLUMN_CURSOR_HPP

#include <memory>
#include <arrow/api.h>

#include "columns/GenericColumnCursor.hpp"
#include "core/DBSchema.hpp"

namespace db {

    class TableCursor;


    template<typename T>
    class BaseColumnCursor : public GenericColumnCursor {
    public:
        explicit BaseColumnCursor(TableCursor &table_cursor);

        static std::shared_ptr<BaseColumnCursor<T>> makeCursor(
                std::shared_ptr<arrow::ChunkedArray> column, ColumnEncoding encoding, TableCursor &table_cursor);

        /**
         * Get value at current position.
         * @return
         */
        virtual typename T::ElementType get() = 0;

    protected:

        /**
         * Seek to the given position.
         * @param to zero-based ordinal position of element in column
         * @return True if successful.
         */
        virtual bool seek(uint64_t to) = 0;

        /**
         * Will next() produce another element?
         * @return
         */
        virtual bool hasMore() = 0;

        /**
         * Move to the next element.
         * @return True if an element is available, false otherwise (end of column.)
         */
        virtual bool next() = 0;

    };
};

#endif // BASE_COLUMN_CURSOR_HPP
