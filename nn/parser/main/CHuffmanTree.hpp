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

#ifndef _NN_CLASS_HUFFMAN_TREE_HPP_
#define _NN_CLASS_HUFFMAN_TREE_HPP_

class CHuffmanTree {
	public:
		CHuffmanTree();
		CHuffmanTree* setTree2d(unsigned* buffer);
		CHuffmanTree* setNumCodes(unsigned numcodes);
		CHuffmanTree* setMaxBitLen(unsigned maxbitlen);
		bool huffman_tree_create_lengths(const unsigned *bitlen);
		unsigned* tree2d;
		/*maximum number of bits a single code can get */
		unsigned maxbitlen;	
		/* number of symbols in the alphabet 
		 * = number of codes */
		unsigned numcodes;
};

#include "CHuffmanTree.cc"

#endif