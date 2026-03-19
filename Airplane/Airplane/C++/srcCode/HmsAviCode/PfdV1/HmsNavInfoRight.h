#pragma once
#include "HmsNavInfo.h"

class CHmsNavInfoRight : public CHmsNavInfo
{
public:
	CHmsNavInfoRight();
	~CHmsNavInfoRight();

	CREATE_FUNC(CHmsNavInfoRight);

	virtual bool Init() override;

	void UnfoldBox(bool unfold);
};

