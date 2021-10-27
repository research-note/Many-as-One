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

#ifndef CHUNKEDDICTCOLUMNCURSOR_HPP
#define CHUNKEDDICTCOLUMNCURSOR_HPP

#include <arrow/table.h>

#include "columns/BaseColumnCursor.hpp"


namespace db {


/**
 * A simple column cursor implemented on top of a possibly chunked Arrow column, the hides the
 * chunking to present a simpel column structure. This is not directly used for executing queries.
 *
 * @tparam T The underlying Arrow array type:: for example, arrow::Int64Array.
 */
    template<typename T>
    class ChunkedDictColumnCursor : public BaseColumnCursor<T> {
    public:
        /**
         * Create from a column -- initially positioned at first element, if any.
         * @param column
         */
        explicit ChunkedDictColumnCursor(std::shared_ptr<arrow::ChunkedArray> column, TableCursor &table_cursor);

        /**
         * Will next() produce another element?
         * @return
         */
        bool hasMore();

        /**
         * Move to the next element.
         * @return True if an element is available, false otherwise (end of column.)
         */
        bool next();

        /**
         * Is the element at the current position null?
         * @return
         */
        bool isNull();

        /**
         * Get value at current position.
         * @return
         */
        typename T::ElementType get();

        /**
         * Reset to the first element, if any.
         */
        void reset();

        /**
         * Seek to the given position.
         * @param to zero-based ordinal position of element in column
         * @return True if successful.
         */
        bool seek(uint64_t to);

    protected:

        /**
         * Advance to the next chunk in the column's chunk sequence, when the values
         * in the current chunk have been exhausted.
         * @return True if successful, false if the current chunk was the last.
         */
        bool advance_chunk();

    private:

        /**
         * The underlying column
         */
        std::shared_ptr<arrow::ChunkedArray> _column;

        /**
         * The current chunk of the underlying column
         */

        std::shared_ptr<typename T::ArrayType> _current_dict;
        std::shared_ptr<arrow::NumericArray<arrow::Int8Type>> _current_indices;

        /**
         * Offset of current chunk inthe sequence of chunks
         */
        int32_t _chunk = 0;

        /**
         * Offset within the current chunk
         */
        int64_t _pos_in_chunk = 0;

        /**
         * Position within the (logical) column.
         */
        int64_t _pos = 0;

    private:
        void populate_pointers();
    };

};


#endif // CHUNKEDDICTCOLUMNCURSOR_HPP
