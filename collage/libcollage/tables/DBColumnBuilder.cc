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

#include "columns/GenericColumnCursor.hpp"
#include "core/DBSchema.hpp"
#include "core/Status.hpp"
#include "tables/DBColumnBuilder.hpp"

using namespace db;

template <typename T>
DBColumnBuilder<T>::DBColumnBuilder(std::shared_ptr<arrow::Field> field,
                                    db::ColumnEncoding encoding,
                                    arrow::MemoryPool *pool)
{
    _encoding = encoding;
    if (encoding == db::ColumnEncoding::DICT) {
        _dictBuilder.reset(new typename T::DictionaryBuilderType(pool));
    } else {
        _builder.reset(new typename T::BuilderType(pool));
    }
    _haveData = false;
    _field = field;
}

template < typename T>
Status
DBColumnBuilder<T>::add(std::shared_ptr<db::GenValue> value) {
    // it may be a NullValue
    if (std::dynamic_pointer_cast<db::NullValue>(value) != nullptr) {
        DB_RETURN_NOT_OK(addNull());
    } else {
        DB_RETURN_NOT_OK(add(std::dynamic_pointer_cast<db::Value<typename T::ElementType>>(value)->get()));
    }
    return Status::OK();
}

template <typename T>
Status
DBColumnBuilder<T>::addNull()
{
    _haveData = true;
    if (_encoding == db::ColumnEncoding::DICT) {
        arrow::Status status = _dictBuilder->AppendNull();
        if (!status.ok()) return db::Status(db::StatusCode::OutOfMemory);
    } else {
        arrow::Status status = _builder->AppendNull();
        if (!status.ok()) return db::Status(db::StatusCode::OutOfMemory);
    }
    return Status::OK();
}

template <typename T>
Status
DBColumnBuilder<T>::add(typename T::ElementType element)
{
    _haveData = true;
    if (_encoding == db::ColumnEncoding::DICT) {
        arrow::Status status = _dictBuilder->Append(element);
        if (!status.ok()) return db::Status(db::StatusCode::OutOfMemory);
    } else {
        arrow::Status status = _builder->Append(element);
        if (!status.ok()) return db::Status(db::StatusCode::OutOfMemory);
    }
    return Status::OK();
}

template <typename T>
Status
DBColumnBuilder<T>::endChunk()
{
    // Arrow doesn't like to create columns with no chunks so if we don't have any at all
    // we create an empty one
    if (_haveData || _chunks.size() == 0) {
        if (_encoding == db::ColumnEncoding::DICT) {
            std::shared_ptr<arrow::Array> array;
            arrow::Status status = _dictBuilder->Finish(&array);
            if (!status.ok()) return db::Status(db::StatusCode::OutOfMemory);
            _chunks.push_back(array);
        } else {
            std::shared_ptr<arrow::Array> array;
            arrow::Status status = _builder->Finish(&array);
            if (!status.ok()) return db::Status(db::StatusCode::OutOfMemory);
            _chunks.push_back(array);
        }
        _haveData = false;
    }
    return Status::OK();
}


template <typename T>
std::shared_ptr<arrow::ChunkedArray>
DBColumnBuilder<T>::getColumn()
{
    endChunk(); // just in case it wasn't explicitly called
    // std::cout << "Finishing [" << _chunks.size() << "]" << std::endl;
    return std::make_shared<arrow::ChunkedArray>(std::move(_chunks));
}

template class db::DBColumnBuilder<db::LongType>;
template class db::DBColumnBuilder<db::DoubleType>;
template class db::DBColumnBuilder<db::StringType>;