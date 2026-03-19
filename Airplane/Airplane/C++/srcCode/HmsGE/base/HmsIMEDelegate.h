
#ifndef __HMS_IME_DELEGATE_H__
#define __HMS_IME_DELEGATE_H__

#include <string>
#include "math/HmsGeometry.h"
#include "base/HmsEventKeyboard.h"


NS_HMS_BEGIN

/**
 * A static global empty std::string install.
 */
extern const std::string HMS_DLL STD_STRING_EMPTY;


/**
 * Keyboard notification event type.
 */
typedef struct
{
    Rect        begin;              // the soft keyboard rectangle when animation begins
    Rect        end;                // the soft keyboard rectangle when animation ends
    float       duration;           // the soft keyboard animation duration
} IMEKeyboardNotificationInfo;

/**
 *Input method editor delegate.
 */
class HMS_DLL IMEDelegate
{
public:
    /**
     * Default constructor.
     */
    virtual ~IMEDelegate();
    
    /**
     * Default destructor.
     */
    virtual bool attachWithIME();
    
    /**
     * Determine whether the IME is detached or not.
     */
    virtual bool detachWithIME();

protected:
    friend class IMEDispatcher;

    /**
    Decide if the delegate instance is ready to receive an IME message.

    Called by IMEDispatcher.
    */
    virtual bool canAttachWithIME() { return false; }
    /**
    When the delegate detaches from the IME, this method is called by IMEDispatcher.
    */
    virtual void didAttachWithIME() {}

    /**
    Decide if the delegate instance can stop receiving IME messages.
    */
    virtual bool canDetachWithIME() { return false; }

    /**
    When the delegate detaches from the IME, this method is called by IMEDispatcher.
    */
    virtual void didDetachWithIME() {}

    /**
    Called by IMEDispatcher when text input received from the IME.
    */
    virtual void insertText(const char* /*text*/, size_t /*len*/) {}

    /**
    Called by IMEDispatcher after the user clicks the backward key.
    */
    virtual void deleteBackward() {}

    /**
    Called by IMEDispatcher after the user press control key.
    */
    virtual void controlKey(EventKeyboard::KeyCode /*keyCode*/) {}

    /**
    Called by IMEDispatcher for text stored in delegate.
    */
    virtual const std::string& getContentText() { return STD_STRING_EMPTY; }

    //////////////////////////////////////////////////////////////////////////
    // keyboard show/hide notification
    //////////////////////////////////////////////////////////////////////////
    virtual void keyboardWillShow(IMEKeyboardNotificationInfo& /*info*/)   {}
    virtual void keyboardDidShow(IMEKeyboardNotificationInfo& /*info*/)    {}
    virtual void keyboardWillHide(IMEKeyboardNotificationInfo& /*info*/)   {}
    virtual void keyboardDidHide(IMEKeyboardNotificationInfo& /*info*/)    {}

protected:
    IMEDelegate();
};


NS_HMS_END
// end of base group
/// @}

#endif    // __HMS_IME_DELEGATE_H__
