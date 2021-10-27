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

#include "core/DBSchema.hpp"
#include "tables/FilterProjectTableCursor.hpp"
#include "filters/Filter.hpp"
#include "columns/BaseColumnCursor.hpp"

using namespace db;

FilterProjectTableCursor::FilterProjectTableCursor(TableCursor &source_cursor, std::shared_ptr<Filter> &filter)
        : _source_cursor(source_cursor), _filter(filter)
{
    reset();
    filter->initialize(*this);
}

std::shared_ptr<BaseColumnCursor<db::StringType>>
FilterProjectTableCursor::getStringColumn(std::string colName)
{
    return std::dynamic_pointer_cast<BaseColumnCursor<db::StringType>>(_source_cursor.getStringColumn(colName));
}

std::shared_ptr<BaseColumnCursor<db::LongType>>
FilterProjectTableCursor::getLongColumn(std::string colName)
{
    return std::dynamic_pointer_cast<BaseColumnCursor<db::LongType>>(_source_cursor.getLongColumn(colName));
}

std::shared_ptr<BaseColumnCursor<db::DoubleType>>
FilterProjectTableCursor::getDoubleColumn(std::string colName)
{
    return std::dynamic_pointer_cast<BaseColumnCursor<db::DoubleType>>(_source_cursor.getDoubleColumn(colName));
}

std::shared_ptr<GenericColumnCursor>
FilterProjectTableCursor::getColumn(std::string colName)
{
    return _source_cursor.getColumn(colName);
}

bool
FilterProjectTableCursor::hasMore()
{
    while (_source_cursor.hasMore()) {
        if (satisfiesFilter()) return true;
    }
    return false;
}

void FilterProjectTableCursor::reset()
{
    _source_cursor.reset();
}

uint64_t
FilterProjectTableCursor::getPosition() const
{
    return _source_cursor.getPosition();
}

bool
FilterProjectTableCursor::satisfiesFilter()
{
    return _filter->evaluate();
}