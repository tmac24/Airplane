#include "base/HmsIMEDispatcher.h"

#include <list>

NS_HMS_BEGIN

//////////////////////////////////////////////////////////////////////////
// add/remove delegate in IMEDelegate Cons/Destructor
//////////////////////////////////////////////////////////////////////////

IMEDelegate::IMEDelegate()
{
    IMEDispatcher::sharedDispatcher()->addDelegate(this);
}

IMEDelegate::~IMEDelegate()
{
    IMEDispatcher::sharedDispatcher()->removeDelegate(this);
}

bool IMEDelegate::attachWithIME()
{
    return IMEDispatcher::sharedDispatcher()->attachDelegateWithIME(this);
}

bool IMEDelegate::detachWithIME()
{
    return IMEDispatcher::sharedDispatcher()->detachDelegateWithIME(this);
}

//////////////////////////////////////////////////////////////////////////

typedef std::list< IMEDelegate * > DelegateList;
typedef std::list< IMEDelegate * >::iterator  DelegateIter;

//////////////////////////////////////////////////////////////////////////
// Delegate List manage class
//////////////////////////////////////////////////////////////////////////

class IMEDispatcher::Impl
{
public:
    Impl()
    {
    }

    ~Impl()
    {

    }

    void init()
    {
        _delegateWithIme = 0;
    }

    DelegateIter findDelegate(IMEDelegate* delegate)
    {
        DelegateIter end = _delegateList.end();
        for (DelegateIter iter = _delegateList.begin(); iter != end; ++iter)
        {
            if (delegate == *iter)
            {
                return iter;
            }
        }
        return end;
    }

    DelegateList    _delegateList;
    IMEDelegate*  _delegateWithIme;
};

//////////////////////////////////////////////////////////////////////////
// Cons/Destructor
//////////////////////////////////////////////////////////////////////////

IMEDispatcher::IMEDispatcher()
: _impl(new IMEDispatcher::Impl)
{
    _impl->init();
}

IMEDispatcher::~IMEDispatcher()
{
    HMS_SAFE_DELETE(_impl);
}

//////////////////////////////////////////////////////////////////////////
// Add/Attach/Remove IMEDelegate
//////////////////////////////////////////////////////////////////////////

void IMEDispatcher::addDelegate(IMEDelegate* delegate)
{
    if (! delegate || ! _impl)
    {
        return;
    }
    if (_impl->_delegateList.end() != _impl->findDelegate(delegate))
    {
        // pDelegate already in list
        return;
    }
    _impl->_delegateList.push_front(delegate);
}

bool IMEDispatcher::attachDelegateWithIME(IMEDelegate * delegate)
{
    bool ret = false;
    do
    {
        HMS_BREAK_IF(! _impl || ! delegate);

        DelegateIter end  = _impl->_delegateList.end();
        DelegateIter iter = _impl->findDelegate(delegate);

        // if pDelegate is not in delegate list, return
        HMS_BREAK_IF(end == iter);

        if (_impl->_delegateWithIme)
        {
            if (_impl->_delegateWithIme != delegate)
            {
                // if old delegate canDetachWithIME return false 
                // or pDelegate canAttachWithIME return false,
                // do nothing.
                HMS_BREAK_IF(!_impl->_delegateWithIme->canDetachWithIME()
                    || !delegate->canAttachWithIME());

                // detach first
                IMEDelegate * oldDelegate = _impl->_delegateWithIme;
                _impl->_delegateWithIme = 0;
                oldDelegate->didDetachWithIME();

                _impl->_delegateWithIme = *iter;
                delegate->didAttachWithIME();
            }
            ret = true;
            break;
        }

        // delegate hasn't attached to IME yet
        HMS_BREAK_IF(! delegate->canAttachWithIME());

        _impl->_delegateWithIme = *iter;
        delegate->didAttachWithIME();
        ret = true;
    } while (0);
    return ret;
}

bool IMEDispatcher::detachDelegateWithIME(IMEDelegate * delegate)
{
    bool ret = false;
    do
    {
        HMS_BREAK_IF(! _impl || ! delegate);

        // if pDelegate is not the current delegate attached to IME, return
        HMS_BREAK_IF(_impl->_delegateWithIme != delegate);

        HMS_BREAK_IF(! delegate->canDetachWithIME());

        _impl->_delegateWithIme = 0;
        delegate->didDetachWithIME();
        ret = true;
    } while (0);
    return ret;
}

void IMEDispatcher::removeDelegate(IMEDelegate* delegate)
{
    do 
    {
        HMS_BREAK_IF(! delegate || ! _impl);

        DelegateIter iter = _impl->findDelegate(delegate);
        DelegateIter end  = _impl->_delegateList.end();
        HMS_BREAK_IF(end == iter);

        if (_impl->_delegateWithIme)

        if (*iter == _impl->_delegateWithIme)
        {
            _impl->_delegateWithIme = 0;
        }
        _impl->_delegateList.erase(iter);
    } while (0);
}

//////////////////////////////////////////////////////////////////////////
// dispatch text message
//////////////////////////////////////////////////////////////////////////

void IMEDispatcher::dispatchInsertText(const char * text, size_t len)
{
    do 
    {
        HMS_BREAK_IF(! _impl || ! text || len <= 0);

        // there is no delegate attached to IME
        HMS_BREAK_IF(! _impl->_delegateWithIme);

        _impl->_delegateWithIme->insertText(text, len);
    } while (0);
}

void IMEDispatcher::dispatchDeleteBackward()
{
    do 
    {
        HMS_BREAK_IF(! _impl);

        // there is no delegate attached to IME
        HMS_BREAK_IF(! _impl->_delegateWithIme);

        _impl->_delegateWithIme->deleteBackward();
    } while (0);
}

void IMEDispatcher::dispatchControlKey(EventKeyboard::KeyCode keyCode)
{
    do
    {
        HMS_BREAK_IF(!_impl);

        // there is no delegate attached to IME
        HMS_BREAK_IF(!_impl->_delegateWithIme);

        _impl->_delegateWithIme->controlKey(keyCode);
    } while (0);
}

const std::string& IMEDispatcher::getContentText()
{
    if (_impl && _impl->_delegateWithIme)
    {
        return _impl->_delegateWithIme->getContentText();
    }
    return STD_STRING_EMPTY;
}

//////////////////////////////////////////////////////////////////////////
// dispatch keyboard message
//////////////////////////////////////////////////////////////////////////

void IMEDispatcher::dispatchKeyboardWillShow(IMEKeyboardNotificationInfo& info)
{
    if (_impl)
    {
        IMEDelegate * delegate = nullptr;
        DelegateIter last = _impl->_delegateList.end();
        for (DelegateIter first = _impl->_delegateList.begin(); first != last; ++first)
        {
            delegate = *(first);
            if (delegate)
            {
                delegate->keyboardWillShow(info);
            }
        }
    }
}

void IMEDispatcher::dispatchKeyboardDidShow(IMEKeyboardNotificationInfo& info)
{
    if (_impl)
    {
        IMEDelegate * delegate = nullptr;
        DelegateIter last = _impl->_delegateList.end();
        for (DelegateIter first = _impl->_delegateList.begin(); first != last; ++first)
        {
            delegate = *(first);
            if (delegate)
            {
                delegate->keyboardDidShow(info);
            }
        }
    }
}

void IMEDispatcher::dispatchKeyboardWillHide(IMEKeyboardNotificationInfo& info)
{
    if (_impl)
    {
        IMEDelegate * delegate = nullptr;
        DelegateIter last = _impl->_delegateList.end();
        for (DelegateIter first = _impl->_delegateList.begin(); first != last; ++first)
        {
            delegate = *(first);
            if (delegate)
            {
                delegate->keyboardWillHide(info);
            }
        }
    }
}

void IMEDispatcher::dispatchKeyboardDidHide(IMEKeyboardNotificationInfo& info)
{
    if (_impl)
    {
        IMEDelegate * delegate = nullptr;
        DelegateIter last = _impl->_delegateList.end();
        for (DelegateIter first = _impl->_delegateList.begin(); first != last; ++first)
        {
            delegate = *(first);
            if (delegate)
            {
                delegate->keyboardDidHide(info);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// protected member function
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////

IMEDispatcher* IMEDispatcher::sharedDispatcher()
{
    static IMEDispatcher s_instance;
    return &s_instance;
}

NS_HMS_END
