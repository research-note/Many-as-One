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

#ifndef IS_NULL_FILTER_HPP
#define IS_NULL_FILTER_HPP

#include <string>

#include "filters/Filter.hpp"
#include "tables/TableCursor.hpp"
#include "columns/BaseColumnCursor.hpp"

namespace db {

    class IsNullFilter : public Filter {
        public:

            explicit IsNullFilter(std::string column_name);

            void initialize(TableCursor &table_cursor) override;

            bool evaluate() override;

        private:

            std::string _column_name;

            std::shared_ptr <GenericColumnCursor> _cursor;

    };
};


#endif //IS_NULL_FILTER_HPP
