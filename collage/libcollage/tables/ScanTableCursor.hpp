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

#ifndef SCAN_TABLE_CURSOR_HPP
#define SCAN_TABLE_CURSOR_HPP

#include <unordered_map>
#include <arrow/table.h>

#include "columns/BaseColumnCursor.hpp"
#include "columns/GenericColumnCursor.hpp"
#include "tables/TableCursor.hpp"
#include "core/DBSchema.hpp"

namespace db {

    class GenericColumnCursor;

    class TableTest;

/**
 * Table cursor implementation for scanning a table. Get this by calling getScanCursor() on a DBTable.
 */
    class ScanTableCursor : public TableCursor {
        friend class TableTest;

    public:
        /**
         * Overload for specifying non-default column encodings.
         * @param table
         * @param encodings
         */
        explicit ScanTableCursor(
                std::shared_ptr<arrow::Table> table,
                std::vector<db::ColumnEncoding> encodings
        );

        /**
         * Overload using default (PLAIN) column encodings
         * @param table
         */
        explicit ScanTableCursor(
                std::shared_ptr<arrow::Table> table
        );

        std::shared_ptr<BaseColumnCursor<db::StringType>> getStringColumn(std::string colName) override;

        std::shared_ptr<BaseColumnCursor<db::LongType>> getLongColumn(std::string colName) override;

        std::shared_ptr<BaseColumnCursor<db::DoubleType>> getDoubleColumn(std::string colName) override;

        std::shared_ptr<GenericColumnCursor> getColumn(std::string colName) override;

        bool hasMore() override;

        void reset() override;

    protected:
        bool addColumn(std::shared_ptr<arrow::ChunkedArray> column, const std::string& column_name, db::ColumnEncoding encoding);

        uint64_t getPosition() const override;

    private:
        std::shared_ptr<arrow::Table> _table;
        uint64_t _position;
        uint64_t _size;
        std::unordered_map<std::string, std::shared_ptr<GenericColumnCursor>> _cursors;
        bool _initial_state;
    };

};


#endif // SCAN_TABLE_CURSOR_HPP
