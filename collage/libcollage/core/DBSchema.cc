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

std::shared_ptr<db::DataType>
db::string_type()
{
    return std::make_shared<::db::StringType>();
}

std::shared_ptr<db::DataType>
db::long_type()
{
    return std::make_shared<::db::LongType>();
}

std::shared_ptr<db::DataType>
db::half_float_type()
{
    return std::make_shared<::db::HalfFloatType>();
}

std::shared_ptr<db::DataType>
db::float_type()
{
    return std::make_shared<::db::FloatType>();
}

std::shared_ptr<db::DataType>
db::double_type()
{
    return std::make_shared<::db::DoubleType>();
}

std::shared_ptr<db::GenValue>
db::long_val(int64_t i)
{
    return std::make_shared<::db::Value<int64_t>>(i);
}

std::shared_ptr<db::GenValue>
db::half_float_val(float d)
{
    return std::make_shared<::db::Value<float>>(d);
}

std::shared_ptr<db::GenValue>
db::float_val(float d)
{
    return std::make_shared<::db::Value<float>>(d);
}

std::shared_ptr<db::GenValue>
db::double_val(double d)
{
    return std::make_shared<::db::Value<double>>(d);
}

std::shared_ptr<db::GenValue>
db::string_val(std::string s)
{
    return std::make_shared<::db::Value<std::string>>(s);
}

std::shared_ptr<db::GenValue>
db::null_val()
{
    return std::make_shared<::db::NullValue>();
}
