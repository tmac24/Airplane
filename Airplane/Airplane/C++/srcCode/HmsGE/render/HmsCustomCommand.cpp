#include "render/HmsCustomCommand.h"

NS_HMS_BEGIN

CustomCommand::CustomCommand()
: func(nullptr)
{
    _type = RenderCommand::Type::CUSTOM_COMMAND;
}

void CustomCommand::init(float depth, const HmsAviPf::Mat4 &modelViewTransform, uint32_t flags)
{
    RenderCommand::init(depth, modelViewTransform, flags);
}

void CustomCommand::init(float globalOrder)
{
    _globalOrder = globalOrder;
}

CustomCommand::~CustomCommand()
{

}

void CustomCommand::execute()
{
    if(func)
    {
        func();
    }
}

NS_HMS_END
