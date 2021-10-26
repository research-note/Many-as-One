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

#include "CUpng.cc"

#endif