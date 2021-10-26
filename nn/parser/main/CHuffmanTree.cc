#include "CHuffmanTree.hpp"

CHuffmanTree::CHuffmanTree() {
    this->tree2d = NULL;
    this->maxbitlen = 0;
    this->numcodes = 0;
}

CHuffmanTree* CHuffmanTree::setTree2d(unsigned* buffer) {
	this->tree2d = buffer;
    return this;
}

CHuffmanTree* CHuffmanTree::setNumCodes(unsigned numcodes) {
	this->numcodes = numcodes;
    return this;
}

CHuffmanTree* CHuffmanTree::setMaxBitLen(
    unsigned maxbitlen
) {
	this->maxbitlen = maxbitlen;
    return this;
}

/* given the code lengths (as stored in the PNG file), 
 * generate the tree as defined by Deflate. 
 * maxbitlen is the maximum bits that a code 
 * in the tree can have. return value is error.*/
bool CHuffmanTree::huffman_tree_create_lengths(
    const unsigned *bitlen
) {
	unsigned tree1d[MAX_SYMBOLS];
	unsigned blcount[MAX_BIT_LENGTH];
	unsigned nextcode[MAX_BIT_LENGTH+1];
	unsigned bits, n, i;
    /*up to which node it is filled */
	unsigned nodefilled = 0;	
    /*position in the tree (1 of the numcodes columns) */
	unsigned treepos = 0;	

	/* initialize local vectors */
	memset(blcount, 0, sizeof(blcount));
	memset(nextcode, 0, sizeof(nextcode));

	/*step1:count number of instances of each code length */
	for (bits = 0; bits < this->numcodes; bits++) {
		blcount[bitlen[bits]]++;
	}

	/*step 2: generate the nextcode values */
	for (bits = 1; bits <= this->maxbitlen; bits++) {
		nextcode[bits] = 
            (nextcode[bits - 1] + blcount[bits - 1]) << 1;
	}

	/*step 3: generate all the codes */
	for (n = 0; n < this->numcodes; n++) {
		if (bitlen[n] != 0) {
			tree1d[n] = nextcode[bitlen[n]]++;
		}
	}

	/* convert tree1d[] to tree2d[][]. In the 2D array, 
     * a value of 32767 means uninited, a value >= numcodes 
     * is an address to another bit, 
     * a value < numcodes is a code. 
     * The 2 rows are the 2 possible bit values (0 or 1), 
     * there are as many columns as codes - 1
     * a good huffmann tree has N * 2 - 1 nodes, 
     * of which N - 1 are internal nodes. 
     * Here, the internal nodes are stored 
     * (what their 0 and 1 option point to). 
     * There is only memory for such good tree currently, 
     * if there are more nodes 
     * (due to too long length codes), 
     * error 55 will happen */
	for (n = 0; n < this->numcodes * 2; n++) {
        /* 32767 here means the tree2d 
         * isn't filled there yet */
		this->tree2d[n] = 32767;	
	}

	for (n = 0; n < this->numcodes; n++) {	/*the codes */
        /*the bits for this code */
		for (i = 0; i < bitlen[n]; i++) {	
			unsigned char bit = (unsigned char)(
                (tree1d[n] >> (bitlen[n] - i - 1)) & 1
            );
			/* check if oversubscribed */
			if (treepos > this->numcodes - 2) {
				return false;
			}

            /* not yet filled in */
			if (this->tree2d[2 * treepos + bit] == 32767) {	
				if (i + 1 == bitlen[n]) {	/*last bit */
                    /*put the current code in it */
					this->tree2d[2 * treepos + bit] = n;	
					treepos = 0;
				} else {	
                    /* put address of the next step in here, 
                     * first that address 
                     * has to be found of course 
                     * (it's just nodefilled + 1)... */
					nodefilled++;
                    /* addresses encoded with numcodes 
                     * added to it */
					this->tree2d[2 * treepos + bit] = 
                        nodefilled + this->numcodes;	
					treepos = nodefilled;
				}
			} else {
				treepos = this->tree2d[2 * treepos + bit] 
                            - this->numcodes;
			}
		}
	}

	for (n = 0; n < this->numcodes * 2; n++) {
		if (this->tree2d[n] == 32767) {
            /*remove possible remaining 32767's */
			this->tree2d[n] = 0;	
		}
	}

    return true;
}