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

#ifndef TABLE_CURSOR_HPP
#define TABLE_CURSOR_HPP

#include <cstdint>
#include <memory>
#include <arrow/api.h>

#include "core/DBSchema.hpp"

namespace db {

    class GenericColumnCursor;

    class FilterProjectTableCursor;

    template<typename T>
    class BaseColumnCursor;

/**
 * Common base of all table cursors, exposing row iteration and access to columns.
 */
    class TableCursor {

        template <typename T>
        friend class BaseColumnCursor;

        friend class GenericColumnCursor;

        friend class FilterProjectTableCursor;

    public:

        /**
         * Get access to the specified String type column, always synchronized with this cursor's position.
         * @param colName
         * @return
         */
        virtual std::shared_ptr<BaseColumnCursor<db::StringType>> getStringColumn(std::string colName) = 0;

        /**
         * Get access to the specified Long type column, always synchronized with this cursor's position.
         * @param colName
         * @return
         */
        virtual std::shared_ptr<BaseColumnCursor<db::LongType>> getLongColumn(std::string colName) = 0;

        /**
         * Get access to the specified Half Float type column, always synchronized with this cursor's position.
         * @param colName
         * @return
         */
        virtual std::shared_ptr<BaseColumnCursor<db::HalfFloatType>> getHalfFloatColumn(std::string colName) = 0;

        /**
         * Get access to the specified Float type column, always synchronized with this cursor's position.
         * @param colName
         * @return
         */
        virtual std::shared_ptr<BaseColumnCursor<db::FloatType>> getFloatColumn(std::string colName) = 0;

        /**
         * Get access to the specified Double type column, always synchronized with this cursor's position.
         * @param colName
         * @return
         */
        virtual std::shared_ptr<BaseColumnCursor<db::DoubleType>> getDoubleColumn(std::string colName) = 0;

        /**
         * Get access to the specified column, always synchronized with this cursor's position.
         * UYse this in cases where you don't need to deal with the column's type.
         * @param colName
         * @return
         */
        virtual std::shared_ptr<GenericColumnCursor> getColumn(std::string colName) = 0;

        /**
         * True if there are more rows.
         * @return
         */
        virtual bool hasMore() = 0;

        /**
         * Reaset to start and ready to iterate again.
         */
        virtual void reset() = 0;

        virtual ~TableCursor() = default;

    protected:
        virtual uint64_t getPosition() const = 0;
    };

};

#endif // TABLE_CURSOR_HPP
