#include "HmsScreenPointer.h"

NS_HMS_BEGIN

CHmsScreenPointer::CHmsScreenPointer()
{
}


CHmsScreenPointer::~CHmsScreenPointer()
{
}

bool CHmsScreenPointer::Init()
{
	if (!CHmsNode::Init())
	{
		return false;
	}
	this->SetLocalZOrder(+20);

	m_pointer = CHmsImageNode::Create("res/Cursor/cursor.png");
	if (m_pointer)
	{
		m_pointer->SetAnchorPoint(Vec2(2/16.0f, 1.0f));
		this->AddChild(m_pointer);
	}

	return true;
}

void CHmsScreenPointer::SetPointerType(HMS_POINTER_TYPE eType)
{
	switch (eType)
	{
	case HMS_POINTER_TYPE::Pointer_Click_Down:
		m_pointer->SetTexture("res/Cursor/cursor-small.png");
		break;
	case HMS_POINTER_TYPE::Pointer_Normal:
		m_pointer->SetTexture("res/Cursor/cursor.png");
		break;
	}
}

void CHmsScreenPointer::SetPointerPostion(const Vec2 & pos)
{
	if (m_pointer)
	{
		m_pointer->SetPosition(pos);
	}
}

NS_HMS_END