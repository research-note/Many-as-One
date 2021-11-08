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

#ifndef _NN_CLASS_PARSER_HPP_
#define _NN_CLASS_PARSER_HPP_

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>

#include "CUpng.hpp"

using namespace std;

#define SIZE 784 // 28*28
#define NUM_TEST 10000

class CParser {
    public:
		// mnist
        void loadMnist();

		// png
		void loadPng(string fileName);
    private: 
		// mnist
        void readMnistImage(char *file_path, int num_data, int len_info, int arr_n, unsigned char data_char[][SIZE], int info_arr[]);
        void readMnistLabel(char *file_path, int num_data, int len_info, int arr_n, unsigned char data_char[][1], int info_arr[]);
        void imageChar2Double(int num_data, unsigned char data_image_char[][SIZE], double data_image[][SIZE]);
        void labelChar2Int(int num_data, unsigned char data_label_char[][1], int data_label[]);

		//png
};

#include "CParser.cc"

#endif // _NN_CLASS_PARSER_HPP_