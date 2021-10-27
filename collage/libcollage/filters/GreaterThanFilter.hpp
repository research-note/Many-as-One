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

#ifndef GREATERTHANFILTER_HPP
#define GREATERTHANFILTER_HPP

#include <string>

#include "filters/Filter.hpp"
#include "tables/TableCursor.hpp"
#include "columns/BaseColumnCursor.hpp"

namespace db {

    template<typename T>
    class GreaterThanFilter : public Filter {
    public:

        GreaterThanFilter(std::string column_name, typename T::ElementType value);

        void initialize(TableCursor &table_cursor) override;

        bool evaluate() override;

    private:

        std::string _column_name;

        double _value;

        std::shared_ptr<BaseColumnCursor<T>> _cursor;
    };

};


#endif // GREATERTHANFILTER_HPP
