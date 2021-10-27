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

#include "columns/GenericColumnCursor.hpp"
#include "tables/TableCursor.hpp"

using namespace db;

GenericColumnCursor::GenericColumnCursor(TableCursor &table_cursor)
        : _table_cursor(table_cursor)
{

}

uint64_t
GenericColumnCursor::get_table_cursor_position()
{
    return _table_cursor.getPosition();
}