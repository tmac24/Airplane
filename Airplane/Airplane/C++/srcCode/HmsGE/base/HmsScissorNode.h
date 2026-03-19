#pragma once

#include "base/HmsNode.h"
#include "render/HmsGroupCommand.h"
#include "render/HmsCustomCommand.h"
#include "ui/HmsUiEventInterface.h"

NS_HMS_BEGIN

class HMS_DLL CHmsScissorNode : public CHmsNode, public HmsUiEventInterface
{
public:
    static CHmsScissorNode* create();
    CHmsScissorNode();
    virtual ~CHmsScissorNode();
    virtual bool Init() override;
    virtual void Visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags) override;  
	void SetClipRect(Rect rect);

private:
    void onBeforeVisit();
    void onAfterVisit();

private:
    GroupCommand _groupCommand;
    CustomCommand _beforeVisitCmd;
    CustomCommand _afterVisitCmd;
	Rect m_clipRect;
	Rect m_clipWorldRect;
	bool m_bClipDirty;
};

NS_HMS_END
