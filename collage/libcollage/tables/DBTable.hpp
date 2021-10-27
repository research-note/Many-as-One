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

#ifndef DB_TABLE_HPP
#define DB_TABLE_HPP

#include <vector>
#include <arrow/api.h>

#include "columns/GenericColumnCursor.hpp"
#include "core/Status.hpp"
#include "core/DBSchema.hpp"
#include "tables/ScanTableCursor.hpp"
#include "tables/DBColumnBuilder.hpp"

namespace db {

/**
 * Database table class, abstracting Arrow's Table class and other
 * metadata such as column encodings.
 */
    class DBTable {
    public:

        /**
         * Create a table form column names, types and encodings. All
         * argument vectors must be of the same length as the entries
         * refer to corresponding table columns.
         *
         * @param names Column names
         * @param types Column datatypes
         * @param encodings Column encodings
         * @param table out paramater -- shared pointer to resulting table is successful
         * @param pool optional Arrow memory pool
         * @return UnevenArgLists (if the name, type and encoding lists differ in length), InvalidColumn
         *         (if the type and encoding of a column don't make sense) or OK
         */
        static Status create(std::vector<std::string> names,
                      std::vector<std::shared_ptr<db::DataType>> types,
                      std::vector<db::ColumnEncoding> encodings,
                      std::shared_ptr<DBTable> *table,
                      arrow::MemoryPool *pool = arrow::default_memory_pool());

        /**
         * Start a new chunk for each column.
         */
        Status endChunk();

        /**
         * Prepare the table for use.
         */
        Status make();

        /**
         * Return a scan cursor for the table. Use this for querying,
         * possibly by composing other tbale cursors with it.
         * @return
         */
        std::shared_ptr<ScanTableCursor> getScanCursor() const;

        /**
         * Get the underlying Arrow Table.
         * @return
         */
        std::shared_ptr<arrow::Table> getTable() const;

        /**
         * Add a row of values -- specified in same order as columns in constructor.
         * @param values
         */
        Status addRow(std::vector<std::shared_ptr<db::GenValue>> values);

        /**
         * Dump table for debugging
         */
        Status dump() const;

    protected:

        /**
         * Create a table form column names, types and encodings. All
         * argument vectors must be of the same length as the entries
         * refer to corresponding table columns.
         * @param names Column names
         * @param types Column datatypes
         * @param encodings Column encodings
         */
        explicit DBTable(
                std::vector<std::string> names,
                std::vector<std::shared_ptr<db::DataType>> types,
                std::vector<db::ColumnEncoding> encodings,
                arrow::MemoryPool *pool = arrow::default_memory_pool());

        /**
         * Does the given encoding make sense for the given datatype?
         * @param dataType
         * @param encoding
         * @return InvalidColumn
         *         (if the type and encoding don't make sense) or OK
         */
        static Status checkCompatible(std::shared_ptr<db::DataType> dataType,
                                      db::ColumnEncoding encoding);

    private:
        /**
         * Arrow table schema
         */
        std::shared_ptr<arrow::Schema> _schema;

        /**
         * Arrow table
         */
        std::shared_ptr<arrow::Table> _table;

        /**
         * Column encodings
         */
        std::vector<db::ColumnEncoding> _encodings;

        /**
         * Arrow columns
         */
        std::vector<std::shared_ptr<arrow::ChunkedArray>> _columns;

        /**
         * Builders being used to construct columns
         */
        std::vector<std::shared_ptr<DBGenColumnBuilder>> _builders;
    };

};


#endif // DB_TABLE_HPP
