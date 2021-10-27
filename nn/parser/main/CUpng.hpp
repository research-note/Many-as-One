#ifndef _CLASS_UPNG_HPP_
#define _CLASS_UPNG_HPP_

#include "CHuffmanTree.hpp"
#include <climits>

using namespace std;

enum class EUpngState {
	UPNG_ERROR		= -1,
	UPNG_DECODED	= 0,
	UPNG_HEADER		= 1,
	UPNG_NEW		= 2
};

enum class EUpngColor {
	UPNG_LUM		= 0,
	UPNG_RGB		= 2,
	UPNG_INDX		= 3,
	UPNG_LUMA		= 4,
	UPNG_RGBA		= 6
};

enum class EUpngError {
	/* success (no error) */
	UPNG_EOK			= 0,
	/* memory allocation failed */
	UPNG_ENOMEM			= 1,
	/* resource not found (file missing) */
	UPNG_ENOTFOUND		= 2,
	/* image data does not have a PNG header */
	UPNG_ENOTPNG		= 3,
	/* image data is not a valid PNG image */
	UPNG_EMALFORMED		= 4,
	/* critical PNG chunk type is not supported */
	UPNG_EUNSUPPORTED	= 5,
	/* image interlacing is not supported */
	UPNG_EUNINTERLACED	= 6,
	/* image color format is not supported */
	UPNG_EUNFORMAT		= 7,
	/* invalid parameter to method call */
	UPNG_EPARAM			= 8
};

enum class EUpngFormat {
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
};

typedef struct upng_source {
	const unsigned char*	buffer;
	unsigned long			size;
	char					owning;
} upng_source;

#define MAKE_BYTE(b) ((b) & 0xFF)
#define MAKE_DWORD(a,b,c,d) ((MAKE_BYTE(a) << 24) | (MAKE_BYTE(b) << 16) | (MAKE_BYTE(c) << 8) | MAKE_BYTE(d))
#define MAKE_DWORD_PTR(p) MAKE_DWORD((p)[0], (p)[1], (p)[2], (p)[3])

#define CHUNK_IHDR MAKE_DWORD('I','H','D','R')
#define CHUNK_IDAT MAKE_DWORD('I','D','A','T')
#define CHUNK_IEND MAKE_DWORD('I','E','N','D')
#define CHUNK_PLTE MAKE_DWORD('P','L','T','E')

#define FIRST_LENGTH_CODE_INDEX 257
#define LAST_LENGTH_CODE_INDEX 285

#define NUM_DEFLATE_CODE_SYMBOLS 288	/*256 literals, the end code, some length codes, and 2 unused codes */
#define NUM_DISTANCE_SYMBOLS 32	/* the distance codes have their own symbols, 30 used, 2 unused */
#define NUM_CODE_LENGTH_CODES 19	/*the code length codes. 0-15: code lengths, 16: copy previous 3-6 times, 17: 3-10 zeros, 18: 11-138 zeros */

#define DEFLATE_CODE_BITLEN 15
#define DISTANCE_BITLEN 15
#define CODE_LENGTH_BITLEN 7

#define DEFLATE_CODE_BUFFER_SIZE (NUM_DEFLATE_CODE_SYMBOLS * 2)
#define DISTANCE_BUFFER_SIZE (NUM_DISTANCE_SYMBOLS * 2)
#define CODE_LENGTH_BUFFER_SIZE (NUM_DISTANCE_SYMBOLS * 2)

#define upng_chunk_length(chunk) MAKE_DWORD_PTR(chunk)
#define upng_chunk_type(chunk) MAKE_DWORD_PTR((chunk) + 4)
#define upng_chunk_critical(chunk) (((chunk)[4] & 32) == 0)

const unsigned LENGTH_BASE[29] = {	/*the base lengths represented by codes 257-285 */
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
	67, 83, 99, 115, 131, 163, 195, 227, 258
};

const unsigned LENGTH_EXTRA[29] = {	/*the extra bits used by codes 257-285 (added to base length) */
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
	5, 5, 5, 0
};

const unsigned FIXED_DISTANCE_TREE[NUM_DISTANCE_SYMBOLS * 2] = {
	33, 48, 34, 41, 35, 38, 36, 37, 0, 1, 2, 3, 39, 40, 4, 5, 6, 7, 42, 45, 43,
	44, 8, 9, 10, 11, 46, 47, 12, 13, 14, 15, 49, 56, 50, 53, 51, 52, 16, 17,
	18, 19, 54, 55, 20, 21, 22, 23, 57, 60, 58, 59, 24, 25, 26, 27, 61, 62, 28,
	29, 30, 31, 0, 0
};

const unsigned DISTANCE_BASE[30] = {	/*the base backwards distances (the bits of distance codes appear after length codes and use their own huffman tree) */
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
	769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
};

const unsigned DISTANCE_EXTRA[30] = {	/*the extra bits of backwards distances (added to base) */
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
	11, 11, 12, 12, 13, 13
};

/*the order in which "code length alphabet code lengths" are stored, out of this the huffman tree of the dynamic huffman tree lengths is generated */
const unsigned CLCL[NUM_CODE_LENGTH_CODES] = { 
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 
};

const unsigned FIXED_DEFLATE_CODE_TREE[NUM_DEFLATE_CODE_SYMBOLS * 2] = {
	289, 370, 290, 307, 546, 291, 561, 292, 293, 300, 294, 297, 295, 296, 0, 1,
	2, 3, 298, 299, 4, 5, 6, 7, 301, 304, 302, 303, 8, 9, 10, 11, 305, 306, 12,
	13, 14, 15, 308, 339, 309, 324, 310, 317, 311, 314, 312, 313, 16, 17, 18,
	19, 315, 316, 20, 21, 22, 23, 318, 321, 319, 320, 24, 25, 26, 27, 322, 323,
	28, 29, 30, 31, 325, 332, 326, 329, 327, 328, 32, 33, 34, 35, 330, 331, 36,
	37, 38, 39, 333, 336, 334, 335, 40, 41, 42, 43, 337, 338, 44, 45, 46, 47,
	340, 355, 341, 348, 342, 345, 343, 344, 48, 49, 50, 51, 346, 347, 52, 53,
	54, 55, 349, 352, 350, 351, 56, 57, 58, 59, 353, 354, 60, 61, 62, 63, 356,
	363, 357, 360, 358, 359, 64, 65, 66, 67, 361, 362, 68, 69, 70, 71, 364,
	367, 365, 366, 72, 73, 74, 75, 368, 369, 76, 77, 78, 79, 371, 434, 372,
	403, 373, 388, 374, 381, 375, 378, 376, 377, 80, 81, 82, 83, 379, 380, 84,
	85, 86, 87, 382, 385, 383, 384, 88, 89, 90, 91, 386, 387, 92, 93, 94, 95,
	389, 396, 390, 393, 391, 392, 96, 97, 98, 99, 394, 395, 100, 101, 102, 103,
	397, 400, 398, 399, 104, 105, 106, 107, 401, 402, 108, 109, 110, 111, 404,
	419, 405, 412, 406, 409, 407, 408, 112, 113, 114, 115, 410, 411, 116, 117,
	118, 119, 413, 416, 414, 415, 120, 121, 122, 123, 417, 418, 124, 125, 126,
	127, 420, 427, 421, 424, 422, 423, 128, 129, 130, 131, 425, 426, 132, 133,
	134, 135, 428, 431, 429, 430, 136, 137, 138, 139, 432, 433, 140, 141, 142,
	143, 435, 483, 436, 452, 568, 437, 438, 445, 439, 442, 440, 441, 144, 145,
	146, 147, 443, 444, 148, 149, 150, 151, 446, 449, 447, 448, 152, 153, 154,
	155, 450, 451, 156, 157, 158, 159, 453, 468, 454, 461, 455, 458, 456, 457,
	160, 161, 162, 163, 459, 460, 164, 165, 166, 167, 462, 465, 463, 464, 168,
	169, 170, 171, 466, 467, 172, 173, 174, 175, 469, 476, 470, 473, 471, 472,
	176, 177, 178, 179, 474, 475, 180, 181, 182, 183, 477, 480, 478, 479, 184,
	185, 186, 187, 481, 482, 188, 189, 190, 191, 484, 515, 485, 500, 486, 493,
	487, 490, 488, 489, 192, 193, 194, 195, 491, 492, 196, 197, 198, 199, 494,
	497, 495, 496, 200, 201, 202, 203, 498, 499, 204, 205, 206, 207, 501, 508,
	502, 505, 503, 504, 208, 209, 210, 211, 506, 507, 212, 213, 214, 215, 509,
	512, 510, 511, 216, 217, 218, 219, 513, 514, 220, 221, 222, 223, 516, 531,
	517, 524, 518, 521, 519, 520, 224, 225, 226, 227, 522, 523, 228, 229, 230,
	231, 525, 528, 526, 527, 232, 233, 234, 235, 529, 530, 236, 237, 238, 239,
	532, 539, 533, 536, 534, 535, 240, 241, 242, 243, 537, 538, 244, 245, 246,
	247, 540, 543, 541, 542, 248, 249, 250, 251, 544, 545, 252, 253, 254, 255,
	547, 554, 548, 551, 549, 550, 256, 257, 258, 259, 552, 553, 260, 261, 262,
	263, 555, 558, 556, 557, 264, 265, 266, 267, 559, 560, 268, 269, 270, 271,
	562, 565, 563, 564, 272, 273, 274, 275, 566, 567, 276, 277, 278, 279, 569,
	572, 570, 571, 280, 281, 282, 283, 573, 574, 284, 285, 286, 287, 0, 0
};

class CUpng {
    public:
        unsigned int 	width;
        unsigned int 	height;
        EUpngColor		color_type;
        unsigned		color_depth;
        EUpngFormat		format;
        unsigned char*	buffer;
        unsigned long	size;
        unsigned char*	palette;
        EUpngError		error;
        unsigned int	error_line;
        EUpngState		state;
        upng_source		source;

        CUpng();
		void 			readFile(string filename);
		EUpngError 		upng_get_error();
		unsigned int 	upng_get_error_line();
		void 			upng_decode();
		unsigned upng_get_bpp();
	private:
		void SET_ERROR(EUpngError code);
		void upng_header();
		EUpngFormat determine_format();
		unsigned upng_get_components();
		void uz_inflate(
			unsigned char *out, 
			unsigned long outsize, 
			const unsigned char *in, 
			unsigned long insize
		);
		void uz_inflate_data(
			unsigned char* out, 
			unsigned long outsize, 
			const unsigned char *in, 
			unsigned long insize, 
			unsigned long inpos
		);
		void inflate_uncompressed(
			unsigned char* out, 
			unsigned long outsize, 
			const unsigned char *in, 
			unsigned long *bp, 
			unsigned long *pos, 
			unsigned long inlength
		);
		void inflate_huffman(
			unsigned char* out, 
			unsigned long outsize, 
			const unsigned char *in, 
			unsigned long *bp, 
			unsigned long *pos, 
			unsigned long inlength, 
			unsigned btype
		);
		void get_tree_inflate_dynamic(
			CHuffmanTree &codetree, 
			CHuffmanTree &codetreeD, 
			CHuffmanTree &codelengthcodetree, 
			const unsigned char *in, 
			unsigned long *bp, 
			unsigned long inlength
		);
		unsigned char read_bit(
			unsigned long *bitpointer, 
			const unsigned char *bitstream
		);
		unsigned read_bits(
			unsigned long *bitpointer, 
			const unsigned char *bitstream, 
			unsigned long nbits
		);
		unsigned huffman_decode_symbol(
			const unsigned char *in, 
			unsigned long *bp, 
			const CHuffmanTree &codetree, 
			unsigned long inlength
		);
		void post_process_scanlines(
			unsigned char *out, 
			unsigned char *in
		);
		void unfilter(
			unsigned char *out, 
			const unsigned char *in, 
			unsigned w, 
			unsigned h, 
			unsigned bpp
		);
		void unfilter_scanline(
			unsigned char *recon, 
			const unsigned char *scanline, 
			const unsigned char *precon, 
			unsigned long bytewidth, 
			unsigned char filterType, 
			unsigned long length
		);
		int paeth_predictor(int a, int b, int c);
		void remove_padding_bits(
			unsigned char *out, 
			const unsigned char *in, 
			unsigned long olinebits, 
			unsigned long ilinebits, 
			unsigned h
		);
		void upng_free_source();
};

#include "CUpng.cc"

#endif // _CLASS_UPNG_HPP_