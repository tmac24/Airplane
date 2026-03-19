#pragma once
#include "ui/HmsUiEventInterface.h"
#include "base/HmsNode.h"
#include "base/HmsMap.h"
#include <map>

using namespace HmsAviPf;

typedef std::function<bool(std::string)>  RARALLEL_CALLBACK;

struct ParallelCallback
{
	RARALLEL_CALLBACK callback;
};

class CHmsParallelLayer : public CHmsNode, public HmsUiEventInterface
{
public:
	CREATE_FUNC(CHmsParallelLayer);

	CHmsParallelLayer();

	virtual bool Init() override;

    void AddScreen(CHmsNode * node, const std::string & strName);
    void AddSubScreen(CHmsNode * node, const std::string & strName);
	void AddScreenAction(const RARALLEL_CALLBACK & callback, const std::string & strName);

	virtual bool OnPressed(const Vec2 & posOrigin) override;

	virtual void OnMove(const Vec2 & posOrigin) override;

	virtual void OnReleased(const Vec2 & posOrigin) override;

	void RunToScreen(const std::string & strName);
	void ShowSubScreen(const std::string & strName, bool bShow);
	void RunScreenAction(const std::string & strName, const std::string & strParam);

private:
    Map<std::string, CHmsNode*>  m_mapParallelScreen;
    Map<std::string, CHmsNode*>  m_mapSubScreen;
	std::map<std::string, ParallelCallback>    m_mapAction;

    CHmsNode  *  m_pCurNode;

	Vec2					 m_posStart;
	Vec2					 m_posLast;
};

