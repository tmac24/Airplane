#pragma once

#include "base/HmsNode.h"
#include "display/HmsDrawNode.h"
#include "ui/HmsUiEventInterface.h"
#include "ui/HmsUiVerticalScrollView.h"
#include "ui/HmsUiImageButton.h"
#include "ui/HmsUiStretchButton.h"
#include "Nav/ScratchPad/HmsScratchPadClipping.h"
#include <map>
#include "base/HmsVector.h"
#include "HmsAirportsStuDef.h"

using namespace HmsAviPf;

class CHmsProcedurePreview : public CHmsNode, public HmsUiEventInterface
{
public:
    CHmsProcedurePreview();
    ~CHmsProcedurePreview();
    CREATE_FUNC_BY_SIZE(CHmsProcedurePreview);
    virtual void OnEnter() override;
    virtual void OnExit() override;
    virtual void Update(float delta) override;
    void UpdateContent();
    void LayoutBySize(Size s);

protected:
    virtual bool Init(const HmsAviPf::Size & size);
    void OnButtonClick(CHmsUiButtonAbstract *pBtn, int index);
    void UpdateDataFromDb();

private:
    RefPtr<CHmsDrawNode> m_bgDrawNode;
    RefPtr<CHmsScratchPadClipping> m_scratchPadClipping;
    RefPtr<CHmsUiPanel> m_previewPanel;
    std::vector<StarSidStu> m_vecStarSidStu;
    const float c_scale;
    const float c_margin;

    int m_curIndex;
    int m_curRow;
};



