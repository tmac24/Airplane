#ifndef __OSA_COMMON_H__
#define __OSA_COMMON_H__

 

/*------------------------------------------------------------------------
**                            文字量和宏声明                              
**------------------------------------------------------------------------
*/

/// unused parameter defines to suppress not use warning
#define UNUSED_PARAMETER(x) ((void)(x))

#ifdef __cplusplus
#define EXTERN_C_BLOCKSTART extern "C" {
#define EXTERN_C_BLOCKEND }
#else // __cplusplus
#define EXTERN_C_BLOCKSTART
#define EXTERN_C_BLOCKEND
#endif // __cplusplus

//http://www.transmissionzero.co.uk/computing/building-dlls-with-mingw/
#if defined(_WIN32)
#if defined DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#elif defined(DLL_IMPORTS)
#define DLL_API __declspec(dllimport)
#else
#define DLL_API
#endif
#else
#define DLL_API
#endif

#if defined DEBUG || defined UTEST
#define PRIVATE
#else
#define PRIVATE static
#endif

#define PUBLIC

#define COUNT_OF(x)		(sizeof(x) / sizeof((x)[0]))

#if defined(NO_INLINE)
#define INLINE static
#elif defined(_MSC_VER)
#define INLINE static __inline
#elif defined(__GNUC__)
#define INLINE static __inline__
#elif defined(__IAR_SYSTEMS_ICC__)
/* Reference to http://netstorage.iar.com/SuppDB/Public/SUPPORT/005605/Example%201.pdf
 * 如果要打开内联函数, IAR编译器优化要设定为中级或以上,
 * 具体在Project->Options->C/C++ compiler->Optimizations->Level(Medium or High)
 */
#define PRAGMA(x) _Pragma(#x)
#define INLINE PRAGMA(inline=forced)
#elif __STDC_VERSION__ >= 199901L
/* "inline" is a keyword */
#define INLINE inline
#elif !defined(INLINE)
#define INLINE static
#endif

#ifndef MIN
#define MIN(a, b)	((a) <= (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) >= (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(v)		((v) >= 0 ? (v) : -(v))
#endif

//结构体转换成特定数据值
#define STRUCT_TO_VALUE(type, s)	(*((type *)&(s)))

#define OSA_MUTEX_PRIO_BEGIN       0
#define OSA_TASK_PRIO_CUSTOM       8
#define OSA_TASK_PRIO_BEGIN        70

#ifdef ATOMIC_CHECK
static int atomic_counter = 0;
INLINE void atomic_counter_increase()
{
	++atomic_counter;
	if (atomic_counter != 1)
	{
		atomic_counter = atomic_counter;
	}
}
INLINE void atomic_counter_decrease()
{
	--atomic_counter;
	if (atomic_counter != 0)
	{
		atomic_counter = atomic_counter;
	}
}
#else
#define atomic_counter_increase() 
#define atomic_counter_decrease()
#endif

#if OS_CRITICAL_METHOD == 3u
#define OSA_ATOMIC_DECL(x) OS_CPU_SR x = 0;
#define OSA_ATOMIC_LOCK(x) {x=OS_CPU_SR_Save(); atomic_counter_increase();}
#define OSA_ATOMIC_UNLOCK(x) {atomic_counter_decrease(); OS_CPU_SR_Restore(x);}
#else
#define OSA_ATOMIC_DECL(x)
#define OSA_ATOMIC_LOCK(x)
#define OSA_ATOMIC_UNLOCK(x)
#endif

/*------------------------------------------------------------------------
**                              变量声明                                  
**------------------------------------------------------------------------
*/


/*------------------------------------------------------------------------
**                              函数声明                                  
**------------------------------------------------------------------------
*/


#endif /* __OSA_COMMON_H__ */
