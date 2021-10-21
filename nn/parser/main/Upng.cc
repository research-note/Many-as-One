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
	source.buffer = buffer;
	source.size = size;
	source.owning = 1;
}

EUpngError Upng::upng_get_error() {
	return error;
}

unsigned int Upng::upng_get_error_line() {
	return error_line;
}

void Upng::SET_ERROR(EUpngError code) {
	error = code;
	error_line = __LINE__;
}