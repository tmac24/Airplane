#include "HmsUiSystemEditBox.h"
#include "HmsAviDisplayUnit.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "Language/HmsLanguageLabel.h"
#include "HmsPlatform/platform.h"
#include "HmsGlobal.h"
#include "HmsPlatform/HmsPlatformConfig.h"

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
#include "ui/SystemEditBox/UIEditBoxImpl-android.h"
#endif

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_LINUX
#include "ui/SystemEditBox/UIEditBoxImpl-common.h"
#endif

NS_HMS_BEGIN

CHmsUiSystemEditBox * HmsAviPf::CHmsUiSystemEditBox::Create(const char * strTextUtf8, float fSize, const Size & size, const std::string & strBackground)
{
    CHmsUiSystemEditBox *ret = new CHmsUiSystemEditBox();
    if (ret && ret->Init(strTextUtf8, fSize,size,strBackground))
    {
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

void CHmsUiSystemEditBox::CloseLastEditBoxKeyboard()
{
#if HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID
    EditBoxImplAndroid::nativeCloseLastKeyboard();
#endif
}

CHmsUiSystemEditBox::CHmsUiSystemEditBox()
    : CHmsUiButtonAbstract(this)
    , m_delegate(nullptr)
    , m_editBoxImpl(nullptr)
{

}

CHmsUiSystemEditBox::~CHmsUiSystemEditBox()
{
}

void CHmsUiSystemEditBox::Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags)
{
    CHmsNode::Draw(renderer, transform, flags);
    if (m_editBoxImpl)
    {
        m_editBoxImpl->draw(renderer, transform, flags);
    }
}

void CHmsUiSystemEditBox::SetBgColor(const HmsAviPf::Color4B & color)
{
    if (m_bg)
    {
        m_bg->SetColor(Color3B(color));
    }
}

std::string HmsAviPf::CHmsUiSystemEditBox::GetText()
{
    std::string strOut;
    if (m_editBoxImpl)
    {
        strOut = m_editBoxImpl->getText();
    }
    return strOut;
}

bool CHmsUiSystemEditBox::Init(const char * strTextUtf8, float fSize, const Size & size, const std::string & strBackground)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    SetContentSize(size);

    m_bg = CHmsStretchImageNode::Create(strBackground, size);
    if (m_bg)
    {
        m_bg->SetAnchorPoint(Vec2(0, 0));
        m_bg->SetPosition(0, 0);
        this->AddChild(m_bg);
    }
    
#if defined(__APPLE__)
    return true; // 跳过 EditBox
#endif

//    m_editBoxImpl = __createSystemEditBox(this);
//    m_editBoxImpl->initWithSize(size, (int)fSize);
//    m_editBoxImpl->setInputMode(EditBoxInputMode::SINGLE_LINE);
//    m_editBoxImpl->setMaxLength(40);
//    m_editBoxImpl->setText(strTextUtf8);

    return true;
}

Rect CHmsUiSystemEditBox::GetScreenRect()
{
    auto sizeScreen = CHmsAviDisplayUnit::GetInstance()->GetViewSize();

    auto sizeContent = GetContentSize();
    auto leftBottom = ConvertToWorldSpace(Point::ZERO);
    auto rightTop = ConvertToWorldSpace(Point(sizeContent.width, sizeContent.height));

    auto uiLeft = leftBottom.x;
    auto uiTop = (sizeScreen.height - rightTop.y);
    auto uiWidth = (rightTop.x - leftBottom.x);
    auto uiHeight = (rightTop.y - leftBottom.y);

    return Rect(uiLeft, uiTop, uiWidth, uiHeight);
}


void HmsAviPf::CHmsUiSystemEditBox::SetTextHorizontalAlignment(TextHAlignment alignment)
{
    if (m_editBoxImpl)
    {
        m_editBoxImpl->setTextHorizontalAlignment(alignment);
    }
}

EditBoxDelegate * CHmsUiSystemEditBox::GetDelegate()
{
    return m_delegate;
}

void HmsAviPf::CHmsUiSystemEditBox::SetMaxCharCount(int count)
{
    if (m_editBoxImpl)
    {
        m_editBoxImpl->setMaxLength(count);
    }
}

void CHmsUiSystemEditBox::SetDelegate(EditBoxDelegate* delegate)
{
    m_delegate = delegate;
    if (m_editBoxImpl != nullptr)
    {
        m_editBoxImpl->setDelegate(m_delegate);
    }
}


void HmsAviPf::CHmsUiSystemEditBox::SetEndEditCallback(const OnEditBoxCallback & callback)
{
    if (m_editBoxImpl)
    {
        m_editBoxImpl->_callbackEndEdit = callback;
    }
}

void HmsAviPf::CHmsUiSystemEditBox::SetChangedCallback(const OnEditBoxCallback & callback)
{
    if (m_editBoxImpl)
    {
        m_editBoxImpl->_callbackChanged = callback;
    }
}

bool CHmsUiSystemEditBox::OnPressed(const Vec2 & posOrigin)
{
    if (m_editBoxImpl)
    {
        m_editBoxImpl->openKeyboard();
    }
    return true;
}


void CHmsUiSystemEditBox::SetTextColor(const Color4B &color)
{
    if (m_editBoxImpl)
    {
        m_editBoxImpl->setFontColor(color);
    }
}

void CHmsUiSystemEditBox::SetText(const char * str)
{
    if (m_editBoxImpl)
    {
        m_editBoxImpl->setText(str);
    }
}

NS_HMS_END



