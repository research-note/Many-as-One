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

#include "main/CParser.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	CParser parser;

    string mnist("mnist");
    if(0 == mnist.compare(argv[1])) {
        parser.loadMnist();

        cout<<fixed;
        cout.precision(1);
        for (int i=0; i<28; ++i) {
            for (int j=0; j<28; ++j) {
                cout << test_image[0][(i * 28) + j] << " ";
            }
            cout << endl;
        }
    }

    string png("png");
    if(0 == png.compare(argv[1])) {
        parser.loadPng("./png_data/fruits.png");
    }

    // string grey("grey");
    // if(0 == png.compare(argv[1])) {
    //     parser.loadPng("./png_data/fruits.png");
    // }

    // // png
    // parser.loadPng("./png_data/fruits.png");

	return 0;
}
