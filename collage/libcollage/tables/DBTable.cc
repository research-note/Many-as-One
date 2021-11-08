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

#include <iostream>
#include <execution>
#include <algorithm>

#include "DBTable.hpp"

using arrow::Table;
using arrow::Field;

using namespace db;

DBTable::DBTable(std::vector<std::string> names,
                std::vector<std::shared_ptr<db::DataType>> types,
                std::vector<db::ColumnEncoding> encodings,
                arrow::MemoryPool *pool)
{
    _encodings = encodings;

    std::vector<std::shared_ptr<arrow::Field>> schema_vector;

    for (uint64_t i = 0; i < names.size(); i++) {
        std::shared_ptr<db::DataType> tp = types.at(i);
        std::shared_ptr<Field> field = arrow::field(names.at(i), tp->getArrowType());
        db::ColumnEncoding encoding = encodings.at(i);
        schema_vector.push_back(field);

        switch (tp->id()) {
            case db::ColumnType::LONG: {
                _builders.push_back(std::make_shared<DBColumnBuilder<db::LongType>>(field, encoding, pool));
                break;
            }
            case db::ColumnType::DOUBLE: {
                _builders.push_back(std::make_shared<DBColumnBuilder<db::DoubleType>>(field, encoding, pool));
                break;
            }
            case db::ColumnType::STRING: {
                _builders.push_back(std::make_shared<DBColumnBuilder<db::StringType>>(field, encoding, pool));
                break;
            }
            default:
                // TODO: handle this
                break;
        }
    }

    _schema = std::make_shared<arrow::Schema>(schema_vector);
}

Status
DBTable::create(std::vector<std::string> names,
                std::vector<std::shared_ptr<db::DataType>> types,
                std::vector<db::ColumnEncoding> encodings,
                std::shared_ptr<DBTable> *table,
                arrow::MemoryPool *pool)
{

    if ((names.size() != types.size()) || (names.size() != encodings.size())) {
        return Status(StatusCode::UnevenArgLists);
    }

    for (uint64_t i = 0; i < names.size(); i++) {
        Status status = checkCompatible(types.at(i), encodings.at(i));
        if (!status.ok()) {
            return status;
        }
    }

    db::DBTable *pTable = new db::DBTable(names, types, encodings, pool);

    table->reset(pTable);

    return Status::OK();
}

Status
DBTable::checkCompatible(std::shared_ptr<db::DataType> dataType,
                         db::ColumnEncoding encoding)
{
    switch (dataType->id()) {
    case ColumnType::DOUBLE:
        if (encoding == ColumnEncoding::PLAIN) {
            return Status::OK();
        } else {
            return Status(StatusCode::InvalidColumn);
        }
    case ColumnType::LONG:
        return Status::OK();
    case ColumnType::STRING:
        return Status::OK();
    }

    return Status(StatusCode::InternalError);
}

Status
DBTable::addRow(std::vector<std::shared_ptr<db::GenValue>> values)
{
    for (size_t i = 0; i < values.size(); i++) {
        auto builder = _builders.at(i);
        auto v = values.at(i);
        DB_RETURN_NOT_OK(builder->add(v));
    }

    return Status::OK();
}

Status
DBTable::updateRow(std::shared_ptr<db::TableCursor> tc,
                    std::vector<std::string> names,
                    std::vector<std::shared_ptr<db::GenValue>> values)
{
    // if(names.size() != values.size()) {
    //     return Status::invalidColumn();
    // }

    // for (size_t i = 0; i < names.size(); i++) {
    //     std::shared_ptr<GenericColumnCursor> col = tc->getColumn(names.at(i));
    //     std::cout << '[' << i << ']' << " Update From: " 
    //         << names.at(i) << ':' << values.at(i) << '\n';
    //     std::cout << '[' << i << ']' << " Update To: " 
    //         << names.at(i) << ':' << values.at(i) << '\n';
    // }

    return Status::OK();
}

Status
DBTable::endChunk()
{
    for (auto builder : _builders) {
        DB_RETURN_NOT_OK(builder->endChunk());
    }
    return Status::OK();
}

Status
DBTable::make() {
    _columns.clear();
    for (auto builder : _builders) {
        _columns.push_back(builder->getColumn());
    }
    _table = Table::Make(_schema, _columns);
    return Status::OK();
}

std::shared_ptr<ScanTableCursor>
DBTable::getScanCursor() const
{
    std::shared_ptr<ScanTableCursor> tc = std::make_shared<ScanTableCursor>(_table, _encodings);
    return tc;
}

std::shared_ptr<arrow::Table>
DBTable::getTable() const
{
    return _table;
}

Status
DBTable::dump() const
{
    for (int i = 0; i < _table->num_columns(); i++) {
        std::shared_ptr<arrow::ChunkedArray> col = _table->column(i);
        std::cout << "*** COLUMN " << i << " : " << _table->field(i)->name() << '\n';
        std::cout << "Num chunks: " << col->num_chunks() << '\n';
        for (int c = 0; c < col->num_chunks(); c++) {
            std::cout << "Chunk " << c
                      << " length: " << col->chunk(c)->length()
                      << " null count: " << col->chunk(c)->null_count()
                      << '\n';
            std::shared_ptr<arrow::DictionaryArray> da = std::dynamic_pointer_cast<arrow::DictionaryArray>(
                    col->chunk(c));
            if (da == nullptr) {
                std::cout << "SIMPLE array length: " << col->chunk(c) << '\n';
            } else {
                std::cout << "CHUNKED dict array: " << da.get() << '\n';
                std::cout << "dict array length: " << da->length() << '\n';
                std::cout << "dict array dict length: " << da->dictionary()->length() << '\n';
                std::cout << "dict array indices length: " << da->indices()->length() << '\n';
                std::cout << "dict array dictionary type: " << da->dictionary()->type_id() << '\n';
                std::cout << "dict array indices type: " << da->indices()->type_id() << '\n';

                std::shared_ptr<arrow::StringArray> stringDict =
                        std::dynamic_pointer_cast<arrow::StringArray>(da->dictionary());
                if (stringDict != nullptr) {
                    for (int de = 0; de < stringDict->length(); de++) {
                        std::cout << "Dict entry " << de << " : " << stringDict->GetString(de) << '\n';
                    }
                }

                std::shared_ptr<arrow::NumericArray<arrow::Int8Type>> indices =
                        std::dynamic_pointer_cast<arrow::NumericArray<arrow::Int8Type>>(da->indices());
                if (indices != nullptr) {
                    for (int ie = 0; ie < indices->length(); ie++) {
                        if (indices->IsNull(ie)) {
                            std::cout << "Index entry " << ie << " : IS NULL" << '\n';
                        } else {
                            std::cout << "Index entry " << ie << " : " << (int)indices->Value(ie) << '\n';
                        }

                    }
                }

            }
        }

    }

    return Status::OK();
}
