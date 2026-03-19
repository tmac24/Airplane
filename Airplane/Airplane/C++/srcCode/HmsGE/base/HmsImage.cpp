
#include "HmsAviMacros.h"
#include "base/HmsImage.h"

#include <string>
#include <ctype.h>

#include "base/HmsFileUtils.h"
#include "base/HmsData.h"

extern "C"
{
#include "png.h"
#include "jpeglib.h"
}

NS_HMS_BEGIN


//////////////////////////////////////////////////////////////////////////

Texture2D::PixelFormat getDevicePixelFormat(Texture2D::PixelFormat format)
{
	return format;//Texture2D::PixelFormat::RGBA8888;
}

//////////////////////////////////////////////////////////////////////////
// Implement Image
//////////////////////////////////////////////////////////////////////////

Image::Image()
: _data(nullptr)
, _dataLen(0)
, _width(0)
, _height(0)
, _unpack(false)
, _fileType(Format::UNKNOWN)
, _renderFormat(Texture2D::PixelFormat::NONE_USE)
, _numberOfMipmaps(0)
, _hasPremultipliedAlpha(true)
{

}

Image::~Image()
{
    if(_unpack)
    {
		HMS_SAFE_FREE(_mipmaps[0].address);

        for (int i = 1; i < _numberOfMipmaps; ++i)
        {
            HMS_SAFE_DELETE_ARRAY(_mipmaps[i].address);
        }
    }
    else
    {
        HMS_SAFE_FREE(_data);
    }
}

bool Image::initWithImageFile(const std::string& path)
{
    bool ret = false;
    _filePath = CHmsFileUtils::getInstance()->fullPathForFilename(path);

    Data data = CHmsFileUtils::getInstance()->getDataFromFile(_filePath);

    if (!data.isNull())
    {
        ret = initWithImageData(data.getBytes(), data.getSize());
    }

    return ret;
}

bool Image::initWithImageFileThreadSafe(const std::string& fullpath)
{
    bool ret = false;
    _filePath = fullpath;

    Data data = CHmsFileUtils::getInstance()->getDataFromFile(fullpath);

    if (!data.isNull())
    {
        ret = initWithImageData(data.getBytes(), data.getSize());
    }

    return ret;
}

bool Image::initWithImageData(const unsigned char * data, ssize_t dataLen)
{
    bool ret = false;
    
    do
    {
        HMS_BREAK_IF(! data || dataLen <= 0);
        
        unsigned char* unpackedData = nullptr;
        ssize_t unpackedLen = 0;
        
       unpackedData = const_cast<unsigned char*>(data);
       unpackedLen = dataLen;

        _fileType = detectFormat(unpackedData, unpackedLen);

        switch (_fileType)
        {
        case Format::PNG:
            ret = initWithPngData(unpackedData, unpackedLen);
            break;
        case Format::JPG:
            ret = initWithJpgData(unpackedData, unpackedLen);
            break;
        default:
            {
                // load and detect image format
                CCLOG("HmsAviPf: unsupported image format!");
                break;
            }
        }
        
    } while (0);
    
    return ret;
}

bool Image::isPng(const unsigned char * data, ssize_t dataLen)
{
    if (dataLen <= 8)
    {
        return false;
    }

    static const unsigned char PNG_SIGNATURE[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};

    return memcmp(PNG_SIGNATURE, data, sizeof(PNG_SIGNATURE)) == 0;
}

bool Image::isJpg(const unsigned char * data, ssize_t dataLen)
{
    if (dataLen <= 4)
    {
        return false;
    }

    static const unsigned char JPG_SOI[] = {0xFF, 0xD8};

    return memcmp(data, JPG_SOI, 2) == 0;
}


Image::Format Image::detectFormat(const unsigned char * data, ssize_t dataLen)
{
    if (isPng(data, dataLen))
    {
        return Format::PNG;
    }
    else if (isJpg(data, dataLen))
    {
        return Format::JPG;
    }
    else
    {
        CCLOG("HmsAviPf: can't detect image format");
        return Format::UNKNOWN;
    }
}

int Image::getBitPerPixel()
{
    return Texture2D::getPixelFormatInfoMap().at(_renderFormat).bpp;
}

bool Image::hasAlpha()
{
    return Texture2D::getPixelFormatInfoMap().at(_renderFormat).alpha;
}

bool Image::isCompressed()
{
    return Texture2D::getPixelFormatInfoMap().at(_renderFormat).compressed;
}

namespace
{
	/*
	* ERROR HANDLING:
	*
	* The JPEG library's standard error handler (jerror.c) is divided into
	* several "methods" which you can override individually.  This lets you
	* adjust the behavior without duplicating a lot of code, which you might
	* have to update with each future release.
	*
	* We override the "error_exit" method so that control is returned to the
	* library's caller when a fatal error occurs, rather than calling exit()
	* as the standard error_exit method does.
	*
	* We use C's setjmp/longjmp facility to return control.  This means that the
	* routine which calls the JPEG library must first execute a setjmp() call to
	* establish the return point.  We want the replacement error_exit to do a
	* longjmp().  But we need to make the setjmp buffer accessible to the
	* error_exit routine.  To do this, we make a private extension of the
	* standard JPEG error handler object.  (If we were using C++, we'd say we
	* were making a subclass of the regular error handler.)
	*
	* Here's the extended error handler struct:
	*/

	struct MyErrorMgr
	{
		struct jpeg_error_mgr pub;  /* "public" fields */
		jmp_buf setjmp_buffer;  /* for return to caller */
	};

	typedef struct MyErrorMgr * MyErrorPtr;

	/*
	* Here's the routine that will replace the standard error_exit method:
	*/

	METHODDEF(void)
		myErrorExit(j_common_ptr cinfo)
	{
		/* cinfo->err really points to a MyErrorMgr struct, so coerce pointer */
		MyErrorPtr myerr = (MyErrorPtr)cinfo->err;

		/* Always display the message. */
		/* We could postpone this until after returning, if we chose. */
		/* internal message function can't show error message in some platforms, so we rewrite it here.
		* edit it if has version conflict.
		*/
		//(*cinfo->err->output_message) (cinfo);
		char buffer[JMSG_LENGTH_MAX];
		(*cinfo->err->format_message) (cinfo, buffer);
		CCLOG("jpeg error: %s", buffer);

		/* Return control to the setjmp point */
		longjmp(myerr->setjmp_buffer, 1);
	}
}

#ifdef __vxworks
/* Read JPEG image from a memory segment */

#include "jerror.h"

#define ERREXIT(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))


static void init_source (j_decompress_ptr cinfo) {}
static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
    ERREXIT(cinfo, JERR_INPUT_EMPTY);
    return TRUE;
}
static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    struct jpeg_source_mgr* src = (struct jpeg_source_mgr*) cinfo->src;

    if (num_bytes > 0) {
        src->next_input_byte += (size_t) num_bytes;
        src->bytes_in_buffer -= (size_t) num_bytes;
    }
}
static void term_source (j_decompress_ptr cinfo) {}
static void jpeg_mem_src (j_decompress_ptr cinfo, void* buffer, long nbytes)
{
    struct jpeg_source_mgr* src;

    if (cinfo->src == NULL) {   /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
            sizeof(struct jpeg_source_mgr));
    }

    src = (struct jpeg_source_mgr*) cinfo->src;
    src->init_source = init_source;
    src->fill_input_buffer = fill_input_buffer;
    src->skip_input_data = skip_input_data;
    src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->term_source = term_source;
    src->bytes_in_buffer = nbytes;
    src->next_input_byte = (JOCTET*)buffer;
}

static inline unsigned int roundup_pow_of_two(unsigned int n)                         
{
    int i;

    if (n == 0)
        return n;

    for (i = 0; i < 32; i ++)
        {
        if (n == (1 << i))
            return n;
        if ((n > (1 << i)) && (n <= (1 << (i + 1))))
            return (1 << (i + 1));
        }
    return (1 << 31);
}

#endif
bool Image::initWithJpgData(const unsigned char * data, ssize_t dataLen)
{
	/* these are standard libjpeg structures for reading(decompression) */
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct MyErrorMgr jerr;
	/* libjpeg data structure for storing one row, that is, scanline of an image */
	JSAMPROW row_pointer[1] = { 0 };
	unsigned long location = 0;

	bool ret = false;
	do
	{
		/* We set up the normal JPEG error routines, then override error_exit. */
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = myErrorExit;
		/* Establish the setjmp return context for MyErrorExit to use. */
		if (setjmp(jerr.setjmp_buffer))
		{
			/* If we get here, the JPEG code has signaled an error.
			* We need to clean up the JPEG object, close the input file, and return.
			*/
			jpeg_destroy_decompress(&cinfo);
			break;
		}

		/* setup decompression process and source, then read JPEG header */
		jpeg_create_decompress(&cinfo);

		jpeg_mem_src(&cinfo, const_cast<unsigned char*>(data), dataLen);

		/* reading the image header which contains image information */
		jpeg_read_header(&cinfo, TRUE);

		// we only support RGB or grayscale
		if (cinfo.jpeg_color_space == JCS_GRAYSCALE)
		{
			_renderFormat = Texture2D::PixelFormat::I8;
		}
		else
		{
			cinfo.out_color_space = JCS_RGB;
			_renderFormat = Texture2D::PixelFormat::RGB888;
		}

		/* Start decompression jpeg here */
		jpeg_start_decompress(&cinfo);

		/* init image info */
		_width = cinfo.output_width;
		_height = cinfo.output_height;
		_hasPremultipliedAlpha = false;

		_dataLen = cinfo.output_width*cinfo.output_height*cinfo.output_components;
		_data = static_cast<unsigned char*>(malloc(_dataLen * sizeof(unsigned char)));

		if (!_data)
		{
			break;
		}

		/* now actually read the jpeg into the raw buffer */
		/* read one scan line at a time */
		while (cinfo.output_scanline < cinfo.output_height)
		{
			row_pointer[0] = _data + location;
			location += cinfo.output_width*cinfo.output_components;
			jpeg_read_scanlines(&cinfo, row_pointer, 1);
		}

		/* When read image file with broken data, jpeg_finish_decompress() may cause error.
		* Besides, jpeg_destroy_decompress() shall deallocate and release all memory associated
		* with the decompression object.
		* So it doesn't need to call jpeg_finish_decompress().
		*/
		//jpeg_finish_decompress( &cinfo );
		jpeg_destroy_decompress(&cinfo);
		/* wrap up decompression, destroy objects, free pointers and close open files */
		ret = true;
	} while (0);

	return ret;
}

namespace
{
	typedef struct
	{
		const unsigned char * data;
		ssize_t size;
		int offset;
	}tImageSource;

	static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);

		if ((int)(isource->offset + length) <= isource->size)
		{
			memcpy(data, isource->data + isource->offset, length);
			isource->offset += length;
		}
		else
		{
			png_error(png_ptr, "pngReaderCallback failed");
		}
	}
static void pngErrorCallback(png_structp png_ptr, png_const_charp msg)
{
    CCLOG("libpng error: %s", msg);
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void pngWarningCallback(png_structp png_ptr, png_const_charp msg)
{
    CCLOG("libpng warning: %s", msg);
}
}

#define HMS_BREAK_IF(cond) if(cond) break

bool Image::initWithPngData(const unsigned char * data, ssize_t dataLen)
{
	// length of bytes to check if it is a valid png file
#define PNGSIGSIZE  8



	bool ret = false;
	png_byte        header[PNGSIGSIZE] = { 0 };
	png_structp     png_ptr = 0;
	png_infop       info_ptr = 0;

	do
	{
		// png header len is 8 bytes
		HMS_BREAK_IF(dataLen < PNGSIGSIZE);

		// check the data is png or not
		memcpy(header, data, PNGSIGSIZE);
		HMS_BREAK_IF(png_sig_cmp(header, 0, PNGSIGSIZE));

		// init png_struct
//		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        png_ptr = png_create_read_struct(
            PNG_LIBPNG_VER_STRING,
            nullptr,
            pngErrorCallback,
            pngWarningCallback
        );
		HMS_BREAK_IF(!png_ptr);

		// init png_info
		info_ptr = png_create_info_struct(png_ptr);
		HMS_BREAK_IF(!info_ptr);

		HMS_BREAK_IF(setjmp(png_jmpbuf(png_ptr)));

		// set the read call back function
		tImageSource imageSource;
		imageSource.data = (unsigned char*)data;
		imageSource.size = dataLen;
		imageSource.offset = 0;
		png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

		// read png header info

		// read png file info
		png_read_info(png_ptr, info_ptr);

		auto number_passes = png_set_interlace_handling(png_ptr);

		_width = png_get_image_width(png_ptr, info_ptr);
		_height = png_get_image_height(png_ptr, info_ptr);
		png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

		//CCLOG("color type %u", color_type);

		// force palette images to be expanded to 24-bit RGB
		// it may include alpha channel
		if (color_type == PNG_COLOR_TYPE_PALETTE)
		{
			png_set_palette_to_rgb(png_ptr);
		}
		// low-bit-depth grayscale images are to be expanded to 8 bits
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		{
			bit_depth = 8;
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		}
		// expand any tRNS chunk data into a full alpha channel
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		{
			png_set_tRNS_to_alpha(png_ptr);
		}
		// reduce images with 16-bit samples to 8 bits
		if (bit_depth == 16)
		{
			png_set_strip_16(png_ptr);
		}

		// Expanded earlier for grayscale, now take care of palette and rgb
		if (bit_depth < 8)
		{
			png_set_packing(png_ptr);
		}
		// update info
		png_read_update_info(png_ptr, info_ptr);
		bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		color_type = png_get_color_type(png_ptr, info_ptr);

		switch (color_type)
		{
		case PNG_COLOR_TYPE_GRAY:
			_renderFormat = Texture2D::PixelFormat::I8;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			_renderFormat = Texture2D::PixelFormat::AI88;
			break;
		case PNG_COLOR_TYPE_RGB:
			_renderFormat = Texture2D::PixelFormat::RGB888;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			_renderFormat = Texture2D::PixelFormat::RGBA8888;
			break;
		default:
			break;
		}

		// read png data
		png_size_t rowbytes;
		png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * _height);

		rowbytes = png_get_rowbytes(png_ptr, info_ptr);

		_dataLen = rowbytes * _height;
		_data = static_cast<unsigned char*>(malloc(_dataLen * sizeof(unsigned char)));
		if (!_data)
		{
			if (row_pointers != nullptr)
			{
				free(row_pointers);
			}
			break;
		}

		for (unsigned short i = 0; i < _height; ++i)
		{
			row_pointers[i] = _data + i*rowbytes;
		}
		
		png_read_image(png_ptr, row_pointers);

		png_read_end(png_ptr, nullptr);

		// premultiplied alpha for RGBA8888
		if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		{
			premultipliedAlpha();
		}
		else
		{
			_hasPremultipliedAlpha = false;
		}

		if (row_pointers != nullptr)
		{
			free(row_pointers);
		}

		ret = true;
	} while (0);

	if (png_ptr)
	{
		png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
	}
	return ret;
}


bool Image::initWithRawData(const unsigned char * data, ssize_t dataLen, int width, int height, int bitsPerComponent, bool preMulti)
{
    bool ret = false;
    do 
    {
        HMS_BREAK_IF(0 == width || 0 == height);

        _height   = height;
        _width    = width;
        _hasPremultipliedAlpha = preMulti;
        _renderFormat = Texture2D::PixelFormat::RGBA8888;

        // only RGBA8888 supported
        int bytesPerComponent = 4;
        _dataLen = height * width * bytesPerComponent;
        _data = static_cast<unsigned char*>(malloc(_dataLen * sizeof(unsigned char)));
        HMS_BREAK_IF(! _data);
        memcpy(_data, data, _dataLen);

        ret = true;
    } while (0);

    return ret;
}

bool Image::initWithRawDataRGB888(const unsigned char * data, ssize_t dataLen, int width, int height, int bitsPerComponent, bool preMulti)
{
	bool ret = false;
	do
	{
		HMS_BREAK_IF(0 == width || 0 == height);

		_height = height;
		_width = width;
		_hasPremultipliedAlpha = preMulti;
		_renderFormat = Texture2D::PixelFormat::RGB888;

		// only RGB888 supported
		int bytesPerComponent = 3;
		_dataLen = height * width * bytesPerComponent;
		_data = static_cast<unsigned char*>(malloc(_dataLen * sizeof(unsigned char)));
		HMS_BREAK_IF(!_data);
		memcpy(_data, data, _dataLen);

		ret = true;
	} while (0);

	return ret;
}

bool Image::UpdateWidthRawData(const unsigned char * data, int x, int y, int width, int height, int bytesPerComponent)
{
	if (_renderFormat == Texture2D::PixelFormat::RGBA8888)
	{
		if (bytesPerComponent != 4)
		{
			return false;
		}
	}
	else if (_renderFormat == Texture2D::PixelFormat::RGB888)
	{
		if (bytesPerComponent != 3)
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	if (x + width > _width || y + height > _height || _data == nullptr)
	{
		return false;
	}
	for (int row = 0; row < height; ++row)
	{
		auto pDst = _data + ((y + row)*_width + x)*bytesPerComponent;
		auto pSrc = data + (row * width)*bytesPerComponent;
		memcpy(pDst, pSrc, width * bytesPerComponent);
	}
    return true;
}


bool Image::saveImageToPNG(const std::string& filePath, bool isToRGB)
{
	bool ret = false;
	do
	{
		FILE *fp;
		png_structp png_ptr;
		png_infop info_ptr;
		png_colorp palette;
		png_bytep *row_pointers;

		//fp = fopen(FileUtils::getInstance()->getSuitableFOpen(filePath).c_str(), "wb");
		fp = fopen(filePath.c_str(), "wb");
		HMS_BREAK_IF(nullptr == fp);

		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (nullptr == png_ptr)
		{
			fclose(fp);
			break;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (nullptr == info_ptr)
		{
			fclose(fp);
			png_destroy_write_struct(&png_ptr, nullptr);
			break;
		}

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fclose(fp);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			break;
		}

		png_init_io(png_ptr, fp);

		if (!isToRGB && hasAlpha())
		{
			png_set_IHDR(png_ptr, info_ptr, _width, _height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		}
		else
		{
			png_set_IHDR(png_ptr, info_ptr, _width, _height, 8, PNG_COLOR_TYPE_RGB,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		}

		palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
		png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);

		png_write_info(png_ptr, info_ptr);

		png_set_packing(png_ptr);

		row_pointers = (png_bytep *)malloc(_height * sizeof(png_bytep));
		if (row_pointers == nullptr)
		{
			fclose(fp);
			png_destroy_write_struct(&png_ptr, &info_ptr);
			break;
		}

		if (!hasAlpha())
		{
			for (int i = 0; i < (int)_height; i++)
			{
				row_pointers[i] = (png_bytep)_data + i * _width * 3;
			}

			png_write_image(png_ptr, row_pointers);

			free(row_pointers);
			row_pointers = nullptr;
		}
		else
		{
			if (isToRGB)
			{
				unsigned char *tempData = static_cast<unsigned char*>(malloc(_width * _height * 3 * sizeof(unsigned char)));
				if (nullptr == tempData)
				{
					fclose(fp);
					png_destroy_write_struct(&png_ptr, &info_ptr);

					free(row_pointers);
					row_pointers = nullptr;
					break;
				}

				for (int i = 0; i < _height; ++i)
				{
					for (int j = 0; j < _width; ++j)
					{
						tempData[(i * _width + j) * 3] = _data[(i * _width + j) * 4];
						tempData[(i * _width + j) * 3 + 1] = _data[(i * _width + j) * 4 + 1];
						tempData[(i * _width + j) * 3 + 2] = _data[(i * _width + j) * 4 + 2];
					}
				}

				for (int i = 0; i < (int)_height; i++)
				{
					row_pointers[i] = (png_bytep)tempData + i * _width * 3;
				}

				png_write_image(png_ptr, row_pointers);

				free(row_pointers);
				row_pointers = nullptr;

				if (tempData != nullptr)
				{
					free(tempData);
				}
			}
			else
			{
				for (int i = 0; i < (int)_height; i++)
				{
					row_pointers[i] = (png_bytep)_data + i * _width * 4;
				}

				png_write_image(png_ptr, row_pointers);

				free(row_pointers);
				row_pointers = nullptr;
			}
		}

		png_write_end(png_ptr, info_ptr);

		png_free(png_ptr, palette);
		palette = nullptr;

		png_destroy_write_struct(&png_ptr, &info_ptr);

		fclose(fp);

		ret = true;
	} while (0);
	return ret;
}

bool Image::saveImageToJPG(const std::string& filePath)
{
	bool ret = false;
	do
	{
		struct jpeg_compress_struct cinfo;
		struct jpeg_error_mgr jerr;
		FILE * outfile;                 /* target file */
		JSAMPROW row_pointer[1];        /* pointer to JSAMPLE row[s] */
		int     row_stride;          /* physical row width in image buffer */

		cinfo.err = jpeg_std_error(&jerr);
		/* Now we can initialize the JPEG compression object. */
		jpeg_create_compress(&cinfo);

		//HMS_BREAK_IF((outfile = fopen(FileUtils::getInstance()->GetSuitableFOpen(filePath).c_str(), "wb")) == nullptr);
		HMS_BREAK_IF((outfile = fopen(filePath.c_str(), "wb")) == nullptr);

		jpeg_stdio_dest(&cinfo, outfile);

		cinfo.image_width = _width;    /* image width and height, in pixels */
		cinfo.image_height = _height;
		cinfo.input_components = 3;       /* # of color components per pixel */
		cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, 90, TRUE);

		jpeg_start_compress(&cinfo, TRUE);

		row_stride = _width * 3; /* JSAMPLEs per row in image_buffer */

		if (hasAlpha())
		{
			unsigned char *tempData = static_cast<unsigned char*>(malloc(_width * _height * 3 * sizeof(unsigned char)));
			if (nullptr == tempData)
			{
				jpeg_finish_compress(&cinfo);
				jpeg_destroy_compress(&cinfo);
				fclose(outfile);
				break;
			}

			for (int i = 0; i < _height; ++i)
			{
				for (int j = 0; j < _width; ++j)

				{
					tempData[(i * _width + j) * 3] = _data[(i * _width + j) * 4];
					tempData[(i * _width + j) * 3 + 1] = _data[(i * _width + j) * 4 + 1];
					tempData[(i * _width + j) * 3 + 2] = _data[(i * _width + j) * 4 + 2];
				}
			}

			while (cinfo.next_scanline < cinfo.image_height)
			{
				row_pointer[0] = &tempData[cinfo.next_scanline * row_stride];
				(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
			}

			if (tempData != nullptr)
			{
				free(tempData);
			}
		}
		else
		{
			while (cinfo.next_scanline < cinfo.image_height) {
				row_pointer[0] = &_data[cinfo.next_scanline * row_stride];
				(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
			}
		}

		jpeg_finish_compress(&cinfo);
		fclose(outfile);
		jpeg_destroy_compress(&cinfo);

		ret = true;
	} while (0);
	return ret;
}

void Image::premultipliedAlpha()
{
    HMSASSERT(_renderFormat == Texture2D::PixelFormat::RGBA8888, "The pixel format should be RGBA8888!");
    
    unsigned int* fourBytes = (unsigned int*)_data;
    for(int i = 0; i < _width * _height; i++)
    {
        unsigned char* p = _data + i * 4;
        fourBytes[i] = HMS_RGB_PREMULTIPLY_ALPHA(p[0], p[1], p[2], p[3]);
    }
    
    _hasPremultipliedAlpha = true;
}

bool Image::saveToFile(const std::string &filename, bool isToRGB /*= true*/)
{
	//only support for Texture2D::PixelFormat::RGB888 or Texture2D::PixelFormat::RGBA8888 uncompressed data
	if (isCompressed() || (_renderFormat != Texture2D::PixelFormat::RGB888 && _renderFormat != Texture2D::PixelFormat::RGBA8888))
	{
		CCLOG("cocos2d: Image: saveToFile is only support for Texture2D::PixelFormat::RGB888 or Texture2D::PixelFormat::RGBA8888 uncompressed data for now");
		return false;
	}

	std::string fileExtension = CHmsFileUtils::getInstance()->getFileExtension(filename);

	if (fileExtension == ".png")
	{
		return saveImageToPNG(filename, isToRGB);
	}
	else if (fileExtension == ".jpg")
	{
		return saveImageToJPG(filename);
	}
	else
	{
		CCLOG("cocos2d: Image: saveToFile no support file extension(only .png or .jpg) for file: %s", filename.c_str());
		return false;
	}
}

GLboolean Image::GenMipMap2D(GLubyte *src, GLubyte **dst, int srcWidth, int srcHeight, int *dstWidth, int *dstHeight)
{
	int x = 0, y = 0;
	int texelSize = 3;

	*dstWidth = srcWidth / 2;

	if (*dstWidth <= 0)
	{
		*dstWidth = 1;
	}

	*dstHeight = srcHeight / 2;

	if (*dstHeight <= 0)
	{
		*dstHeight = 1;
	}

	*dst = new GLubyte[sizeof (GLubyte)* texelSize * (*dstWidth) * (*dstHeight)];

	if (*dst == nullptr)
	{
		return GL_FALSE;
	}

	for (y = 0; y < *dstHeight; y++)
	{
		for (x = 0; x < *dstWidth; x++)
		{
			int srcIndex[4];
			float r = 0.0f,
				g = 0.0f,
				b = 0.0f;
			int sample;

			// Compute the offsets for 2x2 grid of pixels in previous
			// image to perform box filter
			srcIndex[0] =
				(((y * 2) * srcWidth) + (x * 2)) * texelSize;
			srcIndex[1] =
				(((y * 2) * srcWidth) + (x * 2 + 1)) * texelSize;
			srcIndex[2] =
				((((y * 2) + 1) * srcWidth) + (x * 2)) * texelSize;
			srcIndex[3] =
				((((y * 2) + 1) * srcWidth) + (x * 2 + 1)) * texelSize;

			// Sum all pixels
			for (sample = 0; sample < 4; sample++)
			{
				r += src[srcIndex[sample]];
				g += src[srcIndex[sample] + 1];
				b += src[srcIndex[sample] + 2];
			}

			// Average results
			r /= 4.0;
			g /= 4.0;
			b /= 4.0;

			// Store resulting pixels
			(*dst)[(y * (*dstWidth) + x) * texelSize] = (GLubyte)(r);
			(*dst)[(y * (*dstWidth) + x) * texelSize + 1] = (GLubyte)(g);
			(*dst)[(y * (*dstWidth) + x) * texelSize + 2] = (GLubyte)(b);
		}
	}

	return GL_TRUE;
}
void Image::GenerateMipmaps()
{
    if (_renderFormat != Texture2D::PixelFormat::RGB888)
    {
        return;
    }
	for (int i = 1; i < _numberOfMipmaps; ++i)
	{
		HMS_SAFE_DELETE_ARRAY(_mipmaps[i].address);
	}

	MipmapInfo mipmap;
	mipmap.address = (unsigned char*)_data;
	mipmap.len = static_cast<int>(_dataLen);
	_mipmaps[0] = mipmap;

	int level = 0;
	int width = 0;
	int height = 0;
	GLubyte *prevImage = nullptr;
	GLubyte *newImage = nullptr;

	level = 1;
	width = _width;
	height = _height;
	prevImage = _data;

	while (width > 1 && height > 1)
	{
		int newWidth, newHeight;

		// Generate the next mipmap level
		GenMipMap2D(prevImage, &newImage, width, height, &newWidth, &newHeight);

		MipmapInfo mipmap;
		mipmap.address = (unsigned char*)newImage;
		mipmap.len = static_cast<int>(sizeof (GLubyte) * 3 * newWidth * newHeight);

		_mipmaps[level] = mipmap;

		// Set the previous image for the next iteration
		prevImage = newImage;
		++level;

		// Half the width and height
		width = newWidth;
		height = newHeight;
	}

	_numberOfMipmaps = level;
	_unpack = true;
}

NS_HMS_END

