#include "CUpng.hpp"

CUpng::CUpng() {
	buffer = NULL;
	size = 0;

	width = height = 0;

	color_type = EUpngColor::UPNG_RGBA;
	color_depth = 8;
	format = EUpngFormat::UPNG_RGBA8;

	state = EUpngState::UPNG_NEW;

	error = EUpngError::UPNG_EOK;
	error_line = 0;

	source.buffer = NULL;
	source.size = 0;
	source.owning = 0;
}

void CUpng::readFile(string filename) {
	unsigned char *buffer;
	FILE *file;
	long size;

	file = fopen(filename.c_str(), "rb");
	if (file == NULL) {
		SET_ERROR(EUpngError::UPNG_ENOTFOUND);
		return;
	}

	/* get filesize */
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	/* read contents of the file into the vector */
	buffer = (unsigned char *)malloc((unsigned long)size);
	if (buffer == NULL) {
		fclose(file);
		SET_ERROR(EUpngError::UPNG_ENOMEM);
		return;
	}
	size_t ret =fread(buffer, 1, (unsigned long)size, file);
    if (ret != size) {
        fprintf(stderr, "fread() failed: %zu\n", ret);
        exit(EXIT_FAILURE);
    }
	fclose(file);

	/* set the read buffer as our source buffer, 
	 * with owning flag set */
	this->source.buffer = buffer;
	this->source.size = size;
	this->source.owning = 1;
}

EUpngError CUpng::upng_get_error() {
	return error;
}

unsigned int CUpng::upng_get_error_line() {
	return error_line;
}

void CUpng::upng_decode() {
	const unsigned char *chunk;
	unsigned char* compressed;
	unsigned char* inflated;
	unsigned char* palette = NULL;
	unsigned long compressed_size = 0, compressed_index = 0;
	unsigned long palette_size = 0;
	unsigned long inflated_size;

	/* if we have an error state, bail now */
	if (this->error != EUpngError::UPNG_EOK) {
		return;
	}

	/* parse the main header, if necessary */
	upng_header();
	if (this->error != EUpngError::UPNG_EOK) {
		return;
	}

	/* if the state is not HEADER 
	 * (meaning we are ready to decode the image), 
	 * stop now */
	if (this->state != EUpngState::UPNG_HEADER) {
		return;
	}

	/* release old result, if any */
	if (this->buffer != 0) {
		free(this->buffer);
		this->buffer = 0;
		this->size = 0;
	}

	/* first byte of the first chunk after the header */
	chunk = this->source.buffer + 33;

	/* scan through the chunks, 
	 * finding the size of all IDAT chunks, and also
	 * verify general well-formed-ness */
	while (chunk < this->source.buffer + this->source.size){
		unsigned long length;

		/* make sure chunk header is not larger than the total compressed */
		if ((unsigned long)(chunk - this->source.buffer + 12) > this->source.size) {
			SET_ERROR(EUpngError::UPNG_EMALFORMED);
			return;
		}

		/* get length; sanity check it */
		length = upng_chunk_length(chunk);
		if (length > INT_MAX) {
			SET_ERROR(EUpngError::UPNG_EMALFORMED);
			return;
		}

		/* make sure chunk header+paylaod is not larger than the total compressed */
		if ((unsigned long)(chunk - this->source.buffer + length + 12) > this->source.size) {
			SET_ERROR(EUpngError::UPNG_EMALFORMED);
			return;
		}

		/* parse chunks */
		if (upng_chunk_type(chunk) == CHUNK_IDAT) {
			compressed_size += length;
		} else if (upng_chunk_type(chunk) == CHUNK_PLTE) {
			palette_size = length;
		} else if (upng_chunk_type(chunk) == CHUNK_IEND) {
			break;
		} else if (upng_chunk_critical(chunk)) {
			SET_ERROR(EUpngError::UPNG_EUNSUPPORTED);
			return;
		}

		chunk += length + 12;
	}

	/* allocate enough space for the (compressed and filtered) image data */
	compressed = (unsigned char*)malloc(compressed_size);
	if (compressed == NULL) {
		SET_ERROR(EUpngError::UPNG_ENOMEM);
		return;
	}

	if (palette_size) {
		palette = (unsigned char*)malloc(palette_size);
		if (palette == NULL) {
			free(compressed);
			SET_ERROR(EUpngError::UPNG_ENOMEM);
			return;
		}
	}

	/* scan through the chunks again, this time copying the values into
	 * our compressed buffer.  there's no reason to validate anything a second time. */
	chunk = this->source.buffer + 33;
	while (chunk < this->source.buffer + this->source.size) {
		unsigned long length;
		const unsigned char *data;	/*the data in the chunk */

		length = upng_chunk_length(chunk);

		/* parse chunks */
		if (upng_chunk_type(chunk) == CHUNK_IDAT) {
			data = chunk + 8;
			memcpy(compressed + compressed_index, data, length);
			compressed_index += length;
		} else if (upng_chunk_type(chunk) == CHUNK_PLTE) {
			data = chunk + 8;
			memcpy(palette, data, palette_size);
		} else if (upng_chunk_type(chunk) == CHUNK_IEND) {
			break;
		}

		chunk += length + 12;
	}

	/* allocate space to store inflated (but still filtered) data */
	/* Restrict height and width above to prevent an overflow here */
	inflated_size = ((this->width * (this->height * upng_get_bpp() + 7)) / 8) + this->height;
	inflated = (unsigned char*)malloc(inflated_size);
	if (inflated == NULL) {
		free(palette);
		free(compressed);
		SET_ERROR(EUpngError::UPNG_ENOMEM);
		return;
	}

	/* decompress image data */
	uz_inflate(
		inflated, 
		inflated_size, 
		compressed, 
		compressed_size
	);
	if (this->error != EUpngError::UPNG_EOK) {
		free(palette);
		free(compressed);
		free(inflated);
	}

	/* free the compressed compressed data */
	free(compressed);

	/* allocate final image buffer */
	this->size = (
		this->height * this->width * upng_get_bpp() + 7
	) / 8;
	this->buffer = (unsigned char*)malloc(this->size);
	if (this->buffer == NULL) {
		free(palette);
		free(inflated);
		this->size = 0;
		SET_ERROR(EUpngError::UPNG_ENOMEM);
		return;
	}

	/* unfilter scanlines */
	post_process_scanlines(this->buffer, inflated);
	free(inflated);

	if (this->error != EUpngError::UPNG_EOK) {
		free(palette);
		free(this->buffer);
		this->buffer = NULL;
		this->size = 0;
	} else {
		/* palette */
		this->palette = palette;
		this->state = EUpngState::UPNG_DECODED;
	}

	/* we are done with our input buffer; free it if we own it */
	upng_free_source();

	return;
}

unsigned CUpng::upng_get_bpp()
{
	return this->color_depth * upng_get_components();
}

void CUpng::SET_ERROR(EUpngError code) {
	this->error = code;
	this->error_line = __LINE__;
}

void CUpng::upng_header() {
	/* if we have an error state, bail now */
	if (this->error != EUpngError::UPNG_EOK) {
		return;
	}

	/* if the state is not NEW (meaning we are ready to parse the header), stop now */
	if (this->state != EUpngState::UPNG_NEW) {
		return;
	}

	/* minimum length of a valid PNG file is 29 bytes
	 * FIXME: verify this against the specification, or
	 * better against the actual code below */
	if (this->source.size < 29) {
		SET_ERROR(EUpngError::UPNG_ENOTPNG);
		return;
	}

	/* check that PNG header matches expected value */
	if (this->source.buffer[0] != 137 || 
		this->source.buffer[1] != 80 || 
		this->source.buffer[2] != 78 || 
		this->source.buffer[3] != 71 || 
		this->source.buffer[4] != 13 || 
		this->source.buffer[5] != 10 || 
		this->source.buffer[6] != 26 || 
		this->source.buffer[7] != 10) {
			SET_ERROR(EUpngError::UPNG_ENOTPNG);
			return;
	}

	/* check that the first chunk is the IHDR chunk */
	if (MAKE_DWORD_PTR(this->source.buffer + 12) != CHUNK_IHDR) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* read the values given in the header */
	this->width = MAKE_DWORD_PTR(this->source.buffer + 16);
	this->height = MAKE_DWORD_PTR(this->source.buffer + 20);
	if (this->width > 10000 || this->height > 10000) {
		/* should be upng->width > INT_MAX || upng->height > INT_MAX according to the spec
		 * this arbitrary but reasonable limit makes overflow checks in other parts of the code
		 * redundant */
		SET_ERROR(EUpngError::UPNG_EUNFORMAT);
		return;
    }

	this->color_depth = this->source.buffer[24];
	this->color_type = (EUpngColor)this->source.buffer[25];

	/* determine our color format */
	this->format = determine_format();
	if (this->format == EUpngFormat::UPNG_BADFORMAT) {
		SET_ERROR(EUpngError::UPNG_EUNFORMAT);
		return;
	}

	/* check that the compression method (byte 27) is 0 (only allowed value in spec) */
	if (this->source.buffer[26] != 0) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* check that the compression method (byte 27) is 0 (only allowed value in spec) */
	if (this->source.buffer[27] != 0) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* check that the compression method (byte 27) is 0 (spec allows 1, but uPNG does not support it) */
	if (this->source.buffer[28] != 0) {
		SET_ERROR(EUpngError::UPNG_EUNINTERLACED);
		return;
	}

	this->state = EUpngState::UPNG_HEADER;
	return;
}

EUpngFormat CUpng::determine_format() {
	switch (this->color_type) {
	case EUpngColor::UPNG_LUM:
		switch (this->color_depth) {
		case 1:
			return EUpngFormat::UPNG_LUMINANCE1;
		case 2:
			return EUpngFormat::UPNG_LUMINANCE2;
		case 4:
			return EUpngFormat::UPNG_LUMINANCE4;
		case 8:
			return EUpngFormat::UPNG_LUMINANCE8;
		default:
			return EUpngFormat::UPNG_BADFORMAT;
		}
	case EUpngColor::UPNG_RGB:
		switch (this->color_depth) {
		case 8:
			return EUpngFormat::UPNG_RGB8;
		case 16:
			return EUpngFormat::UPNG_RGB16;
		default:
			return EUpngFormat::UPNG_BADFORMAT;
		}
	case EUpngColor::UPNG_LUMA:
		switch (this->color_depth) {
		case 1:
			return EUpngFormat::UPNG_LUMINANCE_ALPHA1;
		case 2:
			return EUpngFormat::UPNG_LUMINANCE_ALPHA2;
		case 4:
			return EUpngFormat::UPNG_LUMINANCE_ALPHA4;
		case 8:
			return EUpngFormat::UPNG_LUMINANCE_ALPHA8;
		default:
			return EUpngFormat::UPNG_BADFORMAT;
		}
	case EUpngColor::UPNG_RGBA:
		switch (this->color_depth) {
		case 8:
			return EUpngFormat::UPNG_RGBA8;
		case 16:
			return EUpngFormat::UPNG_RGBA16;
		default:
			return EUpngFormat::UPNG_BADFORMAT;
		}
	case EUpngColor::UPNG_INDX:
		switch (this->color_depth) {
		case 1:
			return EUpngFormat::UPNG_INDEX1;
		case 2:
			return EUpngFormat::UPNG_INDEX2;
		case 4:
			return EUpngFormat::UPNG_INDEX4;
		case 8:
			return EUpngFormat::UPNG_INDEX8;
		default:
			return EUpngFormat::UPNG_BADFORMAT;
		}
	default:
		return EUpngFormat::UPNG_BADFORMAT;
	}
}

unsigned CUpng::upng_get_components()
{
	switch (this->color_type) {
	case EUpngColor::UPNG_LUM:
		return 1;
	case EUpngColor::UPNG_RGB:
		return 3;
	case EUpngColor::UPNG_LUMA:
		return 2;
	case EUpngColor::UPNG_RGBA:
		return 4;
	case EUpngColor::UPNG_INDX:
		return 1;
	default:
		return 0;
	}
}

void CUpng::uz_inflate(
	unsigned char *out, 
	unsigned long outsize, 
	const unsigned char *in, 
	unsigned long insize
) {
	/* we require two bytes for the zlib data header */
	if (insize < 2) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* 256 * in[0] + in[1] must be a multiple of 31, 
	 * the FCHECK value is supposed to be made that way */
	if ((in[0] * 256 + in[1]) % 31 != 0) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* error:only compression method 8: inflate with sliding 
	 * window of 32k is supported by the PNG spec */
	if ((in[0] & 15) != 8 || ((in[0] >> 4) & 15) > 7) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* the specification of PNG says about the zlib stream: 
	 * "The additional flags shall not specify 
	 * a preset dictionary." */
	if (((in[1] >> 5) & 1) != 0) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* create output buffer */
	uz_inflate_data(out, outsize, in, insize, 2);

	return;
}

/* inflate the deflated data (cfr. deflate spec); 
 * return value is the error*/
void CUpng::uz_inflate_data(
	unsigned char* out, 
	unsigned long outsize, 
	const unsigned char *in, 
	unsigned long insize, 
	unsigned long inpos
) {
	/* bit pointer in the "in" data, 
	 * current byte is bp >> 3, current bit is bp & 0x7 
	 * (from lsb to msb of the byte) */
	unsigned long bp = 0;	
	/*byte position in the out buffer */
	unsigned long pos = 0;	

	unsigned done = 0;

	while (done == 0) {
		unsigned btype;

		/* ensure next bit doesn't point past 
		 * the end of the buffer */
		if ((bp >> 3) >= insize) {
			SET_ERROR(EUpngError::UPNG_EMALFORMED);
			return;
		}

		/* read block control bits */
		done = read_bit(&bp, &in[inpos]);
		btype = read_bit(&bp, &in[inpos]) | 
				(read_bit(&bp, &in[inpos]) << 1);

		/* process control type appropriateyly */
		if (btype == 3) {
			SET_ERROR(EUpngError::UPNG_EMALFORMED);
			return;
		} else if (btype == 0) {
			inflate_uncompressed(
				out, 
				outsize, 
				&in[inpos], 
				&bp, 
				&pos, 
				insize
			);	/*no compression */
		} else {
			inflate_huffman(
				out, 
				outsize, 
				&in[inpos], 
				&bp, 
				&pos, 
				insize, 
				btype
			);	/*compression, btype 01 or 10 */
		}

		/* stop if an error has occured */
		if (this->error != EUpngError::UPNG_EOK) {
			return;
		}
	}

	return;
}

void CUpng::inflate_uncompressed(
	unsigned char* out, 
	unsigned long outsize, 
	const unsigned char *in, 
	unsigned long *bp, 
	unsigned long *pos, 
	unsigned long inlength
) {
	unsigned long p;
	unsigned len, nlen, n;

	/* go to first boundary of byte */
	while (((*bp) & 0x7) != 0) {
		(*bp)++;
	}
	p = (*bp) / 8;		/*byte position */

	/* read len (2 bytes) and nlen (2 bytes) */
	if (p >= inlength - 4) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	len = in[p] + 256 * in[p + 1];
	p += 2;
	nlen = in[p] + 256 * in[p + 1];
	p += 2;

	/* check if 16-bit nlen is really 
	 * the one's complement of len */
	if (len + nlen != 65535) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	if ((*pos) + len >= outsize) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* read the literal data: len bytes are now stored 
	 * in the out buffer */
	if (p + len > inlength) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	for (n = 0; n < len; n++) {
		out[(*pos)++] = in[p++];
	}

	(*bp) = p * 8;
}

/*inflate a block with dynamic of fixed Huffman tree*/
void CUpng::inflate_huffman(
	unsigned char* out, 
	unsigned long outsize, 
	const unsigned char *in, 
	unsigned long *bp, 
	unsigned long *pos, 
	unsigned long inlength, 
	unsigned btype
) {
	unsigned codetree_buffer[DEFLATE_CODE_BUFFER_SIZE];
	unsigned codetreeD_buffer[DISTANCE_BUFFER_SIZE];
	unsigned done = 0;

	CHuffmanTree codetree;
	CHuffmanTree codetreeD;

	if (btype == 1) {
		/* fixed trees */
		codetree
		.setTree2d((unsigned*)FIXED_DEFLATE_CODE_TREE)
		.setNumCodes(NUM_DEFLATE_CODE_SYMBOLS)
		.setMaxBitLen(DEFLATE_CODE_BITLEN);

		codetreeD
		.setTree2d((unsigned*)FIXED_DISTANCE_TREE)
		.setNumCodes(NUM_DISTANCE_SYMBOLS)
		.setMaxBitLen(DISTANCE_BITLEN);
	} else if (btype == 2) {
		/* dynamic trees */
		unsigned codelengthcodetree_buffer[
			CODE_LENGTH_BUFFER_SIZE
		];
		CHuffmanTree codelengthcodetree;

		codetree.setTree2d(codetree_buffer)
		.setNumCodes(NUM_DEFLATE_CODE_SYMBOLS)
		.setMaxBitLen(DEFLATE_CODE_BITLEN);

		codetreeD.setTree2d(codetreeD_buffer)
		.setNumCodes(NUM_DISTANCE_SYMBOLS)
		.setMaxBitLen(DISTANCE_BITLEN);

		codelengthcodetree
		.setTree2d(codelengthcodetree_buffer)
		.setNumCodes(NUM_CODE_LENGTH_CODES)
		.setMaxBitLen(CODE_LENGTH_BITLEN);

		get_tree_inflate_dynamic(
			codetree, 
			codetreeD, 
			codelengthcodetree, 
			in, 
			bp, 
			inlength
		);
	}

	while (done == 0) {
		unsigned code = huffman_decode_symbol(
			in, bp, codetree, inlength
		);
		if (this->error != EUpngError::UPNG_EOK) {
			return;
		}

		if (code == 256) {
			/* end code */
			done = 1;
		} else if (code <= 255) {
			/* literal symbol */
			if ((*pos) >= outsize) {
				SET_ERROR(EUpngError::UPNG_EMALFORMED);
				return;
			}

			/* store output */
			out[(*pos)++] = (unsigned char)(code);
		} else if (
			code >= FIRST_LENGTH_CODE_INDEX 
			&& code <= LAST_LENGTH_CODE_INDEX
		) {	/*length code */
			/* part 1: get length base */
			unsigned long length = 
				LENGTH_BASE[code - FIRST_LENGTH_CODE_INDEX];
			unsigned codeD, distance, numextrabitsD;
			unsigned long start, forward, backward, 
				numextrabits;

			/* part 2: get extra bits and add 
			 * the value of that to length */
			numextrabits = LENGTH_EXTRA[
				code - FIRST_LENGTH_CODE_INDEX
			];

			/* error, bit pointer will jump past memory */
			if (((*bp) >> 3) >= inlength) {
				SET_ERROR(EUpngError::UPNG_EMALFORMED);
				return;
			}
			length += read_bits(bp, in, numextrabits);

			/*part 3: get distance code */
			codeD = huffman_decode_symbol(
				in, bp, codetreeD, inlength
			);
			if (this->error != EUpngError::UPNG_EOK) {
				return;
			}

			/* invalid distance code 
			 * (30-31 are never used) */
			if (codeD > 29) {
				SET_ERROR(EUpngError::UPNG_EMALFORMED);
				return;
			}

			distance = DISTANCE_BASE[codeD];

			/*part 4: get extra bits from distance */
			numextrabitsD = DISTANCE_EXTRA[codeD];

			/* error, bit pointer will jump past memory */
			if (((*bp) >> 3) >= inlength) {
				SET_ERROR(EUpngError::UPNG_EMALFORMED);
				return;
			}

			distance += read_bits(bp, in, numextrabitsD);

			/* part 5: fill in all the out[n] values based 
			 * on the length and dist */
			start = (*pos);
			backward = start - distance;

			if ((*pos) + length >= outsize) {
				SET_ERROR(EUpngError::UPNG_EMALFORMED);
				return;
			}

			for (forward = 0; forward < length; forward++) {
				out[(*pos)++] = out[backward];
				backward++;

				if (backward >= start) {
					backward = start - distance;
				}
			}
		}
	}
}

/* get the tree of a deflated block with dynamic tree, 
 * the tree itself is also 
 * Huffman compressed with a known tree*/
void CUpng::get_tree_inflate_dynamic(
	CHuffmanTree &codetree, 
	CHuffmanTree &codetreeD, 
	CHuffmanTree &codelengthcodetree, 
	const unsigned char *in, 
	unsigned long *bp, 
	unsigned long inlength
) {
	unsigned codelengthcode[NUM_CODE_LENGTH_CODES];
	unsigned bitlen[NUM_DEFLATE_CODE_SYMBOLS];
	unsigned bitlenD[NUM_DISTANCE_SYMBOLS];
	unsigned n, hlit, hdist, hclen, i;

	/* make sure that length values that 
	 * aren't filled in will be 0, 
	 * or a wrong tree will be generated */
	/* C-code note: use no "return" between ctor 
	 * and dtor of an uivector! */
	if ((*bp) >> 3 >= inlength - 2) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	/* clear bitlen arrays */
	memset(bitlen, 0, sizeof(bitlen));
	memset(bitlenD, 0, sizeof(bitlenD));

	/*the bit pointer is or will go past the memory */
	
	/* number of literal/length codes + 257. 
	 * Unlike the spec, the value 257 is added 
	 * to it here already */
	hlit = read_bits(bp, in, 5) + 257;	
	/* number of distance codes. 
	 * Unlike the spec, the value 1 is added to it 
	 * here already */
	hdist = read_bits(bp, in, 5) + 1;	
	/* number of code length codes. 
	 * Unlike the spec, the value 4 is added to it 
	 * here already */
	hclen = read_bits(bp, in, 4) + 4;	

	for (i = 0; i < NUM_CODE_LENGTH_CODES; i++) {
		if (i < hclen) {
			codelengthcode[CLCL[i]] = read_bits(bp, in, 3);
		} else {
			/*if not, it must stay 0 */
			codelengthcode[CLCL[i]] = 0;	
		}
	}

	if (codelengthcodetree.huffman_tree_create_lengths(
		codelengthcode
	) == false) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	};

	/* now we can use this tree to read the lengths 
	 * for the tree that this function will return */
	i = 0;
	/* i is the current symbol we're reading in the part 
	 * that contains the code lengths of lit/len codes 
	 * and dist codes */
	while (i < hlit + hdist) {	
		unsigned code = huffman_decode_symbol(
			in, bp, codelengthcodetree, inlength
		);
		if (this->error != EUpngError::UPNG_EOK) {
			break;
		}

		if (code <= 15) {	/*a length code */
			if (i < hlit) {
				bitlen[i] = code;
			} else {
				bitlenD[i - hlit] = code;
			}
			i++;
		} else if (code == 16) {	/*repeat previous */
			/* read in the 2 bits that indicate repeat 
			 * length (3-6) */
			unsigned replength = 3;	
			/*set value to the previous code */
			unsigned value;	

			if ((*bp) >> 3 >= inlength) {
				SET_ERROR(EUpngError::UPNG_EMALFORMED);
				break;
			}
			/*error, bit pointer jumps past memory */
			replength += read_bits(bp, in, 2);

			if ((i - 1) < hlit) {
				value = bitlen[i - 1];
			} else {
				value = bitlenD[i - hlit - 1];
			}

			/*repeat this value in the next lengths */
			for (n = 0; n < replength; n++) {
				/* i is larger than the amount of codes */
				if (i >= hlit + hdist) {
					SET_ERROR(EUpngError::UPNG_EMALFORMED);
					break;
				}

				if (i < hlit) {
					bitlen[i] = value;
				} else {
					bitlenD[i - hlit] = value;
				}
				i++;
			}
		} else if (code == 17) { /*repeat "0" 3-10 times */
			/*read in the bits that indicate repeat length*/
			unsigned replength = 3;	
			if ((*bp) >> 3 >= inlength) {
				SET_ERROR(EUpngError::UPNG_EMALFORMED);
				break;
			}

			/*error, bit pointer jumps past memory */
			replength += read_bits(bp, in, 3);

			/*repeat this value in the next lengths */
			for (n = 0; n < replength; n++) {
				/* error: i is larger than 
				 * the amount of codes */
				if (i >= hlit + hdist) {
					SET_ERROR(EUpngError::UPNG_EMALFORMED);
					break;
				}

				if (i < hlit) {
					bitlen[i] = 0;
				} else {
					bitlenD[i - hlit] = 0;
				}
				i++;
			}
		} else if (code == 18) {/*repeat "0" 11-138 times */
			/*read in the bits that indicate repeat length*/
			unsigned replength = 11;	
			/* error, bit pointer jumps past memory */
			if ((*bp) >> 3 >= inlength) {
				SET_ERROR(EUpngError::UPNG_EMALFORMED);
				break;
			}

			replength += read_bits(bp, in, 7);

			/*repeat this value in the next lengths */
			for (n = 0; n < replength; n++) {
				/* i is larger than the amount of codes */
				if (i >= hlit + hdist) {
					SET_ERROR(EUpngError::UPNG_EMALFORMED);
					break;
				}
				if (i < hlit)
					bitlen[i] = 0;
				else
					bitlenD[i - hlit] = 0;
				i++;
			}
		} else {
			/* somehow an unexisting code appeared. 
			 * This can never happen. */
			SET_ERROR(EUpngError::UPNG_EMALFORMED);
			break;
		}
	}

	if (this->error == EUpngError::UPNG_EOK && bitlen[256] == 0) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
	}

	/*the length of the end code 256 must be larger than 0*/
	/*now we've finally got hlit and hdist, 
	 *so generate the code trees, and the function is done*/
	if (this->error == EUpngError::UPNG_EOK) {
		codetree.huffman_tree_create_lengths(bitlen);
	}
	if (this->error == EUpngError::UPNG_EOK) {
		codetreeD.huffman_tree_create_lengths(bitlenD);
	}
}

unsigned char read_bit(
	unsigned long *bitpointer, 
	const unsigned char *bitstream
) {
	unsigned char result = (unsigned char)(
		(bitstream[(*bitpointer) >> 3] 
			>> ((*bitpointer) & 0x7)) & 1
	);
	(*bitpointer)++;
	return result;
}

unsigned CUpng::read_bits(
	unsigned long *bitpointer, 
	const unsigned char *bitstream, 
	unsigned long nbits
) {
	unsigned result = 0, i;
	for (i = 0; i < nbits; i++)
		result |= (
			(unsigned)read_bit(bitpointer, bitstream)
		) << i;
	return result;
}

unsigned CUpng::huffman_decode_symbol(
	const unsigned char *in, 
	unsigned long *bp, 
	const CHuffmanTree &codetree, 
	unsigned long inlength
) {
	unsigned treepos = 0, ct;
	unsigned char bit;
	for (;;) {
		/* error: end of input memory reached 
		 * without endcode */
		if (((*bp) & 0x07) == 0 && ((*bp) >> 3) > inlength){
			SET_ERROR(EUpngError::UPNG_EMALFORMED);
			return 0;
		}

		bit = read_bit(bp, in);

		ct = codetree.tree2d[(treepos << 1) | bit];
		if (ct < codetree.numcodes) {
			return ct;
		}

		treepos = ct - codetree.numcodes;
		if (treepos >= codetree.numcodes) {
			SET_ERROR(EUpngError::UPNG_EMALFORMED);
			return 0;
		}
	}
}

/*out must be buffer big enough to contain full image, and in must contain the full decompressed data from the IDAT chunks*/
void CUpng::post_process_scanlines(unsigned char *out, unsigned char *in)
{
	unsigned bpp = upng_get_bpp();
	unsigned w = this->width;
	unsigned h = this->height;

	if (bpp == 0) {
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		return;
	}

	if (bpp < 8 && w * bpp != ((w * bpp + 7) / 8) * 8) {
		unfilter(in, in, w, h, bpp);
		if (this->error != EUpngError::UPNG_EOK) {
			return;
		}
		remove_padding_bits(
			out, in, w * bpp, ((w * bpp + 7) / 8) * 8, h
		);
	} else {
		/* we can immediatly filter into the out buffer, 
		 * no other steps needed */
		unfilter(out, in, w, h, bpp);	
	}
}

void CUpng::unfilter(
	unsigned char *out, 
	const unsigned char *in, 
	unsigned w, 
	unsigned h, 
	unsigned bpp
) {
	/*
	   For PNG filter method 0
	   this function unfilters a single image 
	   (e.g. without interlacing this is called once, 
	   with Adam7 it's called 7 times)
	   out must have enough bytes allocated already, 
	   in must have the scanlines + 1 
	   filtertype byte per scanline
	   w and h are image dimensions 
	   or dimensions of reduced image, bpp is bpp per pixel
	   in and out are allowed to be the same memory address!
	*/

	unsigned y;
	unsigned char *prevline = 0;

	/* bytewidth is used for filtering, is 1 when bpp < 8, 
	 * number of bytes per pixel otherwise */
	unsigned long bytewidth = (bpp + 7) / 8;	
	unsigned long linebytes = (w * bpp + 7) / 8;

	for (y = 0; y < h; y++) {
		unsigned long outindex = linebytes * y;
		/*the extra filterbyte added to each row */
		unsigned long inindex = (1 + linebytes) * y;	
		unsigned char filterType = in[inindex];

		unfilter_scanline(
			&out[outindex], 
			&in[inindex + 1], 
			prevline, 
			bytewidth, 
			filterType, 
			linebytes
		);
		if (this->error != EUpngError::UPNG_EOK) {
			return;
		}

		prevline = &out[outindex];
	}
}

void CUpng::unfilter_scanline(
	unsigned char *recon, 
	const unsigned char *scanline, 
	const unsigned char *precon, 
	unsigned long bytewidth, 
	unsigned char filterType, 
	unsigned long length
) {
	/*
	   For PNG filter method 0
	   unfilter a PNG image scanline by scanline. 
	   when the pixels are smaller than 1 byte, 
	   the filter works byte per byte (bytewidth = 1)
	   precon is the previous unfiltered scanline, 
	   recon the result, scanline the current one
	   the incoming scanlines do NOT include 
	   the filtertype byte, 
	   that one is given in the parameter filterType instead
	   recon and scanline MAY be the same memory address! 
	   precon must be disjoint.
	 */

	unsigned long i;
	switch (filterType) {
	case 0:
		for (i = 0; i < length; i++)
			recon[i] = scanline[i];
		break;
	case 1:
		for (i = 0; i < bytewidth; i++)
			recon[i] = scanline[i];
		for (i = bytewidth; i < length; i++)
			recon[i] = scanline[i] + recon[i - bytewidth];
		break;
	case 2:
		if (precon)
			for (i = 0; i < length; i++)
				recon[i] = scanline[i] + precon[i];
		else
			for (i = 0; i < length; i++)
				recon[i] = scanline[i];
		break;
	case 3:
		if (precon) {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i] + precon[i] / 2;
			for (i = bytewidth; i < length; i++)
				recon[i] = scanline[i] 
					+ ((recon[i - bytewidth] 
					+ precon[i]) / 2);
		} else {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i];
			for (i = bytewidth; i < length; i++)
				recon[i] = scanline[i] 
					+ recon[i - bytewidth] / 2;
		}
		break;
	case 4:
		if (precon) {
			for (i = 0; i < bytewidth; i++)
				recon[i] = (unsigned char)(scanline[i] 
					+ paeth_predictor(0, precon[i], 0));
			for (i = bytewidth; i < length; i++)
				recon[i] = (unsigned char)(
					scanline[i] + paeth_predictor(
						recon[i - bytewidth], 
						precon[i], 
						precon[i - bytewidth]
					)
				);
		} else {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i];
			for (i = bytewidth; i < length; i++)
				recon[i] = (unsigned char)(
					scanline[i] + paeth_predictor(
						recon[i - bytewidth], 0, 0
					)
				);
		}
		break;
	default:
		SET_ERROR(EUpngError::UPNG_EMALFORMED);
		break;
	}
}

/*Paeth predicter, used by PNG filter type 4*/
int CUpng::paeth_predictor(int a, int b, int c)
{
	int p = a + b - c;
	int pa = p > a ? p - a : a - p;
	int pb = p > b ? p - b : b - p;
	int pc = p > c ? p - c : c - p;

	if (pa <= pb && pa <= pc)
		return a;
	else if (pb <= pc)
		return b;
	else
		return c;
}

void CUpng::remove_padding_bits(
	unsigned char *out, 
	const unsigned char *in, 
	unsigned long olinebits, 
	unsigned long ilinebits, 
	unsigned h
) {
	/*
	   After filtering there are still padding bpp 
	   if scanlines have non multiple of 8 bit amounts. 
	   They need to be removed 
	   (except at last scanline of (Adam7-reduced) image) 
	   before working with pure image buffers 
	   for the Adam7 code, 
	   the color convert code and the output to the user.
	   in and out are allowed to be the same buffer, 
	   in may also be higher but still overlapping; 
	   in must have >= ilinebits*h bpp, 
	   out must have >= olinebits*h bpp, 
	   olinebits must be <= ilinebits
	   also used to move bpp after earlier 
	   such operations happened, 
	   e.g. in a sequence of reduced images from Adam7
	   only useful if (ilinebits - olinebits) is a value 
	   in the range 1..7
	*/
	unsigned y;
	unsigned long diff = ilinebits - olinebits;
	unsigned long obp = 0, ibp = 0;	/*bit pointers */
	for (y = 0; y < h; y++) {
		unsigned long x;
		for (x = 0; x < olinebits; x++) {
			unsigned char bit = (unsigned char)(
				(in[(ibp) >> 3] >> (7 - ((ibp) & 0x7))) & 1
			);
			ibp++;

			if (bit == 0)
				out[(obp) >> 3] &= (unsigned char)(
					~(1 << (7 - ((obp) & 0x7)))
				);
			else
				out[(obp) >> 3] |= (
					1 << (7 - ((obp) & 0x7))
				);
			++obp;
		}
		ibp += diff;
	}
}

void CUpng::upng_free_source()
{
	if (this->source.owning != 0) {
		free((void*)this->source.buffer);
	}

	this->source.buffer = NULL;
	this->source.size = 0;
	this->source.owning = 0;
}