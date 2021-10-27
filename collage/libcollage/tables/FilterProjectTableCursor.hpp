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

#ifndef FILTER_PROJECT_TABLECURSOR_HPP
#define FILTER_PROJECT_TABLECURSOR_HPP

#include "tables/TableCursor.hpp"
#include "core/DBSchema.hpp"

namespace db {

    class Filter;

    template <typename T>
    class BaseColumnCursor;

/**
 * Cursor for filtering and projecting a table. Compose with some other cursor.
 * Compose filters from classes like GreaterTHanFilter, LessThanFilter and AndFilter.
 */
    class FilterProjectTableCursor : public TableCursor {
    public:
        explicit FilterProjectTableCursor(TableCursor &source_cursor, std::shared_ptr<Filter> &filter);

        std::shared_ptr<BaseColumnCursor<db::StringType>> getStringColumn(std::string colName) override;

        std::shared_ptr<BaseColumnCursor<db::LongType>> getLongColumn(std::string colName) override;

        std::shared_ptr<BaseColumnCursor<db::DoubleType>> getDoubleColumn(std::string colName) override;

        std::shared_ptr<GenericColumnCursor> getColumn(std::string colName) override;

        bool hasMore() override;

        void reset() override;

    protected:
        uint64_t getPosition() const override;

        bool satisfiesFilter();

    private:
        TableCursor &_source_cursor;
        std::shared_ptr<Filter> _filter;

    };

};


#endif // FILTERPROJECTTABLECURSOR_HPP
