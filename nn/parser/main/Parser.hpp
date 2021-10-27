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

#ifndef _NN_PARSER_HPP_
#define _NN_PARSER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

#define SIZE 784 // 28*28
#define NUM_TEST 10000

using namespace std;

class Parser {
    public:
        Parser();
        ~Parser();
        void loadMnist();
		void loadPng(string fileName);
    private: 
        void readMnistImage(char *file_path, int num_data, int len_info, int arr_n, unsigned char data_char[][SIZE], int info_arr[]);
        void readMnistLabel(char *file_path, int num_data, int len_info, int arr_n, unsigned char data_char[][1], int info_arr[]);
        void imageChar2Double(int num_data, unsigned char data_image_char[][SIZE], double data_image[][SIZE]);
        void labelChar2Int(int num_data, unsigned char data_label_char[][1], int data_label[]);
};

#include "Parser.cc"

#endif // _NN_PARSER_HPP_


#if !defined(UPNG_H)
#define UPNG_H

typedef enum upng_error {
	UPNG_EOK			= 0, /* success (no error) */
	UPNG_ENOMEM			= 1, /* memory allocation failed */
	UPNG_ENOTFOUND		= 2, /* resource not found (file missing) */
	UPNG_ENOTPNG		= 3, /* image data does not have a PNG header */
	UPNG_EMALFORMED		= 4, /* image data is not a valid PNG image */
	UPNG_EUNSUPPORTED	= 5, /* critical PNG chunk type is not supported */
	UPNG_EUNINTERLACED	= 6, /* image interlacing is not supported */
	UPNG_EUNFORMAT		= 7, /* image color format is not supported */
	UPNG_EPARAM			= 8  /* invalid parameter to method call */
} upng_error;

typedef enum upng_format {
	UPNG_BADFORMAT,
	UPNG_RGB8,
	UPNG_RGB16,
	UPNG_RGBA8,
	UPNG_RGBA16,
	UPNG_LUMINANCE1,
	UPNG_LUMINANCE2,
	UPNG_LUMINANCE4,
	UPNG_LUMINANCE8,
	UPNG_LUMINANCE_ALPHA1,
	UPNG_LUMINANCE_ALPHA2,
	UPNG_LUMINANCE_ALPHA4,
	UPNG_LUMINANCE_ALPHA8,
	UPNG_INDEX1,
	UPNG_INDEX2,
	UPNG_INDEX4,
	UPNG_INDEX8
} upng_format;

typedef struct upng_t upng_t;

upng_t*		upng_new_from_bytes	(const unsigned char* buffer, unsigned long size);
upng_t*		upng_new_from_file	(const char* path);
void		upng_free			(upng_t* upng);

upng_error	upng_header			(upng_t* upng);
upng_error	upng_decode			(upng_t* upng);

upng_error	upng_get_error		(const upng_t* upng);
unsigned	upng_get_error_line	(const upng_t* upng);

unsigned	upng_get_width		(const upng_t* upng);
unsigned	upng_get_height		(const upng_t* upng);
unsigned	upng_get_bpp		(const upng_t* upng);
unsigned	upng_get_bitdepth	(const upng_t* upng);
unsigned	upng_get_components	(const upng_t* upng);
unsigned	upng_get_pixelsize	(const upng_t* upng);
upng_format	upng_get_format		(const upng_t* upng);

const unsigned char*	upng_get_buffer		(const upng_t* upng);
unsigned				upng_get_size		(const upng_t* upng);
const unsigned char*	upng_get_palette	(const upng_t* upng);

#endif /*defined(UPNG_H)*/