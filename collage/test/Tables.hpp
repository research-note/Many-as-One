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

#ifndef TABLES_HPP
#define TABLES_HPP

#include "arrow/api.h"

#include "columns/ChunkedColumnCursor.hpp"
#include "tables/ScanTableCursor.hpp"
#include "tables/FilterProjectTableCursor.hpp"

class DBTable;

class Tables {
public:
    static db::Status createNoRows(std::shared_ptr<db::DBTable>& table,
                                      arrow::MemoryPool *pool = arrow::default_memory_pool());

    static db::Status createSmallSimpleColumns(std::shared_ptr<db::DBTable>& table,
                                                  arrow::MemoryPool *pool = arrow::default_memory_pool());
    static db::Status createSmallSimpleStringColumns(std::shared_ptr<db::DBTable>& table,
                                                        arrow::MemoryPool *pool = arrow::default_memory_pool());

    static db::Status createSmallChunkedColumns(std::shared_ptr<db::DBTable>& table,
                                                   arrow::MemoryPool *pool = arrow::default_memory_pool());
    static db::Status createSmallChunkedColumnsWithNulls(std::shared_ptr<db::DBTable>& table,
                                                            arrow::MemoryPool *pool = arrow::default_memory_pool());

    static db::Status createSimple(std::shared_ptr<db::DBTable>& table,
                                      arrow::MemoryPool *pool = arrow::default_memory_pool());

    static db::Status createSmallDictionaryColumns(std::shared_ptr<db::DBTable>& table,
                                                      arrow::MemoryPool *pool = arrow::default_memory_pool());
    static db::Status createSmallStringDictionaryColumns(std::shared_ptr<db::DBTable>& table,
                                                            arrow::MemoryPool *pool = arrow::default_memory_pool());
    static db::Status createChunkedDictionaryColumns(std::shared_ptr<db::DBTable>& table,
                                                        arrow::MemoryPool *pool = arrow::default_memory_pool());
    static db::Status createChunkedDictionaryColumnsWithNulls(std::shared_ptr<db::DBTable>& table,
                                                                 arrow::MemoryPool *pool = arrow::default_memory_pool());


};


#endif // TABLES_HPP
