#include "HmsParallelLayer.h"
#include "HmsGlobal.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsActionImmediatelyRef.h"
USING_NS_HMS;

CHmsParallelLayer::CHmsParallelLayer()
	: HmsUiEventInterface(this)
	, m_pCurNode(nullptr)
{

}

bool CHmsParallelLayer::Init()
{
    if (!CHmsNode::Init())
	{
		return false;
	}

	auto height = CHmsGlobal::GetInstance()->GetDesignHeight();
	auto width = CHmsGlobal::GetInstance()->GetDesignContentWidthFull();
	Size size(width, height);
	this->SetContentSize(size);

	//this->RegisterMutiTouchEvent(3);
	return true;
}

void CHmsParallelLayer::AddScreen(CHmsNode * node, const std::string & strName)
{
	if (node)
	{
		int nPosition = m_mapParallelScreen.size();
		auto width = CHmsGlobal::GetInstance()->GetDesignContentWidthFull();
		auto height = CHmsGlobal::GetInstance()->GetDesignHeight();

		if (0 == nPosition)
		{
			node->SetVisible(true);
			m_pCurNode = node;
		}
		else
		{
			node->SetVisible(false);
		}

		width *= nPosition;
		node->SetPosition(width, 0);
		this->AddChild(node);

		m_mapParallelScreen.insert(strName, node);

		Size size(width, height);
		this->SetContentSize(size);
	}
}

bool CHmsParallelLayer::OnPressed(const Vec2 & posOrigin)
{
	m_posLast = m_posStart = posOrigin;
	return true;
}

void CHmsParallelLayer::OnMove(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posLast;
    auto pos = this->GetPoistion();
	pos += Vec2(delta.x, 0);
	this->SetPosition(pos);
}

void CHmsParallelLayer::OnReleased(const Vec2 & posOrigin)
{
	auto delta = posOrigin - m_posStart;
	if (delta.x)
	{
	}
}

void CHmsParallelLayer::AddSubScreen(CHmsNode * node, const std::string & strName)
{
	m_mapSubScreen.insert(strName, node);
}

void CHmsParallelLayer::RunToScreen(const std::string & strName)
{
	auto itFind = m_mapParallelScreen.find(strName);
	if (itFind != m_mapParallelScreen.end())
	{
		auto node = itFind->second;
		if (m_pCurNode != node)
		{
            auto pos = itFind->second->GetPoistion();
			node->SetVisible(true);

            auto moveto = CHmsActionMoveTo::Create(1, Vec2(-pos.x, pos.y));
            auto done = CHmsActionCallback::Create([=](){
				m_pCurNode->SetVisible(false);
				m_pCurNode = node;
			});

            auto seq = CHmsActionQueue::Create(moveto, done, nullptr);
			this->StopAllActions();
			this->RunAction(seq);
		}
	}
}

void CHmsParallelLayer::ShowSubScreen(const std::string & strName, bool bShow)
{
	auto itFind = m_mapSubScreen.find(strName);
	if (itFind != m_mapSubScreen.end())
	{
		auto node = itFind->second;
		if (node)
		{
			node->SetVisible(bShow);
		}
	}
}

void CHmsParallelLayer::AddScreenAction(const RARALLEL_CALLBACK & callback, const std::string & strName)
{
	ParallelCallback cb;
	cb.callback = callback;
	m_mapAction[strName] = cb;
}

void CHmsParallelLayer::RunScreenAction(const std::string & strName, const std::string & strParam)
{
	auto it = m_mapAction.find(strName);
	if (it != m_mapAction.end())
	{
		auto cb = it->second;
		if (cb.callback)
		{
			cb.callback(strParam);
		}
	}
}

