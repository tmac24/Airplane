#pragma once

#ifndef __HMS_REF_PTR_H__
#define __HMS_REF_PTR_H__

#include "base/Ref.h"
#include "HmsAviMacros.h"
#include <type_traits>

NS_HMS_BEGIN

/**
* Utility/support macros. Defined to enable RefPtr<T> to contain types like 'const T' because we do not
* regard retain()/release() as affecting mutability of state.
*/
#define HMS_REF_PTR_SAFE_RETAIN(ptr)\
    \
    do\
	    {\
        if (ptr)\
		        {\
            const_cast<Ref*>(static_cast<const Ref*>(ptr))->retain();\
		        }\
    \
	    }   while (0);

#define HMS_REF_PTR_SAFE_RELEASE(ptr)\
    \
    do\
	    {\
        if (ptr)\
		        {\
            const_cast<Ref*>(static_cast<const Ref*>(ptr))->release();\
		        }\
    \
	    }   while (0);

#define HMS_REF_PTR_SAFE_RELEASE_NULL(ptr)\
    \
    do\
	    {\
        if (ptr)\
		        {\
            const_cast<Ref*>(static_cast<const Ref*>(ptr))->release();\
            ptr = nullptr;\
		        }\
    \
	    }   while (0);

/**
* Wrapper class which maintains a strong reference to a cocos2dx HmsAviPf::Ref* type object.
* Similar in concept to a boost smart pointer.
*
* Enables the use of the RAII idiom with Cocos2dx objects and helps automate some of the more
* mundane tasks of pointer initialization and cleanup.
*
* The class itself is modelled on C++ 11 std::shared_ptr, and trys to keep some of the methods
* and functionality consistent with std::shared_ptr.
*/
template <typename T> class RefPtr
{
public:

	inline RefPtr()
		:
		_ptr(nullptr)
	{

	}

	inline RefPtr(RefPtr<T> && other)
	{
		_ptr = other._ptr;
		other._ptr = nullptr;
	}

	inline RefPtr(T * ptr)
		:
		_ptr(const_cast<typename std::remove_const<T>::type*>(ptr))     // Const cast allows RefPtr<T> to reference objects marked const too.
	{
		HMS_REF_PTR_SAFE_RETAIN(_ptr);
	}

	inline RefPtr(std::nullptr_t ptr)
		:
		_ptr(nullptr)
	{

	}

	inline RefPtr(const RefPtr<T> & other)
		:
		_ptr(other._ptr)
	{
		HMS_REF_PTR_SAFE_RETAIN(_ptr);
	}

	inline ~RefPtr()
	{
		HMS_REF_PTR_SAFE_RELEASE_NULL(_ptr);
	}

	inline RefPtr<T> & operator = (const RefPtr<T> & other)
	{
		if (other._ptr != _ptr)
		{
			HMS_REF_PTR_SAFE_RETAIN(other._ptr);
			HMS_REF_PTR_SAFE_RELEASE(_ptr);
			_ptr = other._ptr;
		}

		return *this;
	}

	inline RefPtr<T> & operator = (RefPtr<T> && other)
	{
		if (&other != this)
		{
			HMS_REF_PTR_SAFE_RELEASE(_ptr);
			_ptr = other._ptr;
			other._ptr = nullptr;
		}

		return *this;
	}

	inline RefPtr<T> & operator = (T * other)
	{
		if (other != _ptr)
		{
			HMS_REF_PTR_SAFE_RETAIN(other);
			HMS_REF_PTR_SAFE_RELEASE(_ptr);
			_ptr = const_cast<typename std::remove_const<T>::type*>(other);     // Const cast allows RefPtr<T> to reference objects marked const too.
		}

		return *this;
	}

	inline RefPtr<T> & operator = (std::nullptr_t other)
	{
		HMS_REF_PTR_SAFE_RELEASE_NULL(_ptr);
		return *this;
	}

	// Note: using reinterpret_cast<> instead of static_cast<> here because it doesn't require type info.
	// Since we verify the correct type cast at compile time on construction/assign we don't need to know the type info
	// here. Not needing the type info here enables us to use these operations in inline functions in header files when
	// the type pointed to by this class is only forward referenced.

	inline operator T * () const { return reinterpret_cast<T*>(_ptr); }

	inline T & operator * () const
	{
		HMSASSERT(_ptr, "Attempt to dereference a null pointer!");
		return reinterpret_cast<T&>(*_ptr);
	}

	inline T * operator->() const
	{
		HMSASSERT(_ptr, "Attempt to dereference a null pointer!");
		return reinterpret_cast<T*>(_ptr);
	}

	inline T * get() const { return reinterpret_cast<T*>(_ptr); }


	inline bool operator == (const RefPtr<T> & other) const { return _ptr == other._ptr; }

	inline bool operator == (const T * other) const { return _ptr == other; }

	inline bool operator == (typename std::remove_const<T>::type * other) const { return _ptr == other; }

	inline bool operator == (const std::nullptr_t other) const { return _ptr == other; }


	inline bool operator != (const RefPtr<T> & other) const { return _ptr != other._ptr; }

	inline bool operator != (const T * other) const { return _ptr != other; }

	inline bool operator != (typename std::remove_const<T>::type * other) const { return _ptr != other; }

	inline bool operator != (const std::nullptr_t other) const { return _ptr != other; }


	inline bool operator > (const RefPtr<T> & other) const { return _ptr > other._ptr; }

	inline bool operator > (const T * other) const { return _ptr > other; }

	inline bool operator > (typename std::remove_const<T>::type * other) const { return _ptr > other; }

	inline bool operator > (const std::nullptr_t other) const { return _ptr > other; }


	inline bool operator < (const RefPtr<T> & other) const { return _ptr < other._ptr; }

	inline bool operator < (const T * other) const { return _ptr < other; }

	inline bool operator < (typename std::remove_const<T>::type * other) const { return _ptr < other; }

	inline bool operator < (const std::nullptr_t other) const { return _ptr < other; }


	inline bool operator >= (const RefPtr<T> & other) const { return _ptr >= other._ptr; }

	inline bool operator >= (const T * other) const { return _ptr >= other; }

	inline bool operator >= (typename std::remove_const<T>::type * other) const { return _ptr >= other; }

	inline bool operator >= (const std::nullptr_t other) const { return _ptr >= other; }


	inline bool operator <= (const RefPtr<T> & other) const { return _ptr <= other._ptr; }

	inline bool operator <= (const T * other) const { return _ptr <= other; }

	inline bool operator <= (typename std::remove_const<T>::type * other) const { return _ptr <= other; }

	inline bool operator <= (const std::nullptr_t other) const { return _ptr <= other; }


	inline operator bool() const { return _ptr != nullptr; }

	inline void reset()
	{
		HMS_REF_PTR_SAFE_RELEASE_NULL(_ptr);
	}

	inline void swap(RefPtr<T> & other)
	{
		if (&other != this)
		{
			Ref * tmp = _ptr;
			_ptr = other._ptr;
			other._ptr = tmp;
		}
	}

	/**
	* This function assigns to this RefPtr<T> but does not increase the reference count of the object pointed to.
	* Useful for assigning an object created through the 'new' operator to a RefPtr<T>. Basically used in scenarios
	* where the RefPtr<T> has the initial ownership of the object.
	*
	* E.G:
	*      RefPtr<HmsAviPf::Image> image;
	*      image.weakAssign(new HmsAviPf::Image());
	*
	* Instead of:
	*      RefPtr<HmsAviPf::Image> image;
	*      image = new HmsAviPf::Image();
	*      image->release();               // Required because new'd object already has a reference count of '1'.
	*/
	inline void weakAssign(const RefPtr<T> & other)
	{
		HMS_REF_PTR_SAFE_RELEASE(_ptr);
		_ptr = other._ptr;
	}

private:
	Ref * _ptr;
};

/**
* Cast between RefPtr types statically.
*/
template<class T, class U> RefPtr<T> static_pointer_cast(const RefPtr<U> & r)
{
	return RefPtr<T>(static_cast<T*>(r.get()));
}

/**
* Cast between RefPtr types dynamically.
*/
template<class T, class U> RefPtr<T> dynamic_pointer_cast(const RefPtr<U> & r)
{
	return RefPtr<T>(dynamic_cast<T*>(r.get()));
}

/**
* Done with these macros.
*/
#undef HMS_REF_PTR_SAFE_RETAIN
#undef HMS_REF_PTR_SAFE_RELEASE
#undef HMS_REF_PTR_SAFE_RELEASE_NULL

NS_HMS_END

#endif  // __HMS_REF_PTR_H__

