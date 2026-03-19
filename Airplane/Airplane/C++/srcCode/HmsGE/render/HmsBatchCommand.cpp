#include "render/HmsBatchCommand.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsTextureAtlas.h"
#include "render/HmsTexture2D.h"
#include "render/HmsGLProgram.h"

NS_HMS_BEGIN

BatchCommand::BatchCommand()
: _textureID(0)
, _blendType(BlendFunc::DISABLE)
, _textureAtlas(nullptr)
{
    _type = RenderCommand::Type::BATCH_COMMAND;
    _shader = nullptr;
}

void BatchCommand::init(float globalOrder, GLProgram* shader, BlendFunc blendType, TextureAtlas *textureAtlas, const Mat4& modelViewTransform, uint32_t flags)
{
    HMSASSERT(shader, "shader cannot be null");
    HMSASSERT(textureAtlas, "textureAtlas cannot be null");
    
    RenderCommand::init(globalOrder, modelViewTransform, flags);
    _textureID = textureAtlas->getTexture()->getName();
    _blendType = blendType;
    _shader = shader;
    
    _textureAtlas = textureAtlas;
    
    _mv = modelViewTransform;
}

BatchCommand::~BatchCommand()
{
}

void BatchCommand::execute()
{
    // Set material
    _shader->use();
    _shader->setUniformsForBuiltins(_mv);
    GL::bindTexture2D(_textureID);
    GL::blendFunc(_blendType.src, _blendType.dst);

    // Draw
    _textureAtlas->drawQuads();
}

NS_HMS_END
