#pragma once
#include "display/HmsImageNode.h"
#include "ui/HmsUiEventInterface.h"

using namespace HmsAviPf;

class CHmsNavSvsControlPage : public CHmsNode, public HmsUiEventInterface
{
public:
    CHmsNavSvsControlPage();
    ~CHmsNavSvsControlPage();
    CREATE_FUNC_BY_SIZE(CHmsNavSvsControlPage);
    bool Init(const HmsAviPf::Size & size);

private:
};

