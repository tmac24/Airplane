#pragma once
#include "base/HmsNode.h"
#include "base/RefPtr.h"
#include "display/HmsImageNode.h"
NS_HMS_BEGIN

enum class HMS_POINTER_TYPE
{
	Pointer_None,
	Pointer_Normal,
	Pointer_Click_Down,
};

class CHmsScreenPointer : public CHmsNode
{
public:
	CHmsScreenPointer();
	~CHmsScreenPointer();

	CREATE_FUNC(CHmsScreenPointer);

	virtual bool Init() override;

	void SetPointerPostion(const Vec2 & pos);
	void SetPointerType(HMS_POINTER_TYPE eType);

private:
	RefPtr<CHmsImageNode>  m_pointer;
};

NS_HMS_END