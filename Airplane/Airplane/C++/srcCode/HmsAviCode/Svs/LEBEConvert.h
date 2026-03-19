#ifndef __ATOS_ENDIAN_H__
#define __ATOS_ENDIAN_H__
#include "GLHeader.h"
#if VxWorks_JINGJIA_5400
#define __LITTLE_ENDIAN__ 1
#else

#include <stdint.h>
#endif

#include "osa_common.h"

//Refer to http://sourceforge.net/apps/mediawiki/predef/index.php?title=Endianness
#if !(defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__))

#if defined(_MSC_VER)
#if defined(_M_IX86) || defined(_M_IA64) || defined(_M_X64) || defined(_M_AMD64)
#define __LITTLE_ENDIAN__
#else
#define __BIG_ENDIAN__
#endif//defined(_M_IX86) || defined(_M_IA64) || defined(_M_X64) || defined(_M_AMD64)
#elif defined(_BIG_ENDIAN) ||\
	defined(__ARMEB__) || defined(__THUMBEB__) || \
	defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#define __BIG_ENDIAN__
#elif defined(_LITTLE_ENDIAN) || \
	defined(__ARMEL__) || defined(__THUMBEL__) || \
	defined(_MIPSEL) || defined(_MIPSEL) || defined(__MIPSEL__)

#define __LITTLE_ENDIAN__
#else
#ifdef __vxworks//jy: not finding a more suitable macro, use __vxworks temporary, indicating that vxworks defines __LITTLE_ENDIAN__.
#define __LITTLE_ENDIAN__
#else
#ifndef __linux
{#error Unidentified endianness}
#endif
#endif
#endif//defined(_MSC_VER)

#endif//!(defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__))

#if defined(__BIG_ENDIAN__) && defined(__LITTLE_ENDIAN__)
{
#error __BIG_ENDIAN__ and __LITTLE_ENDIAN__ defined at the same time
}
#endif


EXTERN_C_BLOCKSTART

//http://stackoverflow.com/questions/2182002/convert-big-endian-to-little-endian-in-c-without-using-provided-func

  uint16_t byte_swap_uint16(uint16_t val)
{
	return (val << 8) | (val >> 8);
}

  int16_t byte_swap_int16(int16_t val)
{
	return (val << 8) | ((val >> 8) & 0xFF);
}

  uint32_t byte_swap_uint32(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

  int32_t byte_swap_int32(int32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | ((val >> 16) & 0xFFFF);
}

  int64_t byte_swap_int64(int64_t val)
{
	union 
	{
		int64_t i64;
		uint8_t buf[8];
	} tmp;
	uint8_t tmp_byte, i;

	tmp.i64 = val;
	for (i =  0; i < 4; i++)
	{
		tmp_byte = tmp.buf[i];
		tmp.buf[i] = tmp.buf[7-i];
		tmp.buf[7-i] = tmp_byte;
	}

	return tmp.i64;
}

  uint64_t byte_swap_uint64(uint64_t val)
{
	union
	{
		uint64_t u64;
		uint8_t buf[8];
	} tmp;
	uint8_t tmp_byte, i;

	tmp.u64 = val;
	for (i =  0; i < 4; i++)
	{
		tmp_byte = tmp.buf[i];
		tmp.buf[i] = tmp.buf[7-i];
		tmp.buf[7-i] = tmp_byte;
	}

	return tmp.u64;
}

  float byte_swap_float32(float val)
{
	union
	{
		float f32;
		uint8_t buf[4];
	} tmp;
	uint8_t tmp_byte, i;

	tmp.f32 = val;
	for (i =  0; i < 2; i++)
	{
		tmp_byte = tmp.buf[i];
		tmp.buf[i] = tmp.buf[3-i];
		tmp.buf[3-i] = tmp_byte;
	}

	return tmp.f32;
}

  double byte_swap_float64(double val)
{
	union
	{
		double f64;
		uint8_t buf[8];
	} tmp;
	uint8_t tmp_byte, i;

	tmp.f64 = val;


for (i =  0; i < 4; i++)
	{
		tmp_byte = tmp.buf[i];
		tmp.buf[i] = tmp.buf[7-i];
		tmp.buf[7-i] = tmp_byte;
	}
	return tmp.f64;
}



#define atos_get_data_ptr(ptr, offset) ((((uint8_t *)(ptr)) + (offset)))

/*
 *  uint16_t to be_uint16_t
 *  or be_uint16_t to uint16_t
 */

  uint16_t be_uint16(uint16_t val)
{
#if defined (__LITTLE_ENDIAN__)
	return byte_swap_uint16(val);
#else
	return val;
#endif
}

/*
*
* int16_t to be_int16_t
* or be_int16_t to int16_t
*/
  int16_t be_int16(int16_t val)
{

#if defined (__LITTLE_ENDIAN__)
	return byte_swap_int16(val);
#else
	return val;
#endif
}


/*
 *  uint32_t to be_uint32_t
 *  or be_uint32_t to uint32_t
 */

  uint32_t be_uint32(uint32_t val)
{
#if defined (__LITTLE_ENDIAN__)
	return byte_swap_uint32(val);
#else
	return val;
#endif
}

/*
* int32_t to be_int32_t 
* or be_int32_t to int32_t
*/
  int32_t be_int32(int32_t val)
{

#if defined (__LITTLE_ENDIAN__)
	return byte_swap_int32(val);
#else
	return val;
#endif
}

  uint64_t be_uint64(uint64_t val)
{
#if defined (__LITTLE_ENDIAN__)
	return byte_swap_uint64(val);
#else
	return val;
#endif
}

  int64_t be_int64(int64_t val)
{

#if defined (__LITTLE_ENDIAN__)
	return byte_swap_int64(val);
#else
	return val;
#endif
}

  float be_float32(float val)
{
#if defined (__LITTLE_ENDIAN__)
	return byte_swap_float32(val);
#else
	return val;
#endif

}

  double be_float64(double val)
{
#if defined (__LITTLE_ENDIAN__)
	return byte_swap_float64(val);
#else
	return val;
#endif

}


/*
 *  uint16_t to le_uint16_t
 *  or le_uint16_t to uint16_t
 */

  uint16_t le_uint16(uint16_t val)
{
#if defined (__BIG_ENDIAN__)
	return byte_swap_uint16(val);
#else
	return val;
#endif
}

/*
* int16_t to le_int16_t
* or le_uint16_t to uin16_t
*/
  int16_t le_int16(int16_t val)
{

#if defined (__BIG_ENDIAN__)
	return byte_swap_int16(val);
#else
	return val;
#endif
}

/*
 *  uint32_t to le_uint32_t
 *  or le_uint32_t to uint32_t
 */

  uint32_t le_uint32(uint32_t val)
{
#if defined (__BIG_ENDIAN__)
	return byte_swap_uint32(val);
#else
	return val;
#endif
}

/*
* int32_t to le_int32_t
* or le_int32_t to int32_t
*/
  int32_t le_int32(int32_t val)
{

#if defined (__BIG_ENDIAN__)
	return byte_swap_int32(val);
#else
	return val;
#endif
}

  uint64_t le_uint64(uint64_t val)
{
#if defined (__BIG_ENDIAN__)
	return byte_swap_uint64(val);
#else
	return val;
#endif
}

  int64_t le_int64(int64_t val)
{

#if defined (__BIG_ENDIAN__)
	return byte_swap_int64(val);
#else
	return val;
#endif
}

  float le_float32(float val)
{
#if defined (__BIG_ENDIAN__)
	return byte_swap_float32(val);
#else
	return val;
#endif
}

  double le_float64(double val)
{
#if defined (__BIG_ENDIAN__)
	return byte_swap_float64(val);
#else
	return val;
#endif
}


  uint8_t read_u8(void *buf, uint16_t offset)
{
	return *atos_get_data_ptr(buf, offset) ;
}

#define read_i8(buf, offset) (int8_t)read_u8(buf, offset)

/*
	This is used to read native uint16_t for un-aligned bytes
	such as pack(1)
*/
  uint16_t read_u16(void*buf, uint16_t offset)
{
	uint8_t* data = atos_get_data_ptr(buf, offset);
	uint16_t val;
	uint8_t* ptr = (uint8_t*)&val;
	ptr[0] = data[0];
	ptr[1] = data[1];
	return val;
}

#define read_i16(addr, offset) (int16_t)read_u16(addr)

/*
	This is used to read native uint32_t for un-aligned bytes
	such as pack(1) or pack(2)
*/
  uint32_t read_u32(void*buf, uint16_t offset)
{
	uint8_t* data = atos_get_data_ptr(buf, offset);
	uint32_t val;
	uint8_t* ptr = (uint8_t*)&val;
	ptr[0] = data[0];
	ptr[1] = data[1];
	ptr[2] = data[2];
	ptr[3] = data[3];
	return val;
}

  int32_t read_i32(void*buf, uint16_t offset)
{
	uint8_t* data = atos_get_data_ptr(buf, offset);
	int32_t val;
	uint8_t* ptr = (uint8_t*)&val;
	ptr[0] = data[0];
	ptr[1] = data[1];
	ptr[2] = data[2];
	ptr[3] = data[3];
	return val;
}

  uint64_t read_u64(void*buf, uint16_t offset)
{
	uint8_t* data = atos_get_data_ptr(buf, offset);
	uint64_t val;
	uint8_t* ptr = (uint8_t*)&val;
	ptr[0] = data[0];
	ptr[1] = data[1];
	ptr[2] = data[2];
	ptr[3] = data[3];
	ptr[4] = data[4];
	ptr[5] = data[5];
	ptr[6] = data[6];
	ptr[7] = data[7];
	return val;
}

  int64_t read_i64(void*buf, uint16_t offset)
{
	uint8_t* data = atos_get_data_ptr(buf, offset);
	int64_t val;
	uint8_t* ptr = (uint8_t*)&val;
	ptr[0] = data[0];
	ptr[1] = data[1];
	ptr[2] = data[2];
	ptr[3] = data[3];
	ptr[4] = data[4];
	ptr[5] = data[5];
	ptr[6] = data[6];
	ptr[7] = data[7];
	return val;
}


  float read_float32(void *buf, uint16_t offset)
{
	uint8_t* data = atos_get_data_ptr(buf, offset);
	float val;
	uint8_t* ptr = (uint8_t*)&val;
	ptr[0] = data[0];
	ptr[1] = data[1];
	ptr[2] = data[2];
	ptr[3] = data[3];
	return val;
}

  double read_float64(void *buf, uint16_t offset)
{
	uint8_t* data = atos_get_data_ptr(buf, offset);
	double val;
	uint8_t* ptr = (uint8_t*)&val;
	ptr[0] = data[0];
	ptr[1] = data[1];
	ptr[2] = data[2];
	ptr[3] = data[3];
	ptr[4] = data[4];
	ptr[5] = data[5];
	ptr[6] = data[6];
	ptr[7] = data[7];
	return val;
}

  uint16_t read_be_u16(void *buf, uint16_t offset)
{
	uint8_t * addr = atos_get_data_ptr(buf, offset);
	uint16_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);
	
	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	
	return be_uint16(temp_val);
}

#define read_be_i16(buf, offset) (int16_t)read_be_u16(buf, offset)

  uint16_t read_le_u16(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	uint16_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];

	return le_uint16(temp_val);
}

#define read_le_i16(buf, offset) (int16_t)read_le_u16(buf, offset)

  uint32_t read_be_u32(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	uint32_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];

	return be_uint32(temp_val);
}

  int32_t read_be_i32(void *buf, uint16_t offset)
{

	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int32_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];

	return be_int32(temp_val);
}

  uint64_t read_be_u64(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	uint64_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];
	temp_ptr[4] = addr[4];
	temp_ptr[5] = addr[5];
	temp_ptr[6] = addr[6];
	temp_ptr[7] = addr[7];

	return be_uint64(temp_val);
}

  int64_t read_be_i64(void *buf, uint16_t offset)
{

	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int64_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];
	temp_ptr[4] = addr[4];
	temp_ptr[5] = addr[5];
	temp_ptr[6] = addr[6];
	temp_ptr[7] = addr[7];

	return be_int64(temp_val);
}

  float read_be_float32(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	float temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];

	return be_float32(temp_val);
}

  double read_be_float64(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	double temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];
	temp_ptr[4] = addr[4];
	temp_ptr[5] = addr[5];
	temp_ptr[6] = addr[6];
	temp_ptr[7] = addr[7];

	return be_float64(temp_val);
}


  uint32_t read_le_u32(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	uint32_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];

	return le_uint32(temp_val);
}

  int32_t read_le_i32(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int32_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];

	return le_int32(temp_val);
}

  uint64_t read_le_u64(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	uint64_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];
	temp_ptr[4] = addr[4];
	temp_ptr[5] = addr[5];
	temp_ptr[6] = addr[6];
	temp_ptr[7] = addr[7];

	return le_uint64(temp_val);
}

  int64_t read_le_i64(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int64_t temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];
	temp_ptr[4] = addr[4];
	temp_ptr[5] = addr[5];
	temp_ptr[6] = addr[6];
	temp_ptr[7] = addr[7];

	return le_int64(temp_val);
}

  float read_le_float32(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	float temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];

	return le_float32(temp_val);
}

  double read_le_float64(void *buf, uint16_t offset)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	double temp_val = 0;
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	temp_ptr[0] = addr[0];
	temp_ptr[1] = addr[1];
	temp_ptr[2] = addr[2];
	temp_ptr[3] = addr[3];
	temp_ptr[4] = addr[4];
	temp_ptr[5] = addr[5];
	temp_ptr[6] = addr[6];
	temp_ptr[7] = addr[7];

	return le_float64(temp_val);
}

  void write_u8(void *buf, uint16_t offset, uint8_t val)
{
	*atos_get_data_ptr(buf, offset) = val;
}

  void write_i8(void *buf, uint16_t offset, int8_t val)
{
	*((int8_t *)atos_get_data_ptr(buf, offset)) = val;
}

  void write_be_u16(void *buf, uint16_t offset, uint16_t val)
{
	uint8_t * addr = atos_get_data_ptr(buf, offset);
	uint16_t tmp_val = be_uint16(val);
	*addr = (uint8_t)(tmp_val & 0xFF);
	*(addr + 1) = (uint8_t)((tmp_val >> 8) & 0xFF);
}

  void write_be_i16(void *buf, uint16_t offset, int16_t val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int16_t temp_val = be_int16(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);
	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
}

  void write_le_u16(void *buf, uint16_t offset, uint16_t val)
{
	uint8_t * addr = atos_get_data_ptr(buf, offset);
	uint16_t tmp_val = le_uint16(val);
	*addr = (uint8_t)(tmp_val & 0xFF);
	*(addr + 1) = (uint8_t)((tmp_val >> 8) & 0xFF);
}

  void write_le_i16(void *buf, uint16_t offset, int16_t val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int16_t temp_val = le_int16(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);
	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
}

  void write_be_u32(void *buf, uint16_t offset, uint32_t val)
{
	uint8_t * addr = atos_get_data_ptr(buf, offset);
	uint32_t tmp_val = be_uint32(val);
	*addr = (uint8_t)(tmp_val & 0xFF);
	*(addr + 1) = (uint8_t)((tmp_val >> 8) & 0xFF);
	*(addr + 2) = (uint8_t)((tmp_val >> 16) & 0xFF);
	*(addr + 3) = (uint8_t)((tmp_val >> 24) & 0xFF);
}

  void write_be_i32(void *buf, uint16_t offset, int32_t val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int32_t temp_val = be_int32(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
}

  void write_be_u64(void *buf, uint16_t offset, uint64_t val)
{
	uint8_t * addr = atos_get_data_ptr(buf, offset);
	uint64_t temp_val = be_uint64(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
	addr[4] = temp_ptr[4];
	addr[5] = temp_ptr[5];
	addr[6] = temp_ptr[6];
	addr[7] = temp_ptr[7];
}


  void write_be_i64(void *buf, uint16_t offset, int64_t val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int64_t temp_val = be_int64(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
	addr[4] = temp_ptr[4];
	addr[5] = temp_ptr[5];
	addr[6] = temp_ptr[6];
	addr[7] = temp_ptr[7];
}

  void write_be_float32(void *buf, uint16_t offset, float val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	float temp_val = be_float32(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
}

  void write_be_float64(void *buf, uint16_t offset, double val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	double temp_val = be_float64(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
	addr[4] = temp_ptr[4];
	addr[5] = temp_ptr[5];
	addr[6] = temp_ptr[6];
	addr[7] = temp_ptr[7];
}
	
  void write_le_u32(void *buf, uint16_t offset, uint32_t val)
{
	uint8_t * addr = atos_get_data_ptr(buf, offset);
	uint32_t tmp_val = le_uint32(val);
	*addr = (uint8_t)(tmp_val & 0xFF);
	*(addr + 1) = (uint8_t)((tmp_val >> 8) & 0xFF);
	*(addr + 2) = (uint8_t)((tmp_val >> 16) & 0xFF);
	*(addr + 3) = (uint8_t)((tmp_val >> 24) & 0xFF);

}

  void write_le_i32(void *buf, uint16_t offset, int32_t val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int32_t temp_val = le_int32(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
}

  void write_le_u64(void *buf, uint16_t offset, uint64_t val)
{
	uint8_t * addr = atos_get_data_ptr(buf, offset);
	uint64_t temp_val = le_uint64(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
	addr[4] = temp_ptr[4];
	addr[5] = temp_ptr[5];
	addr[6] = temp_ptr[6];
	addr[7] = temp_ptr[7];
}

  void write_le_i64(void *buf, uint16_t offset, int64_t val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	int64_t temp_val = le_int64(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
	addr[4] = temp_ptr[4];
	addr[5] = temp_ptr[5];
	addr[6] = temp_ptr[6];
	addr[7] = temp_ptr[7];
}

  void write_le_float32(void *buf, uint16_t offset, float val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	float temp_val = le_float32(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
}

  void write_le_float64(void *buf, uint16_t offset, double val)
{
	uint8_t *addr = atos_get_data_ptr(buf, offset);
	double temp_val = le_float64(val);
	uint8_t *temp_ptr = (uint8_t *)(&temp_val);

	addr[0] = temp_ptr[0];
	addr[1] = temp_ptr[1];
	addr[2] = temp_ptr[2];
	addr[3] = temp_ptr[3];
	addr[4] = temp_ptr[4];
	addr[5] = temp_ptr[5];
	addr[6] = temp_ptr[6];
	addr[7] = temp_ptr[7];
}

  uint8_t swap_bits8(uint8_t data, uint8_t bit_count)
{
	uint8_t result = 0;
	uint8_t i;
	for (i = 0; i < bit_count; ++i)
	{
		if (data & (1 << i))
		{
			result |= 1 << (bit_count - 1 - i);
		}
	}

	return result;
}

  uint16_t swap_bits16(uint16_t data, uint8_t bit_count)
{
	uint16_t result = 0;
	uint8_t i;
	for (i = 0; i < bit_count; ++i)
	{
		if (data & (1 << i))
		{
			result |= 1 << (bit_count - 1 - i);
		}
	}

	return result;
}

  uint32_t swap_bits32(uint32_t data, uint8_t bit_count)
{
	uint32_t result = 0;
	uint8_t i;
	for (i = 0; i < bit_count; ++i)
	{
		if (data & (1 << i))
		{
			result |= 1 << (bit_count - 1 - i);
		}
	}

	return result;
}

#if defined(__BIG_ENDIAN__)
#define BE_FILED8_GET(v, bit_count)  (v)
#define LE_FILED8_GET(v, bit_count)  swap_bits8((v), bit_count)

#define BE_FILED16_GET(v, bit_count)  (v)
#define LE_FILED16_GET(v, bit_count)  swap_bits16((v), bit_count)

#define BE_FILED32_GET(v, bit_count)  (v)
#define LE_FILED32_GET(v, bit_count)  swap_bits32((v), bit_count)

/*
大端模式下位字段定义如下：
struct
{
uint8_t bit7:1;		//高位在前
uint8_t bit6:1;
uint8_t bit5:1;
uint8_t bit4:1;
uint8_t bit3:1;
uint8_t bit2:1;
uint8_t bit1:1;
uint8_t bit0:1;
};
*/
#else
	#define BE_FILED8_GET(v, bit_count)  swap_bits8((v), bit_count)
	#define LE_FILED8_GET(v, bit_count)  (v)

	#define BE_FILED16_GET(v, bit_count)  swap_bits16((v), bit_count)
	#define LE_FILED16_GET(v, bit_count)  (v)

	#define BE_FILED32_GET(v, bit_count)  swap_bits32((v), bit_count)
	#define LE_FILED32_GET(v, bit_count)  (v)

/*
小端模式下位字段定义如下：
struct
{
uint8_t bit0:1;		//低位在前（符合低位在低地址原则吧）
uint8_t bit1:1;
uint8_t bit2:1;
uint8_t bit3:1;
uint8_t bit4:1;
uint8_t bit5:1;
uint8_t bit6:1;
uint8_t bit7:1;
};
*/
#endif


#if 0

#ifndef _BV
#define _BV(n)  (1 << (n))
#endif

#ifndef TEST_BIT
#define TEST_BIT(v, n)  (((v) & _BV(n)) == _BV(n))
#endif

#define CLR_BIT(v, n)   do { (v) &= ~_BV(n); } while (0)
#define SET_BIT(v, n)   do { (v) |= _BV(n); } while (0)

#define MAKE_BIT(v, n, b) do {\
	if (b) SET_BIT(v, n); \
	else   CLR_BIT(v, n);\
} while (0)

#define CLR_ALL_BITS(v)  do { (v) = 0x00000000; } while (0)
#define SET_ALL_BITS(v)  do { (v) = 0xFFFFFFFF; } while (0)

#endif

EXTERN_C_BLOCKEND

#endif /*__ATOS_ENDIAN_H__*/
