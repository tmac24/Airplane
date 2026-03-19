#pragma once
#include <stdio.h>
#include <limits.h>
#include <stddef.h>
#ifdef __cplusplus
#define NS_HMS_BEGIN                     namespace HmsAviPf {
#define NS_HMS_END                       }
#define USING_NS_HMS                     using namespace HmsAviPf
#define NS_HMS                           ::HmsAviPf
#else
#define NS_HMS_BEGIN 
#define NS_HMS_END 
#define USING_NS_HMS 
#define NS_HMS
#endif 

#if 0
#if defined(_USRDLL)
#define HMS_DLL     __declspec(dllexport)
#else         /* use a DLL library */
#define HMS_DLL     __declspec(dllimport)
#endif
#else
#define HMS_DLL
#endif

#define HMS_UNUSED
#define HMS_UNUSED_PARAM

#define HMS_SAFE_DELETE(p)           do { if(p) { delete (p); (p) = nullptr; } } while(0)
#define HMS_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define HMS_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define HMS_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define HMS_SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define HMS_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define HMS_BREAK_IF(cond)           if(cond) break

#define HMSASSERT(cond, msg)
#define HMS_ASSERT(cond)
#define GP_ASSERT(cond)

#ifndef HMS_ARRAYSIZE
#define HMS_ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

#define STD_NOTHROW /* (std::nothrow)*/

#define HMS_REPEAT_FOREVER (UINT_MAX -1)
/**
*A macro to disallow the copy constructor and operator= functions.
*This should be used in the private: declarations for a class
*param   
*returns 
*by [3/24/2017 song.zhang079] 
*/
#if defined(__GNUC__) && ((__GNUC__ >= 5) || ((__GNUG__ == 4) && (__GNUC_MINOR__ >= 4))) \
    || (defined(__clang__) && (__clang_major__ >= 3)) || (_MSC_VER >= 1800)
#define HMS_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete; \
    TypeName &operator =(const TypeName &) = delete;
#else
#define HMS_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &); \
    TypeName &operator =(const TypeName &);
#endif

/**
* A macro to disallow all the implicit constructors, namely the
* default constructor, copy constructor and operator= functions.
*
* This should be used in the private: declarations for a class
* that wants to prevent anyone from instantiating it. This is
* especially useful for classes containing only static methods.
*param   
*returns 
*by [3/24/2017 song.zhang079] 
*/
#define HMS_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName)    \
    TypeName();                                        \
    HMS_DISALLOW_COPY_AND_ASSIGN(TypeName)


/**
*Only certain compiler support __attribute__((format))
*
*param   formatPos 1-based position of format string argument.
*param	 argPos    1-based position of first format-dependent argument.
*returns 
*by [3/24/2017 song.zhang079] 
*/
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define HMS_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
#if __has_attribute(format)
#define HMS_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#endif // __has_attribute(format)
#else
#define HMS_FORMAT_PRINTF(formatPos, argPos)
#endif

#if defined(_MSC_VER)
#define HMS_FORMAT_PRINTF_SIZE_T "%08lX"
#else
#define HMS_FORMAT_PRINTF_SIZE_T "%08zX"
#endif

/**
*This attribute , attached to a function, means that the function is meant to be possibly unused.
*GCC will not produce a warning for this function
*param   
*returns 
*by [3/24/2017 song.zhang079] 
*/
#if 0
#if defined(__GNUC__)
#define HMS_UNUSED __attribute__ ((unused))
#else
#define HMS_UNUSED
#endif
#endif

#if 0
#define CCLOGERROR(...) 
#define CCLOGINFO(...)
#define CCLOG(...) 
#else

#ifdef __linux
#define CCLOG(...)
#else
//#define CCLOG(...)  printf(##__VA_ARGS__);
#define CCLOG(fmt, ...) \
        printf(fmt "\n", ##__VA_ARGS__)
//#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
//#define CCLOG(fmt, ...) \
//    printf("[%s:%d] " fmt "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)

#define CHECK_GL_ERROR_DEBUG() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            CCLOG("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
        } \
    } while (false)

#endif
#define CCLOGINFO(...)
#define CCLOGERROR CCLOG

#endif

#define CREATE_FUNC(__TYPE__) \
static __TYPE__* Create() \
{ \
    __TYPE__ *pRet = new __TYPE__(); \
    if (pRet && pRet->Init()) \
		    { \
        pRet->autorelease(); \
        return pRet; \
		    } \
			    else \
			    { \
        delete pRet; \
        pRet = nullptr; \
        return nullptr; \
			    } \
}

#define CREATE_FUNC_BY_SIZE(__TYPE__) \
	static __TYPE__* Create(const HmsAviPf::Size & size) \
{ \
	__TYPE__ *pRet = new __TYPE__(); \
if (pRet && pRet->Init(size)) \
			{ \
			pRet->autorelease(); \
			return pRet; \
			} \
				else \
				{ \
				delete pRet; \
				pRet = nullptr; \
				return nullptr; \
				} \
}

#define HmsSchedulerFunc  std::function<void(float)>

#define HMS_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define HMS_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define HMS_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define HMS_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define HMS_CALLBACK_4(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, \
    std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)
#define HMS_CALLBACK_5(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, \
    std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, ##__VA_ARGS__)
#define HMS_CALLBACK_6(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, \
    std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, ##__VA_ARGS__)
