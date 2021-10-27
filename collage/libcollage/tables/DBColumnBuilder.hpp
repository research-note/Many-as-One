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

#ifndef DB_COLUMN_BUILDER_HPP
#define DB_COLUMN_BUILDER_HPP

#include <arrow/api.h>

namespace db {

    class DBGenColumnBuilder {

    public:

        virtual Status add(std::shared_ptr<db::GenValue> value) = 0;

        virtual Status endChunk() = 0;

        virtual std::shared_ptr<arrow::ChunkedArray> getColumn() = 0;

        virtual ~DBGenColumnBuilder() = default;

    private:

        virtual Status addNull() = 0;
    };

    template<typename T>
    class DBColumnBuilder : public DBGenColumnBuilder {

    public:

        explicit DBColumnBuilder(std::shared_ptr<arrow::Field> field,
                                 db::ColumnEncoding encoding,
                                 arrow::MemoryPool *pool = arrow::default_memory_pool());

        Status add(std::shared_ptr<db::GenValue> value) override;

        Status endChunk() override;

        std::shared_ptr<arrow::ChunkedArray> getColumn() override;

    protected:

        Status add(typename T::ElementType element);

        Status addNull() override;

    private:

        bool _haveData;

        db::ColumnEncoding _encoding;

        std::unique_ptr<typename T::BuilderType> _builder;
        std::unique_ptr<typename T::DictionaryBuilderType> _dictBuilder;

        arrow::ArrayVector _chunks;

        std::shared_ptr<arrow::Field> _field;
    };

};


#endif // DB_COLUMN_BUILDER_HPP
