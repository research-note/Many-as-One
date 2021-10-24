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
	upng_header();
	if (this->error != EUpngError::UPNG_EOK) {
		return;
	}

	/* if the state is not HEADER (meaning we are ready to decode the image), stop now */
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

	/* scan through the chunks, finding the size of all IDAT chunks, and also
	 * verify general well-formed-ness */
	while (chunk < this->source.buffer + this->source.size) {
		unsigned long length;
		// const unsigned char *data;	/*the data in the chunk */

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
	inflated_size = ((this->width * (this->height * upng_get_bpp(upng) + 7)) / 8) + this->height;
	inflated = (unsigned char*)malloc(inflated_size);
	if (inflated == NULL) {
		free(palette);
		free(compressed);
		SET_ERROR(EUpngError::UPNG_ENOMEM);
		return;
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

EUpngFormat Upng::determine_format() {
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

unsigned upng_get_bpp()
{
	return this->color_depth * upng_get_components(upng);
}