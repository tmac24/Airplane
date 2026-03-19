#ifndef _HMS_EDITBOX_DELEGATE_H__
#define _HMS_EDITBOX_DELEGATE_H__

#include <functional>

NS_HMS_BEGIN

/**
* The popup keyboard return type.
*/
enum class KeyboardReturnType
{
    DEFAULT,
    DONE,
    SEND,
    SEARCH,
    GO,
    NEXT
};

/**
* @brief The EditBox::InputMode defines the type of text that the user is allowed
* to enter.
*/
enum class EditBoxInputMode
{
    /**
    * The user is allowed to enter any text, including line breaks.
    */
    ANY,

    /**
    * The user is allowed to enter an e-mail address.
    */
    EMAIL_ADDRESS,

    /**
    * The user is allowed to enter an integer value.
    */
    NUMERIC,

    /**
    * The user is allowed to enter a phone number.
    */
    PHONE_NUMBER,

    /**
    * The user is allowed to enter a URL.
    */
    URL,

    /**
    * The user is allowed to enter a real number value.
    * This extends kEditBoxInputModeNumeric by allowing a decimal point.
    */
    DECIMAL,

    /**
    * The user is allowed to enter any text, except for line breaks.
    */
    SINGLE_LINE,
};

/**
* @brief The EditBox::InputFlag defines how the input text is displayed/formatted.
*/
enum class EditBoxInputFlag
{
    /**
    * Indicates that the text entered is confidential data that should be
    * obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
    */
    PASSWORD,

    /**
    * Indicates that the text entered is sensitive data that the
    * implementation must never store into a dictionary or table for use
    * in predictive, auto-completing, or other accelerated input schemes.
    * A credit card number is an example of sensitive data.
    */
    SENSITIVE,

    /**
    * This flag is a hint to the implementation that during text editing,
    * the initial letter of each word should be capitalized.
    */
    INITIAL_CAPS_WORD,

    /**
    * This flag is a hint to the implementation that during text editing,
    * the initial letter of each sentence should be capitalized.
    */
    INITIAL_CAPS_SENTENCE,

    /**
    * Capitalize all characters automatically.
    */
    INITIAL_CAPS_ALL_CHARACTERS,

    /**
    * Lowercase all characters automatically.
    */
    LOWERCASE_ALL_CHARACTERS
};


class CHmsUiSystemEditBox;

typedef std::function<void(CHmsUiSystemEditBox * editbox, const std::string &)> OnEditBoxCallback;

class HMS_DLL EditBoxDelegate
{
public:

    /**
    * Reason for ending edit (for platforms where it is known)
    */
    enum class EditBoxEndAction {
        UNKNOWN,
        TAB_TO_NEXT,
        TAB_TO_PREVIOUS,
        RETURN
    };

    virtual ~EditBoxDelegate() {};

    /**
    * This method is called when an edit box gains focus after keyboard is shown.
    * @param editBox The edit box object that generated the event.
    */
    virtual void editBoxEditingDidBegin(CHmsUiSystemEditBox* editBox) {};


    /**
    * This method is called when an edit box loses focus after keyboard is hidden.
    * @param editBox The edit box object that generated the event.
    * @deprecated Use editBoxEditingDidEndWithAction() instead to receive reason for end
    */
    virtual void editBoxEditingDidEnd(CHmsUiSystemEditBox* editBox) {};

    /**
    * This method is called when the edit box text was changed.
    * @param editBox The edit box object that generated the event.
    * @param text The new text.
    */
    virtual void editBoxTextChanged(CHmsUiSystemEditBox* editBox, const std::string& text) {};

    /**
    * This method is called when the return button was pressed or the outside area of keyboard was touched.
    * @param editBox The edit box object that generated the event.
    */
    virtual void editBoxReturn(CHmsUiSystemEditBox* editBox) = 0;

    /**
    * This method is called when an edit box loses focus after keyboard is hidden.
    * @param editBox The edit box object that generated the event.
    * @param type The reason why editing ended.
    */
    virtual void editBoxEditingDidEndWithAction(CHmsUiSystemEditBox * editBox, EditBoxEndAction action) {};
};


NS_HMS_END

#endif // _HMS_EDITBOX_DELEGATE_H__