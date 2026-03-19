#ifndef __UIEditBoxIMPLWIN_H__
#define __UIEditBoxIMPLWIN_H__

#include "HmsPlatformConfig.h"
#include "ui/HmsUiSystemEditBox.h"

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
#include "ui/SystemEditBox/UIEditBoxImpl-common.h"

NS_HMS_BEGIN

class HMS_DLL EditBoxImplWin : public EditBoxImplCommon
{
public:

    EditBoxImplWin(CHmsUiSystemEditBox* pEditText);
    virtual ~EditBoxImplWin();

    virtual bool isEditing() override;
    virtual void createNativeControl(const Rect& frame) override;
    virtual void setNativeFont(const char* pFontName, int fontSize) override;
    virtual void setNativeFontColor(const Color4B& color) override;
    virtual void setNativeInputMode(EditBoxInputMode inputMode) override;
    virtual void setNativeInputFlag(EditBoxInputFlag inputFlag) override;
    virtual void setNativeReturnType(KeyboardReturnType returnType)override;
    virtual void setNativeTextHorizontalAlignment(HmsAviPf::TextHAlignment alignment) {}
    virtual void setNativeText(const char* pText) override;
    virtual void setNativeVisible(bool visible) override;
    virtual void updateNativeFrame(const Rect& rect) override;
    virtual const char* getNativeDefaultFontName() override;
    virtual void nativeOpenKeyboard() override;
    virtual void nativeCloseKeyboard() override;
    virtual void setNativeMaxLength(int maxLength);

private:
    void createSingleLineEditCtrl();
    void createMultilineEditCtrl();
    void cleanupEditCtrl();
    std::string getText() const;
    void  _WindowProc(HWND, UINT, WPARAM, LPARAM);

    WNDPROC _prevWndProc;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK hookGLFWWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND hwndEdit;
    //FIXME: fontSize should be in parent class
    int _fontSize;
    bool _changedTextManually;
    bool _hasFocus;
    static WNDPROC s_prevCocosWndProc;

    static HINSTANCE s_hInstance;
    static HWND s_hwndCocos;
    static HWND s_previousFocusWnd;
    static bool s_isInitialized;
    static int s_editboxChildID;
    static void lazyInit();
};

NS_HMS_END

#endif /* (HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32) */

#endif /* __UIEditBoxIMPLWIN_H__ */