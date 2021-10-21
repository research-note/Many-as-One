#include "Upng.hpp"

Upng::Upng() {
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

void Upng::readFile(string filename) {
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
	size_t ret = fread(buffer, 1, (unsigned long)size, file);
    if (ret != size) {
        fprintf(stderr, "fread() failed: %zu\n", ret);
        exit(EXIT_FAILURE);
    }
	fclose(file);

	/* set the read buffer as our source buffer, with owning flag set */
	this->source.buffer = buffer;
	this->source.size = size;
	this->source.owning = 1;
}

EUpngError Upng::upng_get_error() {
	return error;
}

unsigned int Upng::upng_get_error_line() {
	return error_line;
}

void Upng::upng_decode() {
	const unsigned char *chunk;
	unsigned char* compressed;
	unsigned char* inflated;
	unsigned char* palette = NULL;
	unsigned long compressed_size = 0, compressed_index = 0;
	unsigned long palette_size = 0;
	unsigned long inflated_size;
	upng_error error;

	/* if we have an error state, bail now */
	if (this->error != EUpngError::UPNG_EOK) {
		return;
	}

	/* parse the main header, if necessary */
	upng.upng_header();
	if (this->error != EUpngError::UPNG_EOK) {
		return;
	}

	/* if the state is not HEADER (meaning we are ready to decode the image), stop now */
	if (upng->state != UPNG_HEADER) {
		return;
	}

	/* release old result, if any */
	if (upng->buffer != 0) {
		free(upng->buffer);
		upng->buffer = 0;
		upng->size = 0;
	}

	/* first byte of the first chunk after the header */
	chunk = upng->source.buffer + 33;

	/* scan through the chunks, finding the size of all IDAT chunks, and also
	 * verify general well-formed-ness */
	while (chunk < upng->source.buffer + upng->source.size) {
		unsigned long length;
		// const unsigned char *data;	/*the data in the chunk */

		/* make sure chunk header is not larger than the total compressed */
		if ((unsigned long)(chunk - upng->source.buffer + 12) > upng->source.size) {
			SET_ERROR(upng, UPNG_EMALFORMED);
			return upng->error;
		}

		/* get length; sanity check it */
		length = upng_chunk_length(chunk);
		if (length > INT_MAX) {
			SET_ERROR(upng, UPNG_EMALFORMED);
			return upng->error;
		}

		/* make sure chunk header+paylaod is not larger than the total compressed */
		if ((unsigned long)(chunk - upng->source.buffer + length + 12) > upng->source.size) {
			SET_ERROR(upng, UPNG_EMALFORMED);
			return upng->error;
		}

		/* get pointer to payload */
		// data = chunk + 8;

		/* parse chunks */
		if (upng_chunk_type(chunk) == CHUNK_IDAT) {
			compressed_size += length;
		} else if (upng_chunk_type(chunk) == CHUNK_PLTE) {
			palette_size = length;
		} else if (upng_chunk_type(chunk) == CHUNK_IEND) {
			break;
		} else if (upng_chunk_critical(chunk)) {
			SET_ERROR(upng, UPNG_EUNSUPPORTED);
			return upng->error;
		}

		chunk += length + 12;
	}

	/* allocate enough space for the (compressed and filtered) image data */
	compressed = (unsigned char*)malloc(compressed_size);
	if (compressed == NULL) {
		SET_ERROR(upng, UPNG_ENOMEM);
		return upng->error;
	}

	if (palette_size) {
		palette = (unsigned char*)malloc(palette_size);
		if (palette == NULL) {
			free(compressed);
			SET_ERROR(upng, UPNG_ENOMEM);
			return upng->error;
		}
	}

	/* scan through the chunks again, this time copying the values into
	 * our compressed buffer.  there's no reason to validate anything a second time. */
	chunk = upng->source.buffer + 33;
	while (chunk < upng->source.buffer + upng->source.size) {
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
	inflated_size = ((upng->width * (upng->height * upng_get_bpp(upng) + 7)) / 8) + upng->height;
	inflated = (unsigned char*)malloc(inflated_size);
	if (inflated == NULL) {
		free(palette);
		free(compressed);
		SET_ERROR(upng, UPNG_ENOMEM);
		return upng->error;
	}

	/* decompress image data */
	error = uz_inflate(upng, inflated, inflated_size, compressed, compressed_size);
	if (error != UPNG_EOK) {
		free(palette);
		free(compressed);
		free(inflated);
		return upng->error;
	}

	/* free the compressed compressed data */
	free(compressed);

	/* allocate final image buffer */
	upng->size = (upng->height * upng->width * upng_get_bpp(upng) + 7) / 8;
	upng->buffer = (unsigned char*)malloc(upng->size);
	if (upng->buffer == NULL) {
		free(palette);
		free(inflated);
		upng->size = 0;
		SET_ERROR(upng, UPNG_ENOMEM);
		return upng->error;
	}

	/* unfilter scanlines */
	post_process_scanlines(upng, upng->buffer, inflated, upng);
	free(inflated);

	if (upng->error != UPNG_EOK) {
		free(palette);
		free(upng->buffer);
		upng->buffer = NULL;
		upng->size = 0;
	} else {
		/* palette */
		upng->palette = palette;
		upng->state = UPNG_DECODED;
	}

	/* we are done with our input buffer; free it if we own it */
	upng_free_source(upng);

	return upng->error;
}

void Upng::SET_ERROR(EUpngError code) {
	this->error = code;
	this->error_line = __LINE__;
}

void Upng::upng_header() {
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
	if (MAKE_DWORD_PTR(upng->source.buffer + 12) != CHUNK_IHDR) {
		SET_ERROR(upng, UPNG_EMALFORMED);
		return upng->error;
	}

	/* read the values given in the header */
	upng->width = MAKE_DWORD_PTR(upng->source.buffer + 16);
	upng->height = MAKE_DWORD_PTR(upng->source.buffer + 20);
	if (upng->width > 10000 || upng->height > 10000) {
		/* should be upng->width > INT_MAX || upng->height > INT_MAX according to the spec
		 * this arbitrary but reasonable limit makes overflow checks in other parts of the code
		 * redundant */
		SET_ERROR(upng, UPNG_EUNFORMAT);
		return upng->error;
    }

	upng->color_depth = upng->source.buffer[24];
	upng->color_type = (upng_color)upng->source.buffer[25];

	/* determine our color format */
	upng->format = determine_format(upng);
	if (upng->format == UPNG_BADFORMAT) {
		SET_ERROR(upng, UPNG_EUNFORMAT);
		return upng->error;
	}

	/* check that the compression method (byte 27) is 0 (only allowed value in spec) */
	if (upng->source.buffer[26] != 0) {
		SET_ERROR(upng, UPNG_EMALFORMED);
		return upng->error;
	}

	/* check that the compression method (byte 27) is 0 (only allowed value in spec) */
	if (upng->source.buffer[27] != 0) {
		SET_ERROR(upng, UPNG_EMALFORMED);
		return upng->error;
	}

	/* check that the compression method (byte 27) is 0 (spec allows 1, but uPNG does not support it) */
	if (upng->source.buffer[28] != 0) {
		SET_ERROR(upng, UPNG_EUNINTERLACED);
		return upng->error;
	}

	upng->state = UPNG_HEADER;
	return upng->error;
}