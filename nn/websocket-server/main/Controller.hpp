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

#ifndef _NN_CONTROLLER_HPP_
#define _NN_CONTROLLER_HPP_

#include <iostream>
#include <string>
#include <array>
#include <map>
#include <vector>
#include <functional>
#include <execution>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <fstream>

namespace controller {

std::map<std::string, std::function<int(std::string t)>> m {
    {
        "test",
        [](std::string message) -> int {
            std::system("cd ../parser && ./bazel-bin/main/example mnist > example.txt");
            std::cout << std::ifstream("../parser/example.txt").rdbuf();

            return 0;
        }
    },
    {
        "cluster",
        [](std::string message) -> int {

            std::array <std::string, 3> x = {"1", "2", "3"};

            std::for_each(std::execution::par_unseq, x.begin(), x.end(),
                [](auto v) {
                    std::string command("cd ../parser && ./bazel-bin/main/example > ");
                    std::string fileName("cluster");
                    auto files = command.append(fileName).append(v).append(".txt").c_str();
                    std::system(files);
                    std::string path("../parser/");
                    std::cout << std::ifstream(path.append(files)).rdbuf();
                });

            return 0;
        }
    },
};

int router(std::string message) {
    std::istringstream iss(message);
    std::string token;

    std::cout << "message: " << message << std::endl;
    std::getline(iss, token, '/');
    std::getline(iss, token, '/');
    auto result = m[token](message);

    return result;
}

}

#endif // _NN_CONTROLLER_HPP_