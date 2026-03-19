#include "ui/SystemEditBox/UIEditBoxImpl-android.h"

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)

#include "ui/HmsUiSystemEditBox.h"
#include <jni.h>
#include "JniHelper.h"
#include "display/HmsLabel.h"
#include "base/HmsUTF8.h"
#include "math/Vec2.h"
#include "HmsAviDisplayUnit.h"
#include "base/HmsFileUtils.h"

NS_HMS_BEGIN

static const std::string editBoxClassName = "com/hermes/hmsflarm/HmsEditBoxHelper";

static std::unordered_map<int, EditBoxImplAndroid*> s_allEditBoxes;


EditBoxImpl* __createSystemEditBox(CHmsUiSystemEditBox* editBox)
{
    return new EditBoxImplAndroid(editBox);
}


EditBoxImplAndroid::EditBoxImplAndroid(CHmsUiSystemEditBox* pEditText)
: EditBoxImplCommon(pEditText)
, _editBoxIndex(-1)
{
}

EditBoxImplAndroid::~EditBoxImplAndroid()
{
    s_allEditBoxes.erase(_editBoxIndex);
    JniHelper::callStaticVoidMethod(editBoxClassName, "removeEditBox", _editBoxIndex);

}

void EditBoxImplAndroid::createNativeControl(const Rect& frame)
{
    auto sizeScreen = CHmsAviDisplayUnit::GetInstance()->GetViewSize();

    
    auto sizeContent = frame.size;
    auto leftBottom = _editBox->ConvertToWorldSpace(Point::ZERO);
    auto rightTop = _editBox->ConvertToWorldSpace(Point(sizeContent.width, sizeContent.height));

    auto uiLeft = leftBottom.x;
    auto uiTop = (sizeScreen.height - rightTop.y);
    auto uiWidth = (rightTop.x - leftBottom.x);
    auto uiHeight = (rightTop.y - leftBottom.y);

    _editBoxIndex = JniHelper::callStaticIntMethod(editBoxClassName, "createEditBox",
                                                   (int)uiLeft, (int)uiTop, (int)uiWidth, (int)uiHeight, 
                                                   (float)1.0f);
    s_allEditBoxes[_editBoxIndex] = this;
}

void EditBoxImplAndroid::setNativeFont(const char* pFontName, int fontSize)
{

    auto isFontFileExists = CHmsFileUtils::getInstance()->isFileExist(pFontName);
    std::string realFontPath = pFontName;
    if(isFontFileExists) {
        realFontPath = CHmsFileUtils::getInstance()->fullPathForFilename(pFontName);
        if (realFontPath.find("assets/") == 0)
        {
            realFontPath = realFontPath.substr(strlen("assets/"));   // Chop out the 'assets/' portion of the path.
        }
    }
    JniHelper::callStaticVoidMethod(editBoxClassName, "setFont",
                                    _editBoxIndex, realFontPath,
                                    (float)fontSize);
}

void EditBoxImplAndroid::setNativeFontColor(const Color4B& color)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setFontColor", _editBoxIndex, 
                                    (int)color.r, (int)color.g, (int)color.b, (int)color.a);
}

// void EditBoxImplAndroid::setNativePlaceholderFontColor(const Color4B& color)
// {
//     JniHelper::callStaticVoidMethod(editBoxClassName, "setPlaceHolderTextColor", _editBoxIndex, 
//                                     (int)color.r, (int)color.g, (int)color.b, (int)color.a);
// }

void EditBoxImplAndroid::setNativeInputMode(EditBoxInputMode inputMode)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setInputMode", 
                                    _editBoxIndex, static_cast<int>(inputMode));
}

void EditBoxImplAndroid::setNativeMaxLength(int maxLength)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setMaxLength", _editBoxIndex, maxLength);
}

void EditBoxImplAndroid::setNativeInputFlag(EditBoxInputFlag inputFlag)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setInputFlag", 
                                    _editBoxIndex, static_cast<int>(inputFlag));
}

void EditBoxImplAndroid::setNativeReturnType(KeyboardReturnType returnType)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setReturnType", 
                                    _editBoxIndex, static_cast<int>(returnType));
}

void EditBoxImplAndroid::setNativeTextHorizontalAlignment(TextHAlignment alignment)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setTextHorizontalAlignment", 
                                    _editBoxIndex, static_cast<int>(alignment));
}

bool EditBoxImplAndroid::isEditing()
{
    return false;
}

void EditBoxImplAndroid::setNativeText(const char* pText)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setText", _editBoxIndex, pText);
}

// void EditBoxImplAndroid::setNativePlaceHolder(const char* pText)
// {
//     JniHelper::callStaticVoidMethod(editBoxClassName, "setPlaceHolderText", _editBoxIndex, pText);
// }


void EditBoxImplAndroid::setNativeVisible(bool visible)
{ // don't need to be implemented on android platform.
    JniHelper::callStaticVoidMethod(editBoxClassName, "setVisible", _editBoxIndex, visible);
}

void EditBoxImplAndroid::updateNativeFrame(const Rect& rect)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setEditBoxViewRect", _editBoxIndex, 
                                    (int)rect.origin.x, (int)rect.origin.y, 
                                    (int)rect.size.width, (int)rect.size.height);
}

void EditBoxImplAndroid::nativeOpenKeyboard()
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "openKeyboard", _editBoxIndex);
}


void EditBoxImplAndroid::nativeCloseKeyboard()
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "closeKeyboard", _editBoxIndex);
}

void EditBoxImplAndroid::nativeCloseLastKeyboard()
{
    //JniHelper::callStaticVoidMethod(editBoxClassName, "CloseLastKeyboard");
    JniHelper::callStaticVoidMethod("com/hermes/hmsflarm/MainActivity", "SendCloseLastKeyboard");
}

void editBoxEditingDidBegin(int index)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingDidBegin();
    }
}
void editBoxEditingDidChanged(int index, const std::string& text)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingChanged(text);
    }
}

void editBoxEditingDidEnd(int index, const std::string& text, int action)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingDidEnd(text, static_cast<EditBoxDelegate::EditBoxEndAction>(action));
    }
}

const char* EditBoxImplAndroid::getNativeDefaultFontName()
{
    return "sans-serif";
}



NS_HMS_END

USING_NS_HMS;

void editBoxEditingDidBegin(int index)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingDidBegin();
    }
}

void editBoxEditingDidChanged(int index, const std::string& text)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingChanged(text);
    }
}

void editBoxEditingDidEnd(int index, const std::string& text, int action)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingDidEnd(text, static_cast<EditBoxDelegate::EditBoxEndAction>(action));
    }
}

#endif /* #if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID) */

