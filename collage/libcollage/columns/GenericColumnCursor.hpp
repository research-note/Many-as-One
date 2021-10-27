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

#ifndef GENERIC_COLUMN_CURSOR_HPP
#define GENERIC_COLUMN_CURSOR_HPP

#include <cstdint>

namespace db {

    class TableCursor;

    /**
     * Access to columns, controlled by a TableCursor. Obtain one of these by
     * calling getColumn on your outermost TableCursor, and use that TableCursor's hasMore()
     * method to iterate. TO get data out of one of these, cast it to the right kind of
     * ColumnCursorWrapper and call get().
     */
    class GenericColumnCursor {
        friend class ScanTableCursor;

    public:
        virtual ~GenericColumnCursor() = default;

        /**
         * Is the element at the current position null?
         * @return
         */
        virtual bool isNull() = 0;

    protected:
        explicit GenericColumnCursor(TableCursor &table_cursor);

        /**
         * Reset to the first element, if any.
         */
        virtual void reset() = 0;

        uint64_t get_table_cursor_position();

    private:
        TableCursor &_table_cursor;
    };

};


#endif // GENERIC_COLUMN_CURSOR_HPP
