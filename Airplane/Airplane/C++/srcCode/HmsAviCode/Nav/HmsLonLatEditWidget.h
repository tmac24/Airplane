#pragma once

#include "ui/HmsUiStretchButton.h"
#include "display/HmsLineEdit.h"
#include "ui/HmsKeyboard.h"
#include "RoutePlanData/mcduDef.h"
#include <map>
#include "HmsLonLatDegreeEdit.h"

using namespace HmsAviPf;

class CHmsLonLatEditWidget : public HmsAviPf::CHmsNode, public HmsUiEventInterface
{
public:
    CHmsLonLatEditWidget();
    ~CHmsLonLatEditWidget();

    CREATE_FUNC(CHmsLonLatEditWidget)
    virtual bool Init();

    void Blur();
    void ClearText();
    bool IsRightResult();
    std::string GetFormatString();

protected:
    virtual bool OnPressed(const Vec2 & posOrigin) override;

private:
    RefPtr<CHmsLonLatDegreeEdit> m_pLonDegreeEdit;
    RefPtr<CHmsLonLatDegreeEdit> m_pLatDegreeEdit;
};
