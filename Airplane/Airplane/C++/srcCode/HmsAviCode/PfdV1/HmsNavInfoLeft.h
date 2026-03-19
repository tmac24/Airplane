#pragma once
#include "HmsNavInfo.h"

class CHmsNavInfoLeft : public CHmsNavInfo
{
public:
	CHmsNavInfoLeft();
	~CHmsNavInfoLeft();

	CREATE_FUNC(CHmsNavInfoLeft);

	virtual bool Init() override;

	void UnfoldBox(bool unfold);
};

