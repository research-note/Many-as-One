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

#include "CParser.hpp"

// set appropriate path for data
#define TEST_IMAGE (char *) "./mnist_data/t10k-images.idx3-ubyte"
#define TEST_LABEL (char *) "./mnist_data/t10k-labels.idx1-ubyte"

// #define SIZE 784 // 28*28
// #define NUM_TEST 10000
#define LEN_INFO_IMAGE 4
#define LEN_INFO_LABEL 2

#define MAX_IMAGESIZE 1280
#define MAX_NUM_OF_IMAGES 1

unsigned char image[MAX_NUM_OF_IMAGES][MAX_IMAGESIZE][MAX_IMAGESIZE];
int width[MAX_NUM_OF_IMAGES], height[MAX_NUM_OF_IMAGES];

int info_image[LEN_INFO_IMAGE];
int info_label[LEN_INFO_LABEL];

unsigned char test_image_char[NUM_TEST][SIZE];
unsigned char test_label_char[NUM_TEST][1];

double test_image[NUM_TEST][SIZE];
int test_label[NUM_TEST];

void CParser::loadMnist()
{
    readMnistImage(TEST_IMAGE, NUM_TEST, LEN_INFO_IMAGE, SIZE, test_image_char, info_image);
    imageChar2Double(NUM_TEST, test_image_char, test_image);

    readMnistLabel(TEST_LABEL, NUM_TEST, LEN_INFO_LABEL, 1, test_label_char, info_label);
    labelChar2Int(NUM_TEST, test_label_char, test_label);
}

void CParser::readMnistImage(char *file_path, int num_data, int len_info, int arr_n, unsigned char data_char[][SIZE], int info_arr[])
{
    int i, fd;

    if ((fd = open(file_path, O_RDONLY)) == -1) {
        fprintf(stderr, "couldn't open image file");
        exit(-1);
    }
    
    // read-in mnist numbers (pixels|labels)
    for (i=0; i<num_data; i++) {
        pread(fd, data_char[i], arr_n * sizeof(unsigned char), len_info * sizeof(int));
    }

    close(fd);
}

void CParser::imageChar2Double(int num_data, unsigned char data_image_char[][SIZE], double data_image[][SIZE])
{
    int i, j;
    for (i=0; i<num_data; i++)
        for (j=0; j<SIZE; j++)
            data_image[i][j]  = (double)data_image_char[i][j] / 255.0;
}

void CParser::readMnistLabel(char *file_path, int num_data, int len_info, int arr_n, unsigned char data_char[][1], int info_arr[])
{
    int i, fd;

    if ((fd = open(file_path, O_RDONLY)) == -1) {
        fprintf(stderr, "couldn't open image file");
        exit(-1);
    }
    
    // read-in mnist numbers (pixels|labels)
    for (i=0; i<num_data; i++) {
        pread(fd, data_char[i], arr_n * sizeof(unsigned char), len_info * sizeof(int));
    }

    close(fd);
}

void CParser::labelChar2Int(int num_data, unsigned char data_label_char[][1], int data_label[])
{
    int i;
    for (i=0; i<num_data; i++)
        data_label[i]  = (int)data_label_char[i][0];
}

void CParser::loadPng(string fileName) {
	CUpng upng;
	unsigned width, height, depth;
	unsigned x, y, d;

	upng.readFile(fileName);
	if (upng.upng_get_error() != EUpngError::UPNG_EOK) {
		printf("[getUpng] error: %u %u\n", upng.upng_get_error(), upng.upng_get_error_line());
		return;
	}

	upng.upng_decode();
	if (upng.upng_get_error() != EUpngError::UPNG_EOK) {
		printf("[upng_decode] error: %u %u\n", upng.upng_get_error(), upng.upng_get_error_line());
		return;
	}

	width = upng.width;
	height = upng.height;
	depth = upng.upng_get_bpp() / 8;

	printf("size:	%ux%ux%u (%lu)\n", width, height, upng.upng_get_bpp(), upng.size);
	printf("format:	%u\n", upng.format);

	if (upng.format == EUpngFormat::UPNG_RGB8 || upng.format == EUpngFormat::UPNG_RGBA8) {

		for (y = 0; y < height; ++y) {
			for (x = 0; x < width; ++x) {
                unsigned int index = (y * width + x) * depth;
				/* printf("( ");
				for (d = 0; d < depth; ++d) {
					printf("%d ", upng.buffer[index + d]);
				}
                printf(") -> "); */
                // Y = 0.299 * R + 0.587 * G + 0.114 * B
                float val = 0.0;
                val += 0.299 * upng.buffer[index];
                val += 0.587 * upng.buffer[index + 1];
                val += 0.114 * upng.buffer[index + 2];
                val /= 256;

                std::cout << "(" << val << ") " << std::endl;
			}
		}
		printf("\n\n-\n\n");
	}
}
