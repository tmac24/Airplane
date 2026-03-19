#include "ui/SystemEditBox/UIEditBoxImpl-common.h"

#define kLabelZOrder  9999

#include "ui/HmsUiSystemEditBox.h"
#include "HmsAviDisplayUnit.h"
#include "display/HmsLabel.h"

static const int CC_EDIT_BOX_PADDING = 5;

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_WINRT || HMS_TARGET_PLATFORM == HMS_PLATFORM_WIN32)
#define PASSWORD_CHAR "*"
#else
#define PASSWORD_CHAR "\u25CF"
#endif

NS_HMS_BEGIN

EditBoxImplCommon::EditBoxImplCommon(CHmsUiSystemEditBox* pEditText)
: EditBoxImpl(pEditText)
, _label(nullptr)
, _editBoxInputMode(EditBoxInputMode::SINGLE_LINE)
, _editBoxInputFlag(EditBoxInputFlag::INITIAL_CAPS_ALL_CHARACTERS)
, _keyboardReturnType(KeyboardReturnType::DEFAULT)
, _fontSize(-1)
, _colText(Color3B::WHITE)
, _maxLength(-1)
, _alignment(TextHAlignment::LEFT)
, _editingMode(false)
{
}

EditBoxImplCommon::~EditBoxImplCommon()
{
}


bool EditBoxImplCommon::initWithSize(const Size& size, int nFontSize)
{
    do 
    {
        
        Rect rect = Rect(0, 0, size.width, size.height);
        
        this->createNativeControl(rect);
        
        initInactiveLabels(size, nFontSize);
        setContentSize(size);
        
        return true;
    }while (0);
    
    return false;
}

void EditBoxImplCommon::initInactiveLabels(const Size& size, int nFontSize)
{
    const char* pDefaultFontName = this->getNativeDefaultFontName();

    _label = CHmsLabel::createWithTTF("", "fonts/msyh.ttc", nFontSize);
    _label->SetAnchorPoint(Vec2(0,0.5f));
    _label->SetPosition(Vec2(0, size.height/2.0f));
    //_label->SetOverflow(Label::Overflow::CLAMP);
    _label->SetVisible(false);
    _editBox->AddChild(_label, kLabelZOrder);
    
    setFont(pDefaultFontName, nFontSize);
}

void EditBoxImplCommon::placeInactiveLabels(const Size& size)
{
    //_label->SetDimensions(size.width, size.height);
    
    if (_editBoxInputMode == EditBoxInputMode::ANY){
        _label->SetPosition(Vec2(CC_EDIT_BOX_PADDING, (size.height - CC_EDIT_BOX_PADDING)*0.5f));
        //_label->SetVerticalAlignment(TextVAlignment::TOP);
        //_label->enableWrap(true);
    }
    else {
        //_label->enableWrap(false);
        _label->SetPosition(Vec2(CC_EDIT_BOX_PADDING, size.height * 0.5f));
        //_label->SetVerticalAlignment(TextVAlignment::CENTER);
        
    }
}

void EditBoxImplCommon::setInactiveText(const char* pText)
{
    if (EditBoxInputFlag::PASSWORD == _editBoxInputFlag)
    {
        std::string passwordString;
        for(size_t i = 0; i < strlen(pText); ++i)
            passwordString.append(PASSWORD_CHAR);
        _label->SetString(passwordString);
    }
    else
    {
        _label->SetString(pText);
    }
    // Clip the text width to fit to the text box
    float fMaxWidth = _editBox->GetContentSize().width;
    float fMaxHeight = _editBox->GetContentSize().height;
    Size labelSize = _label->GetContentSize();
    if(labelSize.width > fMaxWidth || labelSize.height > fMaxHeight)
    {
        //_label->SetDimensions(fMaxWidth, fMaxHeight);
    }
}
    
void EditBoxImplCommon::setFont(const char* pFontName, int fontSize)
{
    _fontName = pFontName;
    _fontSize = fontSize;
    this->setNativeFont(pFontName, fontSize * _label->GetNodeToWorldAffineTransform().a);
    if (!_fontName.empty())
    {
        //_label->SetSystemFontName(pFontName);
    }
    if (fontSize > 0)
    {
        //_label->SetSystemFontSize(fontSize);
    }
}

void EditBoxImplCommon::setFontColor(const Color4B& color)
{
    _colText = color;
    this->setNativeFontColor(color);
    _label->SetTextColor(color);
}
   

void EditBoxImplCommon::setInputMode(EditBoxInputMode inputMode)
{
    _editBoxInputMode = inputMode;
    this->setNativeInputMode(inputMode);
    this->placeInactiveLabels(_editBox->GetContentSize());
}

void EditBoxImplCommon::setMaxLength(int maxLength)
{
    _maxLength = maxLength;
    this->setNativeMaxLength(maxLength);
}

void EditBoxImplCommon::setTextHorizontalAlignment(HmsAviPf::TextHAlignment alignment)
{
    _alignment = alignment;
    Vec2 pos(CC_EDIT_BOX_PADDING, _contentSize.height*0.5f);
    Vec2 ap(0, 0.5f);
    switch (_alignment)
    {
    case HmsAviPf::TextHAlignment::LEFT:
        pos.x = CC_EDIT_BOX_PADDING;
        ap.x = 0;
        break;
    case HmsAviPf::TextHAlignment::CENTER:
        pos.x = _contentSize.width * 0.5f;
        ap.x = 0.5f;
        break;
    case HmsAviPf::TextHAlignment::RIGHT:
        pos.x = _contentSize.width - CC_EDIT_BOX_PADDING;
        ap.x = 1.0f;
        break;
    default:
        break;
    }
    if (_label)
    {
        _label->SetPosition(pos);
        _label->SetAnchorPoint(ap);
    }
    this->setNativeTextHorizontalAlignment(alignment);
}

void EditBoxImplCommon::setInputFlag(EditBoxInputFlag inputFlag)
{
    _editBoxInputFlag = inputFlag;
    this->setNativeInputFlag(inputFlag);
}

void EditBoxImplCommon::setReturnType(KeyboardReturnType returnType)
{
    _keyboardReturnType = returnType;
    this->setNativeReturnType(returnType);
}
    
void EditBoxImplCommon::refreshInactiveText()
{
    setInactiveText(_text.c_str());

    refreshLabelAlignment();
    if (!_editingMode) {
        if (_text.size() == 0) {
            _label->SetVisible(false);
        } else {
            _label->SetVisible(true);
        }
    }
}

void EditBoxImplCommon::refreshLabelAlignment()
{
    //_label->SetHorizontalAlignment(_alignment);
    //_labelPlaceHolder->SetHorizontalAlignment(_alignment);
}

void EditBoxImplCommon::setText(const char* text)
{
    if (nullptr != text) {
        this->setNativeText(text);
        _text = text;
        refreshInactiveText();
    }
}

void EditBoxImplCommon::setVisible(bool visible)
{
    if(visible) {
        refreshInactiveText();
    } else {
        this->setNativeVisible(visible);
        _label->SetVisible(visible);
    }
}

void EditBoxImplCommon::setContentSize(const Size& size)
{
    _contentSize = size;
    CCLOG("[Edit text] content size = (%f, %f)", size.width, size.height);
    placeInactiveLabels(size);
}

void EditBoxImplCommon::draw(Renderer* /*renderer*/, const Mat4& /*transform*/, uint32_t flags)
{
    if(flags)
    {
        auto rect = _editBox->GetScreenRect();
        this->updateNativeFrame(rect);
    }
}

void EditBoxImplCommon::onEnter(void)
{
    const char* pText = getText();
    if (pText) {
        setInactiveText(pText);
    }
}

void EditBoxImplCommon::openKeyboard()
{
    _label->SetVisible(false);
    _editingMode = true;
    this->setNativeVisible(true);
    this->nativeOpenKeyboard();
}

void EditBoxImplCommon::closeKeyboard()
{
    this->nativeCloseKeyboard();
    _editingMode = false;
}

void EditBoxImplCommon::onEndEditing(const std::string& /*text*/)
{
    _editingMode = false;
    this->setNativeVisible(false);
    refreshInactiveText();
}
    
void EditBoxImplCommon::editBoxEditingDidBegin()
{
    // LOGD("textFieldShouldBeginEditing...");
    EditBoxDelegate *pDelegate = _editBox->GetDelegate();
    
    if (pDelegate != nullptr)
    {
        pDelegate->editBoxEditingDidBegin(_editBox);
    }
   
}

void EditBoxImplCommon::editBoxEditingDidEnd(const std::string& text, EditBoxDelegate::EditBoxEndAction action)
{
    // LOGD("textFieldShouldEndEditing...");
    _text = text;
    
    EditBoxDelegate *pDelegate = _editBox->GetDelegate();
    if (pDelegate != nullptr)
    {
        pDelegate->editBoxEditingDidEndWithAction(_editBox, action);
        pDelegate->editBoxEditingDidEnd(_editBox);   
        pDelegate->editBoxReturn(_editBox);
    }
    
    if (_editBox != nullptr)
    {
        this->onEndEditing(_text);

        if (_callbackEndEdit)
        {
            _callbackEndEdit(_editBox, _text);
        }
    }
}

void EditBoxImplCommon::editBoxEditingChanged(const std::string& text)
{
    // LOGD("editBoxTextChanged...");
    EditBoxDelegate *pDelegate = _editBox->GetDelegate();
    _text = text;
    if (pDelegate != nullptr)
    {
        pDelegate->editBoxTextChanged(_editBox, text);
    }
  
    if (_callbackChanged)
    {
        _callbackChanged(_editBox, _text);
    }
}

NS_HMS_END


