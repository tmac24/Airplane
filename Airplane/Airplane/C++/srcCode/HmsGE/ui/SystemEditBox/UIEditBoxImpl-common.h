
#ifndef __UIEditBoxIMPLICOMMON_H__
#define __UIEditBoxIMPLICOMMON_H__

#include "HmsPlatformConfig.h"
#include "ui/SystemEditBox/HmsUiEditBoxImpl.h"
#include "ui/HmsUiSystemEditBox.h"

NS_HMS_BEGIN

class CHmsUiSystemEditBox;

class HMS_DLL EditBoxImplCommon : public EditBoxImpl
{
public:
    /**
     * @js NA
     */
    EditBoxImplCommon(CHmsUiSystemEditBox* pEditText);
    /**
     * @js NA
     * @lua NA
     */
    virtual ~EditBoxImplCommon();
    
    virtual bool initWithSize(const Size& size, int nFontSize) override;
    
    virtual void setFont(const char* pFontName, int fontSize) override;
    virtual void setFontColor(const Color4B& color) override;
    virtual void setInputMode(EditBoxInputMode inputMode) override;
    virtual void setInputFlag(EditBoxInputFlag inputFlag) override;
    virtual void setReturnType(KeyboardReturnType returnType) override;
    virtual void setText(const char* pText) override;
    virtual void setVisible(bool visible) override;

    virtual void setMaxLength(int maxLength) override;
    virtual void setTextHorizontalAlignment(TextHAlignment alignment) override;

    virtual int  getMaxLength() override { return _maxLength; }
    virtual const char* getText(void) override { return _text.c_str(); }

    virtual const char* getFontName() override { return _fontName.c_str(); }
    virtual int getFontSize() override { return _fontSize; }
    virtual const Color4B& getFontColor() override { return _colText; }

    virtual EditBoxInputMode getInputMode() override { return _editBoxInputMode; }
    virtual EditBoxInputFlag getInputFlag() override { return _editBoxInputFlag; }
    virtual KeyboardReturnType getReturnType() override { return _keyboardReturnType; }
    virtual TextHAlignment getTextHorizontalAlignment() override { return _alignment; }

    virtual void refreshInactiveText();
    
    virtual void setContentSize(const Size& size) override;
    
    virtual void setAnchorPoint(const Vec2& anchorPoint) override {}
    virtual void setPosition(const Vec2& pos) override {}
    
    /**
     * @js NA
     * @lua NA
     */
    virtual void draw(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags) override;
    /**
     * @js NA
     * @lua NA
     */
    virtual void onEnter(void) override;
    virtual void openKeyboard() override;
    virtual void closeKeyboard() override;

    virtual void onEndEditing(const std::string& text);
    
    void editBoxEditingDidBegin();
    void editBoxEditingChanged(const std::string& text);
    void editBoxEditingDidEnd(const std::string& text, EditBoxDelegate::EditBoxEndAction action = EditBoxDelegate::EditBoxEndAction::UNKNOWN);

    virtual bool isEditing() override = 0;
    virtual void createNativeControl(const Rect& frame) = 0;
    virtual void setNativeFont(const char* pFontName, int fontSize) = 0;
    virtual void setNativeFontColor(const Color4B& color) = 0;
    virtual void setNativeInputMode(EditBoxInputMode inputMode) = 0;
    virtual void setNativeInputFlag(EditBoxInputFlag inputFlag) = 0;
    virtual void setNativeReturnType(KeyboardReturnType returnType) = 0;
    virtual void setNativeTextHorizontalAlignment(HmsAviPf::TextHAlignment alignment) = 0;
    virtual void setNativeText(const char* pText) = 0;
    virtual void setNativeVisible(bool visible) = 0;
    virtual void updateNativeFrame(const Rect& rect) = 0;
    virtual const char* getNativeDefaultFontName() = 0;
    virtual void nativeOpenKeyboard() = 0;
    virtual void nativeCloseKeyboard() = 0;
    virtual void setNativeMaxLength(int maxLength) {};

protected:
    void         initInactiveLabels(const Size& size, int nFontSize);
    void         setInactiveText(const char* pText);
    void         refreshLabelAlignment();
    void         placeInactiveLabels(const Size& size);
    virtual void doAnimationWhenKeyboardMove(float duration, float distance)override {};

    CHmsLabel* _label;
    EditBoxInputMode    _editBoxInputMode;
    EditBoxInputFlag    _editBoxInputFlag;
    KeyboardReturnType  _keyboardReturnType;
    HmsAviPf::TextHAlignment _alignment;

    std::string _text;

    std::string _fontName;

    int _fontSize;

    Color4B _colText;
    
    int   _maxLength;
    Size _contentSize;
    bool _editingMode;
};

NS_HMS_END


#endif /* __UIEditBoxIMPLICOMMON_H__ */

