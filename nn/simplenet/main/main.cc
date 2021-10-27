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
#include <string>

#include "lib/cross_entropy_error.hpp"
#include "lib/numerical_gradient.hpp"
#include "lib/softmax.hpp"

#include "main/NDGrid.hpp"
#include "main/Simplenet.hpp"

using namespace std;

int main(int argc, char** argv) {

    vector<double> y {
        0.1, 0.05, 0.6, 0.0, 0.05,
        0.1, 0.0, 0.1, 0.0, 0.0
    };

    vector<unsigned int> t { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };
    cross_entropy_error(y, t);
    softmax(y);

    numerical_gradient(
        [](auto v) -> auto { return pow(v, 2); }, y);

    NDGrid<float, 3> layerWeight (3);
    layerWeight[0][0][0] = 5;

    cout << "layerWeight.getSize(): " << layerWeight.getSize() << endl;
    cout << "layerWeight[2].getSize(): " << (layerWeight[2]).getSize() << endl;
    cout << "layerWeight[2][2].getSize(): " << (layerWeight[2][2]).getSize() << endl;
    cout << layerWeight[0][0][0] << endl;
    // layerWeight.randomize();

    return 0;
}
