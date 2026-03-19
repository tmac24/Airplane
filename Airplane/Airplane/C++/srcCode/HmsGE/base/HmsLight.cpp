#include "base/HmsLight.h"
#include "math/HmsMathBase.h"
#include "HmsAviDisplayUnit.h"

NS_HMS_BEGIN

void BaseLight::setIntensity(float intensity)
{
    HMS_ASSERT(intensity >= 0);
    _intensity = intensity;
}

void BaseLight::OnEnter()
{
#if 0
    auto scene = getScene();
    if (scene)
    {
        auto &lights = scene->_lights;
        auto iter = std::find(lights.begin(), lights.end(), this);
        if (iter == lights.end())
            lights.push_back(this);
    }
#endif
    CHmsNode::OnEnter();
}

void BaseLight::OnExit()
{
#if 0
    auto scene = getScene();
    if (scene)
    {
        auto &lights = scene->_lights;
        auto iter = std::find(lights.begin(), lights.end(), this);
        if (iter != lights.end())
            lights.erase(iter);
    }
#endif
    CHmsNode::OnExit();
}

void BaseLight::setRotationFromDirection( const Vec3 &direction )
{
    float projLen = sqrt(direction.x * direction.x + direction.z * direction.z);
	float rotY = MATH_RAD_TO_DEG(atan2f(-direction.x, -direction.z));
	float rotX = -MATH_RAD_TO_DEG(atan2f(-direction.y, projLen));
    SetRotation3D(Vec3(rotX, rotY, 0.0f));
}

BaseLight::BaseLight()
: _intensity(1.0f)
, _lightFlag(LightFlag::LIGHT0)
, _enabled(true)
{
    
}
BaseLight::~BaseLight()
{
    
}


////////////////////////////////////////////////////////////////////
DirectionLight* DirectionLight::create(const Vec3 &direction, const Color3B &color)
{
    auto light = new DirectionLight();
    light->setRotationFromDirection(direction);
    light->SetColor(color);
    light->autorelease();
    return light;
}

void DirectionLight::setDirection(const Vec3 &dir)
{
    setRotationFromDirection(dir);
}
Vec3 DirectionLight::getDirection() const
{
    Mat4 mat = GetNodeToParentTransform();
    return Vec3(-mat.m[8], -mat.m[9], -mat.m[10]);
}
Vec3 DirectionLight::getDirectionInWorld() const
{
    Mat4 mat = GetNodeToWorldTransform();
    return Vec3(-mat.m[8], -mat.m[9], -mat.m[10]);
}
DirectionLight::DirectionLight()
{
    
}
DirectionLight::~DirectionLight()
{
    
}

//////////////////////////////////////////////////////////////////
PointLight* PointLight::create(const Vec3 &position, const Color3B &color, float range)
{
    auto light = new PointLight();
    light->SetPosition3D(position);
    light->SetColor(color);
    light->_range = range;
    light->autorelease();
    return light;
}

PointLight::PointLight()
{
    
}
PointLight::~PointLight()
{
    
}

//////////////////////////////////////////////////////////////
SpotLight* SpotLight::create(const Vec3 &direction, const Vec3 &position, const Color3B &color, float innerAngle, float outerAngle, float range)
{
    auto light = new (std::nothrow) SpotLight();
    light->setRotationFromDirection(direction);
    light->SetPosition3D(position);
    light->SetColor(color);
    light->setInnerAngle(innerAngle);
    light->setOuterAngle(outerAngle);
    light->_range = range;
    light->autorelease();
    return light;
}

void SpotLight::setDirection(const Vec3 &dir)
{
    setRotationFromDirection(dir);
}

Vec3 SpotLight::getDirection() const
{
    Mat4 mat = GetNodeToParentTransform();
    return Vec3(-mat.m[8], -mat.m[9], -mat.m[10]);
}

Vec3 SpotLight::getDirectionInWorld() const
{
    Mat4 mat = GetNodeToWorldTransform();
    return Vec3(-mat.m[8], -mat.m[9], -mat.m[10]);
}

void SpotLight::setInnerAngle(float angle)
{
    _innerAngle = angle;
    _cosInnerAngle = cosf(angle);
}

void SpotLight::setOuterAngle(float angle)
{
    _outerAngle = angle;
    _cosOuterAngle = cosf(angle);
}

SpotLight::SpotLight()
{

}

SpotLight::~SpotLight()
{

}

/////////////////////////////////////////////////////////////

AmbientLight* AmbientLight::create( const Color3B &color )
{
    auto light = new (std::nothrow) AmbientLight();
    light->SetColor(color);
    light->autorelease();
    return light;
}

AmbientLight::AmbientLight()
{

}

AmbientLight::~AmbientLight()
{

}

NS_HMS_END