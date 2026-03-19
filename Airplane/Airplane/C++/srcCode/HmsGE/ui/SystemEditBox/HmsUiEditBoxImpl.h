#ifndef __UIEditBoxIMPL_H__
#define __UIEditBoxIMPL_H__

#include "HmsAviPf.h"
#include "ui/SystemEditBox/HmsEditBoxDelegate.h"

NS_HMS_BEGIN

class HMS_DLL EditBoxImpl
{
public:
    /**
    * @js NA
    */
    EditBoxImpl(CHmsUiSystemEditBox * pEditBox) : _delegate(nullptr), _editBox(pEditBox) {}
    /**
    * @js NA
    * @lua NA
    */
    virtual ~EditBoxImpl() {}

    virtual bool initWithSize(const Size& size, int nFontSize) = 0;
    virtual void setFont(const char* pFontName, int fontSize) = 0;
    virtual void setFontColor(const Color4B& color) = 0;
    virtual void setInputMode(EditBoxInputMode inputMode) = 0;
    virtual void setInputFlag(EditBoxInputFlag inputFlag) = 0;
    virtual void setMaxLength(int maxLength) = 0;
    virtual int  getMaxLength() = 0;
    virtual void setTextHorizontalAlignment(TextHAlignment alignment) = 0;
    virtual void setReturnType(KeyboardReturnType returnType) = 0;
    virtual bool isEditing() = 0;

    virtual void setText(const char* pText) = 0;
    virtual const char* getText(void) = 0;

    virtual const char* getFontName() = 0;
    virtual int getFontSize() = 0;
    virtual const Color4B& getFontColor() = 0;

    virtual EditBoxInputMode getInputMode() = 0;
    virtual EditBoxInputFlag getInputFlag() = 0;
    virtual KeyboardReturnType getReturnType() = 0;
    virtual TextHAlignment getTextHorizontalAlignment() = 0;

    virtual void doAnimationWhenKeyboardMove(float duration, float distance) = 0;

    virtual void openKeyboard() = 0;
    virtual void closeKeyboard() = 0;

    virtual void setPosition(const Vec2& pos) = 0;
    virtual void setVisible(bool visible) = 0;
    virtual void setContentSize(const Size& size) = 0;
    virtual void setAnchorPoint(const Vec2& anchorPoint) = 0;

    /**
    * check the editbox's position, update it when needed
    */
    virtual void updatePosition(float dt){};
    /**
    * @js NA
    * @lua NA
    */
    virtual void draw(HmsAviPf::Renderer *renderer, HmsAviPf::Mat4 const &transform, uint32_t flags) = 0;
    /**
    * @js NA
    * @lua NA
    */
    virtual void onEnter(void) = 0;


    void setDelegate(EditBoxDelegate* pDelegate) { _delegate = pDelegate; };
    EditBoxDelegate* getDelegate() { return _delegate; };
    CHmsUiSystemEditBox* getEditBox() { return _editBox; };

protected:
    EditBoxDelegate*        _delegate;
    CHmsUiSystemEditBox*    _editBox;
public:
    OnEditBoxCallback       _callbackChanged;
    OnEditBoxCallback       _callbackEndEdit;
};

// This method must be implemented at each subclass of EditBoxImpl.
extern EditBoxImpl* __createSystemEditBox(CHmsUiSystemEditBox* pEditBox);

NS_HMS_END

#endif /* __UIEditBoxIMPL_H__ */
