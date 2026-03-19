
#include "render/HmsQuadCommand.h"

#include "render/HmsGLStateCache.h"
#include "render/HmsGLProgram.h"
#include "render/HmsMaterial.h"
#include "render/HmsTechnique.h"
#include "render/HmsRenderer.h"
#include "render/HmsPass.h"
#include "external/xxhash/xxhash.h"

NS_HMS_BEGIN


QuadCommand::QuadCommand()
:_materialID(0)
,_textureID(0)
,_glProgramState(nullptr)
,_blendType(BlendFunc::DISABLE)
,_quads(nullptr)
,_quadsCount(0)
{
    _type = RenderCommand::Type::QUAD_COMMAND;
}

void QuadCommand::init(float globalOrder, GLuint textureID, GLProgramState* shader, const BlendFunc& blendType, V3F_C4B_T2F_Quad* quads, ssize_t quadCount,
                       const Mat4& mv, uint32_t flags)
{
    HMSASSERT(shader, "Invalid GLProgramState");
    HMSASSERT(shader->getVertexAttribsFlags() == 0, "No custom attributes are supported in QuadCommand");
    
    RenderCommand::init(globalOrder, mv, flags);
    
    _quadsCount = quadCount;
    _quads = quads;
    
    _mv = mv;
    
    if( _textureID != textureID || _blendType.src != blendType.src || _blendType.dst != blendType.dst || _glProgramState != shader) {
        
        _textureID = textureID;
        _blendType = blendType;
        _glProgramState = shader;
        
        generateMaterialID();
    }
}

QuadCommand::~QuadCommand()
{
}

void QuadCommand::generateMaterialID()
{
    _skipBatching = false;

    if(_glProgramState->getUniformCount() == 0)
    {
        int glProgram = (int)_glProgramState->getGLProgram()->getProgram();
        int intArray[4] = { glProgram, (int)_textureID, (int)_blendType.src, (int)_blendType.dst};

        _materialID = XXH32((const void*)intArray, sizeof(intArray), 0);
    }
    else
    {
        _materialID = Renderer::MATERIAL_ID_DO_NOT_BATCH;
        _skipBatching = true;
    }
}

void QuadCommand::useMaterial() const
{
    //Set texture
    GL::bindTexture2D(_textureID);
    
    //set blend mode
    GL::blendFunc(_blendType.src, _blendType.dst);
    
    _glProgramState->applyGLProgram(_mv);
    _glProgramState->applyUniforms();
}

NS_HMS_END
