/*
  TIFF configuration for Apple platforms (iOS/macOS).
  Uses C99 stdint.h types so that uint64/int64 etc. are portable.
*/

#ifndef _TIFFCONF_
#define _TIFFCONF_

#include <stdint.h>
#include <stddef.h>

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* Signed 8-bit type */
#define TIFF_INT8_T int8_t

/* Unsigned 8-bit type */
#define TIFF_UINT8_T uint8_t

/* Signed 16-bit type */
#define TIFF_INT16_T int16_t

/* Unsigned 16-bit type */
#define TIFF_UINT16_T uint16_t

/* Signed 32-bit type formatter */
#define TIFF_INT32_FORMAT "%d"

/* Signed 32-bit type */
#define TIFF_INT32_T int32_t

/* Unsigned 32-bit type formatter */
#define TIFF_UINT32_FORMAT "%u"

/* Unsigned 32-bit type */
#define TIFF_UINT32_T uint32_t

/* Signed 64-bit type formatter (C99 / POSIX) */
#define TIFF_INT64_FORMAT "%lld"

/* Signed 64-bit type */
#define TIFF_INT64_T int64_t

/* Unsigned 64-bit type formatter */
#define TIFF_UINT64_FORMAT "%llu"

/* Unsigned 64-bit type */
#define TIFF_UINT64_T uint64_t

/* Signed size type */
#if defined(__LP64__) || defined(_WIN64)
#define TIFF_SSIZE_T int64_t
#define TIFF_SSIZE_FORMAT "%lld"
#else
#define TIFF_SSIZE_T int32_t
#define TIFF_SSIZE_FORMAT "%d"
#endif

/* Pointer difference type */
#define TIFF_PTRDIFF_T ptrdiff_t

#define HAVE_IEEEFP 1
#define HOST_FILLORDER FILLORDER_LSB2MSB
#define HOST_BIGENDIAN 0

#define CCITT_SUPPORT 1
#define LOGLUV_SUPPORT 1
#define LZW_SUPPORT 1
#define NEXT_SUPPORT 1
#define PACKBITS_SUPPORT 1
#define THUNDER_SUPPORT 1
#define STRIPCHOP_DEFAULT TIFF_STRIPCHOP
#define SUBIFD_SUPPORT 1
#define DEFAULT_EXTRASAMPLE_AS_ALPHA 1
#define CHECK_JPEG_YCBCR_SUBSAMPLING 1

#define COLORIMETRY_SUPPORT
#define YCBCR_SUPPORT
#define CMYK_SUPPORT
#define ICC_SUPPORT
#define PHOTOSHOP_SUPPORT
#define IPTC_SUPPORT

#endif /* _TIFFCONF_ */
