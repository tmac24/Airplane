#ifndef __BASE_CCREF_H__
#define __BASE_CCREF_H__

#include "HmsAviMacros.h"
#include "HmsPlatformConfig.h"

#define HMS_REF_LEAK_DETECTION 0

NS_HMS_BEGIN


class Ref;

/**
*Interface that defines how to clone an Ref.
*by [3/24/2017 song.zhang079] 
*/

class HMS_DLL Clonable
{
public:
    /** Returns a copy of the Ref. */
    virtual Clonable* clone() const = 0;
    
    virtual ~Clonable() {};
};

/**
*Ref is used for reference count management. If a class inherits from Ref,
*then it is easy to be shared in different places.
*by [3/24/2017 song.zhang079] 
*/

class HMS_DLL Ref
{
public:
	/***
	*Retains the ownership.
	*This increases the Ref's reference count.
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
    void retain();

	/**
	* Releases the ownership immediately.
	*
	* This decrements the Ref's reference count.
	*
	* If the reference count reaches 0 after the decrement, this Ref is
	* destructed.
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
    void release();

	/**
	* Releases the ownership sometime soon automatically.
	*
	* This decrements the Ref's reference count at the end of current
	* autorelease pool block.
	*
	* If the reference count reaches 0 after the decrement, this Ref is
	* destructed.
	*
	* @see AutoreleasePool, retain, releaseA
	*returns The Ref itself.
	*by [3/24/2017 song.zhang079] 
	*/
    Ref* autorelease();

	/**
	*Returns the Ref's current reference count.
	*param   
	*returns The Ref's reference count.
	*by [3/24/2017 song.zhang079] 
	*/
    unsigned int getReferenceCount() const;

protected:
	/**
	*Constructor
	*The Ref's reference count is 1 after construction.
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
    Ref();

public:
	/**
	*Destructor
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
    virtual ~Ref();

protected:
    /// count of references
    unsigned int _referenceCount;

    friend class AutoreleasePool;
};

class CHmsNode;

typedef void (Ref::*SEL_CallFunc)();
typedef void (Ref::*SEL_CallFuncN)(CHmsNode*);
typedef void (Ref::*SEL_CallFuncND)(CHmsNode*, void*);
typedef void (Ref::*SEL_CallFuncO)(Ref*);
typedef void (Ref::*SEL_MenuHandler)(Ref*);
typedef void (Ref::*SEL_SCHEDULE)(float);

#define HMS_CALLFUNC_SELECTOR(_SELECTOR)		static_cast<HmsAviPf::SEL_CallFunc>(&_SELECTOR)
#define HMS_CALLFUNCN_SELECTOR(_SELECTOR)	static_cast<HmsAviPf::SEL_CallFuncN>(&_SELECTOR)
#define HMS_CALLFUNCND_SELECTOR(_SELECTOR)	static_cast<HmsAviPf::SEL_CallFuncND>(&_SELECTOR)
#define HMS_CALLFUNCO_SELECTOR(_SELECTOR)	static_cast<HmsAviPf::SEL_CallFuncO>(&_SELECTOR)
#define HMS_MENU_SELECTOR(_SELECTOR)			static_cast<HmsAviPf::SEL_MenuHandler>(&_SELECTOR)
#define HMS_SCHEDULE_SELECTOR(_SELECTOR)		static_cast<HmsAviPf::SEL_SCHEDULE>(&_SELECTOR)



NS_HMS_END

#endif // __BASE_CCREF_H__
