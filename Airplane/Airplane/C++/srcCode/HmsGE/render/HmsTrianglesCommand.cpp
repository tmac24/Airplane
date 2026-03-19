#include "render/HmsTrianglesCommand.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsRenderer.h"
#include "external/xxhash/xxhash.h"

NS_HMS_BEGIN

TrianglesCommand::TrianglesCommand()
:_materialID(0)
,_textureID(0)
,_glProgramState(nullptr)
,_blendType(BlendFunc::DISABLE)
{
    _type = RenderCommand::Type::TRIANGLES_COMMAND;
}

void TrianglesCommand::init(float globalOrder, GLuint textureID, GLProgramState* glProgramState, BlendFunc blendType, const Triangles& triangles,const Mat4& mv, uint32_t flags)
{
    HMSASSERT(glProgramState, "Invalid GLProgramState");
    HMSASSERT(glProgramState->getVertexAttribsFlags() == 0, "No custom attributes are supported in QuadCommand");
    
    RenderCommand::init(globalOrder, mv, flags);
    
    _triangles = triangles;
    if(_triangles.indexCount % 3 != 0)
    {
        ssize_t count = _triangles.indexCount;
        _triangles.indexCount = count / 3 * 3;
        CCLOGERROR("Resize indexCount from %zd to %zd, size must be multiple times of 3", count, _triangles.indexCount);
    }
    _mv = mv;
    
    if( _textureID != textureID || _blendType.src != blendType.src || _blendType.dst != blendType.dst || _glProgramState != glProgramState) {
        
        _textureID = textureID;
        _blendType = blendType;
        _glProgramState = glProgramState;
        
        generateMaterialID();
    }
}

TrianglesCommand::~TrianglesCommand()
{
}

void TrianglesCommand::generateMaterialID()
{
    
    if(_glProgramState->getUniformCount() > 0)
    {
        _materialID = Renderer::MATERIAL_ID_DO_NOT_BATCH;
    }
    else
    {
        int glProgram = (int)_glProgramState->getGLProgram()->getProgram();
        int intArray[4] = { glProgram, (int)_textureID, (int)_blendType.src, (int)_blendType.dst};
        
        _materialID = XXH32((const void*)intArray, sizeof(intArray), 0);
    }
}

void TrianglesCommand::useMaterial() const
{
    //Set texture
    GL::bindTexture2D(_textureID);
    
    //set blend mode
    GL::blendFunc(_blendType.src, _blendType.dst);
    
    _glProgramState->apply(_mv);
}

NS_HMS_END
