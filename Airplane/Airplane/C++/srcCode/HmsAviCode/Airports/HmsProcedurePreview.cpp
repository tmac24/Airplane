#include "HmsProcedurePreview.h"
#include "HmsNavImageSql.h"
#include "base/HmsImage.h"
#include "render/HmsTexture2D.h"
#include "HmsNavMgr.h"
#include "HmsLayerAirportProgram.h"
#include "ToolBar/HmsAvionicsDisplayMgr.h"

CHmsProcedurePreview::CHmsProcedurePreview()
: HmsUiEventInterface(this)
, c_scale(0.23f)
, c_margin(20.0f)
, m_curIndex(0)
, m_curRow(-1)
{

}

CHmsProcedurePreview::~CHmsProcedurePreview()
{

}

bool CHmsProcedurePreview::Init(const HmsAviPf::Size & size)
{
    if (!CHmsNode::Init())
    {
        return false;
    }
    this->SetContentSize(size);

    m_bgDrawNode = CHmsDrawNode::Create();
	if (m_bgDrawNode.get())
	{
		m_bgDrawNode->DrawSolidRect(Rect(Vec2(0, 0), size), Color4F(1.0f, 1.0f, 1.0f, 0.3f));
		this->AddChild(m_bgDrawNode);
	}    

    m_scratchPadClipping = CHmsScratchPadClipping::Create();
	if (m_scratchPadClipping.get())
	{
		this->AddChild(m_scratchPadClipping);
		auto nodeMask = CHmsDrawNode::Create();
		nodeMask->DrawSolidRect(Rect(Vec2(), size));
		m_scratchPadClipping->setStencil(nodeMask);
		m_scratchPadClipping->SetContentSize(size);
		m_scratchPadClipping->SetEventMode(ClippingEventMode::E_DRAG_CLICK);

		m_previewPanel = CHmsUiPanel::Create();
		m_previewPanel->SetAnchorPoint(Vec2(0, 0));
		m_scratchPadClipping->AddChildWithEvent(m_previewPanel, true);
	}    

    this->RegisterAllNodeChild();

    CHmsNavImageSql::GetInstance().AddFavoriteModifyCallback(std::bind(&CHmsProcedurePreview::UpdateDataFromDb, this));

    return true;
}

void CHmsProcedurePreview::OnButtonClick(CHmsUiButtonAbstract *pBtn, int index)
{
    this->RemoveAllChildInterface();

    auto pProgram = CHmsLayerAirportProgram::GetInstance();
	if (pProgram)
	{
		this->AddChild(pProgram);
		this->AddChildInterface(pProgram);
		pProgram->SetOnCloseCallback([=](){

			this->RegisterAllNodeChild();
		});

		pProgram->SetStarSidStu(m_vecStarSidStu[index]);
	}
    
}

void CHmsProcedurePreview::UpdateDataFromDb()
{
    m_vecStarSidStu = CHmsNavImageSql::GetInstance().GetFavoriteStarSidInfo();

    m_previewPanel->RemoveAllChildInterface();
    m_previewPanel->RemoveAllChildren();

    m_curIndex = 0;
    m_curRow = -1;
}

void CHmsProcedurePreview::OnEnter()
{
    CHmsNode::OnEnter();
    SetScheduleUpdate();

    if (m_vecStarSidStu.empty())
    {
        UpdateDataFromDb();
    }
}

void CHmsProcedurePreview::OnExit()
{
    CHmsNode::OnExit();
    SetUnSchedule();
}

void CHmsProcedurePreview::Update(float delta)
{
    if (m_curIndex < (int)m_vecStarSidStu.size())
    {
        UpdateContent();
    } 
}

void CHmsProcedurePreview::UpdateContent()
{ 
    Size panelSize = this->GetContentSize();
    panelSize.height = 0;

    auto width = 2500 * c_scale;
    auto height = 3500 * c_scale;
    int columnCount = panelSize.width / (width + c_margin);
	for (int i = 0; i < (int)m_vecStarSidStu.size(); ++i)
    {
        int column = i % columnCount;
        if (0 == column)
        {
            panelSize.height += height + c_margin;
        }
    }

	if (m_curIndex < (int)m_vecStarSidStu.size())
    {
        auto &stu = m_vecStarSidStu.at(m_curIndex);

        auto pImage = new Image;
        pImage->initWithImageFile(stu.imagePath.c_str());
        //pImage->GenerateMipmaps();
        auto pTexture = new Texture2D;
        pTexture->initWithImage(pImage);

        auto drawPanel = CHmsUiImageButton::CreateWithMipMapTexture(pTexture);
        m_previewPanel->AddChild(drawPanel);
        drawPanel->SetAnchorPoint(Vec2(0, 0));        
        drawPanel->SetVisible(true); 
        drawPanel->SetOnClickedFunc(std::bind(&CHmsProcedurePreview::OnButtonClick, this, std::placeholders::_1, m_curIndex));

        float scale = c_scale;
        while (pImage->getWidth() * scale > width)
        {
            scale -= 0.01f;
        }
        drawPanel->SetScale(scale);

        auto pLabel = CHmsGlobal::CreateUTF8Label(stu.icaoCode.c_str(), 32, Color4F(Color3B(0, 71, 157)), true);
		if (pLabel)
		{
			pLabel->SetAnchorPoint(Vec2(0.5, 0));
			pLabel->SetPosition(Vec2(pImage->getWidth() * 0.5, 0));
			pLabel->SetScale(1.0f / scale);
			drawPanel->AddChild(pLabel);
		}
       

        int column = m_curIndex % columnCount;
        if (0 == column)
        {
            ++m_curRow;
        }
        drawPanel->SetPosition(Vec2(0, 0));
        drawPanel->SetPosition(column*(width + c_margin) + c_margin * 2, panelSize.height - (height + c_margin)*(m_curRow + 1));

        auto intf = dynamic_cast<HmsUiEventInterface*>(drawPanel);
        if (intf)
        {
            m_previewPanel->AddChildInterface(intf);
        }

        HMS_SAFE_RELEASE(pImage);
        HMS_SAFE_RELEASE(pTexture);

        ++m_curIndex;
    }
    m_previewPanel->SetContentSize(panelSize);
}

void CHmsProcedurePreview::LayoutBySize(Size s)
{
    this->SetContentSize(s);

    m_bgDrawNode->clear();
    m_bgDrawNode->DrawSolidRect(Rect(Vec2(0, 0), s), Color4F(1.0f, 1.0f, 1.0f, 0.3f));

    auto nodeMask = CHmsDrawNode::Create();
	if (nodeMask)
	{
		nodeMask->DrawSolidRect(Rect(Vec2(), s));
		m_scratchPadClipping->setStencil(nodeMask);
	}    
    m_scratchPadClipping->SetContentSize(s);
}