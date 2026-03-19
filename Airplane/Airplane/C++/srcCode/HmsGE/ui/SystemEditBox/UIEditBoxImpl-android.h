#ifndef __UIEDITBOXIMPLANDROID_H__
#define __UIEDITBOXIMPLANDROID_H__

#include "HmsPlatformConfig.h"
#include "ui/HmsUiSystemEditBox.h"

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)

#include "ui/SystemEditBox/UIEditBoxImpl-common.h"

NS_HMS_BEGIN

class CHmsLabel;
class EditBox;

class EditBoxImplAndroid : public EditBoxImplCommon
{
public:
    /**
     * @js NA
     */
    EditBoxImplAndroid(CHmsUiSystemEditBox* pEditText);
    /**
     * @js NA
     * @lua NA
     */
    virtual ~EditBoxImplAndroid();
    

    virtual bool isEditing() override;
    virtual void createNativeControl(const Rect& frame) override;
    virtual void setNativeFont(const char* pFontName, int fontSize) override;
    virtual void setNativeFontColor(const Color4B& color) override;
    virtual void setNativeInputMode(EditBoxInputMode inputMode) override;
    virtual void setNativeInputFlag(EditBoxInputFlag inputFlag) override;
    virtual void setNativeReturnType(KeyboardReturnType returnType)override;
    virtual void setNativeTextHorizontalAlignment(TextHAlignment alignment);
    virtual void setNativeText(const char* pText) override;
    virtual void setNativeVisible(bool visible) override;
    virtual void updateNativeFrame(const Rect& rect) override;
    virtual const char* getNativeDefaultFontName() override;
    virtual void nativeOpenKeyboard() override;
    virtual void nativeCloseKeyboard() override;
    virtual void setNativeMaxLength(int maxLength) override;

    static void nativeCloseLastKeyboard();
    
private:
    int _editBoxIndex;
};

NS_HMS_END

#endif /* #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) */

#endif /* __UIEDITBOXIMPLANDROID_H__ */

