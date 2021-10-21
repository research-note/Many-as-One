#ifndef _UPNG_HPP_
#define _UPNG_HPP_

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
	UPNG_EOK			= 0, /* success (no error) */
	UPNG_ENOMEM			= 1, /* memory allocation failed */
	UPNG_ENOTFOUND		= 2, /* resource not found (file missing) */
	UPNG_ENOTPNG		= 3, /* image data does not have a PNG header */
	UPNG_EMALFORMED		= 4, /* image data is not a valid PNG image */
	UPNG_EUNSUPPORTED	= 5, /* critical PNG chunk type is not supported */
	UPNG_EUNINTERLACED	= 6, /* image interlacing is not supported */
	UPNG_EUNFORMAT		= 7, /* image color format is not supported */
	UPNG_EPARAM			= 8  /* invalid parameter to method call */
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

enum class EUpngState {
	UPNG_ERROR		= -1,
	UPNG_DECODED	= 0,
	UPNG_HEADER		= 1,
	UPNG_NEW		= 2
};

typedef struct upng_source {
	const unsigned char*	buffer;
	unsigned long			size;
	char					owning;
} upng_source;

class Upng {
    public:
        unsigned int width;
        unsigned int height;

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

        Upng();
        ~Upng();
		void readFile(string filename);
		EUpngError upng_get_error();
		unsigned int upng_get_error_line();
	private:
		void SET_ERROR(EUpngError code);
};

#include "Upng.cc"

#endif // _UPNG_HPP_