#include "HmsPass.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLProgram.h"
#include "render/HmsTexture2D.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsTechnique.h"
#include "render/HmsMaterial.h"
#include "render/HmsVertexAttribBinding.h"

#include "base/HmsTypes.h"
#include "base/HmsNode.h"

NS_HMS_BEGIN


Pass* Pass::create(Technique* technique)
{
    auto pass = new  Pass();
    if (pass && pass->init(technique))
    {
        pass->autorelease();
        return pass;
    }
    return nullptr;
}

Pass* Pass::createWithGLProgramState(Technique* technique, GLProgramState* programState)
{
    auto pass = new  Pass();
    if (pass && pass->initWithGLProgramState(technique, programState))
    {
        pass->autorelease();
        return pass;
    }
    return nullptr;
}

bool Pass::init(Technique* technique)
{
    _parent = technique;
    return true;
}

bool Pass::initWithGLProgramState(Technique* technique, GLProgramState *glProgramState)
{
    _parent = technique;
    _glProgramState = glProgramState;
    HMS_SAFE_RETAIN(_glProgramState);
    return true;
}

Pass::Pass()
: _glProgramState(nullptr)
, _vertexAttribBinding(nullptr)
{
}

Pass::~Pass()
{
    HMS_SAFE_RELEASE(_glProgramState);
    HMS_SAFE_RELEASE(_vertexAttribBinding);
}

Pass* Pass::clone() const
{
    auto pass = new  Pass();
    if (pass)
    {
        RenderState::cloneInto(pass);
        pass->_glProgramState = _glProgramState->clone();
        HMS_SAFE_RETAIN(pass->_glProgramState);

        pass->_vertexAttribBinding = _vertexAttribBinding;
        HMS_SAFE_RETAIN(pass->_vertexAttribBinding);

        pass->autorelease();
    }
    return pass;
}

GLProgramState* Pass::getGLProgramState() const
{
    return _glProgramState;
}

void Pass::setGLProgramState(GLProgramState* glProgramState)
{
    if ( _glProgramState != glProgramState) {
        HMS_SAFE_RELEASE(_glProgramState);
        _glProgramState = glProgramState;
        HMS_SAFE_RETAIN(_glProgramState);

        _hashDirty = true;
    }
}

uint32_t Pass::getHash() const
{
    if (_hashDirty || _state->isDirty()) {
        uint32_t glProgram = (uint32_t)_glProgramState->getGLProgram()->getProgram();
        uint32_t textureid = _texture ? _texture->getName() : -1;
        uint32_t stateblockid = _state->getHash();

        _hash = glProgram ^ textureid ^ stateblockid;

//        _hash = XXH32((const void*)intArray, sizeof(intArray), 0);
        _hashDirty = false;
    }

    return _hash;
}

void Pass::bind(const Mat4& modelView)
{
    bind(modelView, true);
}

void Pass::bind(const Mat4& modelView, bool bindAttributes)
{

    // vertex attribs
    if (bindAttributes && _vertexAttribBinding)
        _vertexAttribBinding->bind();

    auto glprogramstate = _glProgramState ? _glProgramState : getTarget()->GetGLProgramState();

    glprogramstate->applyGLProgram(modelView);
    glprogramstate->applyUniforms();

    //set render state
    RenderState::bind(this);

}

CHmsNode* Pass::getTarget() const
{
    HMSASSERT(_parent && _parent->_parent, "Pass must have a Technique and Material");

    Material *material = static_cast<Material*>(_parent->_parent);
    return material->_target;
}

void Pass::unbind()
{
    RenderState::StateBlock::restore(0);

    _vertexAttribBinding->unbind();
}

void Pass::setVertexAttribBinding(VertexAttribBinding* binding)
{
    if (_vertexAttribBinding != binding)
    {
        HMS_SAFE_RELEASE(_vertexAttribBinding);
        _vertexAttribBinding = binding;
        HMS_SAFE_RETAIN(_vertexAttribBinding);
    }
}

VertexAttribBinding* Pass::getVertexAttributeBinding() const
{
    return _vertexAttribBinding;
}


NS_HMS_END
