#pragma once
#include "display/HmsLabel.h"
#include "display/HmsStretchImageNode.h"
#include "ui/HmsUiButtonAbstract.h"
#include "base/HmsIMEDelegate.h"
#include "ui/SystemEditBox/HmsUiEditBoxImpl.h"
#include "ui/SystemEditBox/HmsEditBoxDelegate.h"

NS_HMS_BEGIN

class CHmsUiSystemEditBox : public CHmsNode, public CHmsUiButtonAbstract, public IMEDelegate
{
public:
	CHmsUiSystemEditBox();
	~CHmsUiSystemEditBox();

    static CHmsUiSystemEditBox * Create(const char * strTextUtf8, float fSize,  const Size & size, const std::string & strBackground);

    static void CloseLastEditBoxKeyboard();

    bool Init(const char * strTextUtf8, float fSize, const Size & size, const std::string & strBackground);


	void SetText(const char * str);

	void SetTextColor(const HmsAviPf::Color4B &color);
    void SetBgColor(const HmsAviPf::Color4B & color);

    virtual std::string GetText() override;

    void SetMaxCharCount(int count);

    /**
    * Gets/Sets the delegate for edit box.
    */
    void SetDelegate(EditBoxDelegate* delegate);
    EditBoxDelegate* GetDelegate();

    Rect GetScreenRect();

    void SetTextHorizontalAlignment(TextHAlignment alignment);

    void SetEndEditCallback(const OnEditBoxCallback & callback);
    void SetChangedCallback(const OnEditBoxCallback & callback);

public:
    virtual bool OnPressed(const Vec2 & posOrigin) override;

protected:
    EditBoxImpl*      m_editBoxImpl;
    EditBoxDelegate*  m_delegate;

private:
    RefPtr<CHmsStretchImageNode>    m_bg;


public:
    virtual void Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags) override;

};

NS_HMS_END