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

#define MAKE_BYTE(b) ((b) & 0xFF)
#define MAKE_DWORD(a,b,c,d) ((MAKE_BYTE(a) << 24) | (MAKE_BYTE(b) << 16) | (MAKE_BYTE(c) << 8) | MAKE_BYTE(d))
#define MAKE_DWORD_PTR(p) MAKE_DWORD((p)[0], (p)[1], (p)[2], (p)[3])

#define CHUNK_IHDR MAKE_DWORD('I','H','D','R')
#define CHUNK_IDAT MAKE_DWORD('I','D','A','T')
#define CHUNK_IEND MAKE_DWORD('I','E','N','D')
#define CHUNK_PLTE MAKE_DWORD('P','L','T','E')

#define upng_chunk_length(chunk) MAKE_DWORD_PTR(chunk)
#define upng_chunk_type(chunk) MAKE_DWORD_PTR((chunk) + 4)
#define upng_chunk_critical(chunk) (((chunk)[4] & 32) == 0)

class Upng {
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

        Upng();
        ~Upng();
		void 			readFile(string filename);
		EUpngError 		upng_get_error();
		unsigned int 	upng_get_error_line();
		void 			upng_decode();
	private:
		void SET_ERROR(EUpngError code);
		void upng_header();
		EUpngFormat determine_format();
		unsigned upng_get_bpp();
};

#include "Upng.cc"

#endif // _UPNG_HPP_