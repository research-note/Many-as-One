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

#include "tables/ScanTableCursor.hpp"

using namespace db;

ScanTableCursor::ScanTableCursor(
        std::shared_ptr<arrow::Table> table,
        std::vector<db::ColumnEncoding> encodings
    )
: _table(table)
{
    for (int i = 0; i < table->num_columns(); i++) {
        addColumn(table->column(i), table->field(i)->name(), encodings.at(i));
    }
    _size = (uint64_t) table->num_rows();
    reset();
}

ScanTableCursor::ScanTableCursor(
        std::shared_ptr<arrow::Table> table
)
        : _table(table)
{
    for (int i = 0; i < table->num_columns(); i++) {
        addColumn(table->column(i), table->field(i)->name(), db::ColumnEncoding::PLAIN);
    }
    _size = (uint64_t) table->num_rows();
    reset();
}

std::shared_ptr<BaseColumnCursor<db::StringType>>
ScanTableCursor::getStringColumn(std::string colName)
{
    if (_cursors.count(colName) == 0) return nullptr;
    return std::dynamic_pointer_cast<BaseColumnCursor<db::StringType>>(_cursors[colName]);
}

std::shared_ptr<BaseColumnCursor<db::LongType>>
ScanTableCursor::getLongColumn(std::string colName)
{
    if (_cursors.count(colName) == 0) return nullptr;
    return std::dynamic_pointer_cast<BaseColumnCursor<db::LongType>>(_cursors[colName]);
}

std::shared_ptr<BaseColumnCursor<db::HalfFloatType>>
ScanTableCursor::getHalfFloatColumn(std::string colName)
{
    if (_cursors.count(colName) == 0) return nullptr;
    return std::dynamic_pointer_cast<BaseColumnCursor<db::HalfFloatType>>(_cursors[colName]);
}

std::shared_ptr<BaseColumnCursor<db::FloatType>>
ScanTableCursor::getFloatColumn(std::string colName)
{
    if (_cursors.count(colName) == 0) return nullptr;
    return std::dynamic_pointer_cast<BaseColumnCursor<db::FloatType>>(_cursors[colName]);
}

std::shared_ptr<BaseColumnCursor<db::DoubleType>>
ScanTableCursor::getDoubleColumn(std::string colName)
{
    if (_cursors.count(colName) == 0) return nullptr;
    return std::dynamic_pointer_cast<BaseColumnCursor<db::DoubleType>>(_cursors[colName]);
}

std::shared_ptr<GenericColumnCursor>
ScanTableCursor::getColumn(std::string colName)
{
    if (_cursors.count(colName) == 0) return nullptr;
    return _cursors[colName];
}

bool
ScanTableCursor::addColumn(std::shared_ptr<arrow::ChunkedArray> column, const std::string& column_name, db::ColumnEncoding encoding)
{
    switch (column->type()->id()) {
    case arrow::Type::INT64:
        _cursors[column_name] = BaseColumnCursor<db::LongType>::makeCursor(column, encoding, *this);
        return true;

    case arrow::Type::HALF_FLOAT:
        _cursors[column_name] = BaseColumnCursor<db::HalfFloatType>::makeCursor(column, encoding, *this);
        return true;

    case arrow::Type::FLOAT:
        _cursors[column_name] = BaseColumnCursor<db::FloatType>::makeCursor(column, encoding, *this);
        return true;

    case arrow::Type::DOUBLE:
        _cursors[column_name] = BaseColumnCursor<db::DoubleType>::makeCursor(column, encoding, *this);
        return true;
    
    case arrow::Type::STRING:
        _cursors[column_name] = BaseColumnCursor<db::StringType>::makeCursor(column, encoding, *this);
        return true;
    
    default:
        return false;
    }

}

bool
ScanTableCursor::hasMore()
{
    if (_size == 0) return false;
    if (_initial_state) {
        _initial_state = false;
        _position = 0;
        return true;
    }
    if ((_position + 1) >= _size) return false;
    _position++;
    return true;
}

void
ScanTableCursor::reset()
{
    _position = 0;
    _initial_state = true;
    for (auto cursor =_cursors.begin(); cursor != _cursors.end(); cursor++) {
        cursor->second->reset();
    }
}

uint64_t
ScanTableCursor::getPosition() const
{
    return _position;
}
