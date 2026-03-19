

#include "render/HmsVertexAttribBinding.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLStateCache.h"
#include "HmsGL.h"
#include "render/HmsGLProgram.h"

NS_HMS_BEGIN

std::string s_attributeNames[] = {
    GLProgram::ATTRIBUTE_NAME_POSITION,
    GLProgram::ATTRIBUTE_NAME_COLOR,
    GLProgram::ATTRIBUTE_NAME_TEX_COORD,
    GLProgram::ATTRIBUTE_NAME_TEX_COORD1,
    GLProgram::ATTRIBUTE_NAME_TEX_COORD2,
    GLProgram::ATTRIBUTE_NAME_TEX_COORD3,
    GLProgram::ATTRIBUTE_NAME_NORMAL,
    GLProgram::ATTRIBUTE_NAME_BLEND_WEIGHT,
    GLProgram::ATTRIBUTE_NAME_BLEND_INDEX
};

static GLuint __maxVertexAttribs = 0;
static std::vector<VertexAttribBinding*> __vertexAttribBindingCache;

VertexAttribBinding::VertexAttribBinding() :
    _handle(0), _attributes(), _meshIndexData(nullptr), _glProgramState(nullptr)
{
}

VertexAttribBinding::~VertexAttribBinding()
{
    // Delete from the vertex attribute binding cache.
    std::vector<VertexAttribBinding*>::iterator itr = std::find(__vertexAttribBindingCache.begin(), __vertexAttribBindingCache.end(), this);
    if (itr != __vertexAttribBindingCache.end())
    {
        __vertexAttribBindingCache.erase(itr);
    }

    //HMS_SAFE_RELEASE(_meshIndexData);
    HMS_SAFE_RELEASE(_glProgramState);
    _attributes.clear();

#if 0
    if (_handle)
    {
        glDeleteVertexArrays(1, &_handle);
        _handle = 0;
    }
#endif
}

VertexAttribBinding* VertexAttribBinding::create(MeshIndexData* meshIndexData, GLProgramState* glProgramState)
{
    HMSASSERT(meshIndexData && glProgramState, "Invalid MeshIndexData and/or GLProgramState");

    // Search for an existing vertex attribute binding that can be used.
    VertexAttribBinding* b;
    for (size_t i = 0, count = __vertexAttribBindingCache.size(); i < count; ++i)
    {
        b = __vertexAttribBindingCache[i];
        HMS_ASSERT(b);
        if (b->_meshIndexData == meshIndexData && b->_glProgramState == glProgramState)
        {
            // Found a match!
            return b;
        }
    }

    b = new  VertexAttribBinding();
    if (b && b->init(meshIndexData, glProgramState))
    {
        b->autorelease();
        __vertexAttribBindingCache.push_back(b);
    }

    return b;
}

bool VertexAttribBinding::init(MeshIndexData* meshIndexData, GLProgramState* glProgramState)
{
#if 0
    CCASSERT(meshIndexData && glProgramState, "Invalid arguments");

    // One-time initialization.
    if (__maxVertexAttribs == 0)
    {
        GLint temp;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &temp);

        __maxVertexAttribs = temp;
        if (__maxVertexAttribs <= 0)
        {
            CCLOGERROR("The maximum number of vertex attributes supported by OpenGL on the current device is 0 or less.");
            return NULL;
        }
    }

    _meshIndexData = meshIndexData;
    _meshIndexData->retain();
    _glProgramState = glProgramState;
    _glProgramState->retain();

    auto meshVertexData = meshIndexData->getMeshVertexData();
    auto attributeCount = meshVertexData->getMeshVertexAttribCount();


    // Parse and set attributes
    parseAttributes();
    long offset = 0;
    for (auto k = 0; k < attributeCount; k++)
    {
        auto meshattribute = meshVertexData->getMeshVertexAttrib(k);
        setVertexAttribPointer(
                               s_attributeNames[meshattribute.vertexAttrib],
                               meshattribute.size,
                               meshattribute.type,
                               GL_FALSE,
                               meshVertexData->getVertexBuffer()->getSizePerVertex(),
                               (GLvoid*)offset);
        offset += meshattribute.attribSizeBytes;
    }

    // VAO hardware
    //if (Configuration::getInstance()->supportsShareableVAO())
	if (Renderer::UseGlVao())
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &_handle);
        GL::bindVAO(_handle);
        glBindBuffer(GL_ARRAY_BUFFER, meshVertexData->getVertexBuffer()->getVBO());

        auto flags = _vertexAttribsFlags;
        for (int i = 0; flags > 0; i++) {
            int flag = 1 << i;
            if (flag & flags)
                glEnableVertexAttribArray(i);
            flags &= ~flag;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexData->getIndexBuffer()->getVBO());

        for(auto &attribute : _attributes)
        {
            attribute.second.apply();
        }

        GL::bindVAO(0);
    }
#endif
    return true;
}

void VertexAttribBinding::bind()
{

    if (_handle)
    {
        // hardware
        GL::bindVAO(_handle);
    }
    else
    {
#if 0
        // software
        auto meshVertexData = _meshIndexData->getMeshVertexData();
        glBindBuffer(GL_ARRAY_BUFFER, meshVertexData->getVertexBuffer()->getVBO());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshIndexData->getIndexBuffer()->getVBO());

        // Software mode
        GL::enableVertexAttribs(_vertexAttribsFlags);
        // set attributes
        for(auto &attribute : _attributes)
        {
            attribute.second.apply();
        }
#endif
        
    }
}

void VertexAttribBinding::unbind()
{
    if (_handle)
    {
        // Hardware
       GL::bindVAO(0);
    }
    else
    {
        // Software
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

uint32_t VertexAttribBinding::getVertexAttribsFlags() const
{
    return _vertexAttribsFlags;
}

void VertexAttribBinding::parseAttributes()
{
    HMSASSERT(_glProgramState, "invalid glprogram");

    _attributes.clear();
    _vertexAttribsFlags = 0;

    auto glprogram = _glProgramState->getGLProgram();

    for(auto &attrib: glprogram->_vertexAttribs)
    {
        VertexAttribValue value(&attrib.second);
        _attributes[attrib.first] = value;
    }
}

VertexAttribValue* VertexAttribBinding::getVertexAttribValue(const std::string& name)
{
    const auto itr = _attributes.find(name);
    if( itr != _attributes.end())
        return &itr->second;
    return nullptr;
}

void VertexAttribBinding::setVertexAttribPointer(const std::string &name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer)
{
    auto v = getVertexAttribValue(name);
    if(v) {
        v->setPointer(size, type, normalized, stride, pointer);
        _vertexAttribsFlags |= 1 << v->_vertexAttrib->index;
    }
    else
    {
        CCLOG("HmsAviPf: warning: Attribute not found: %s", name.c_str());
    }
}

NS_HMS_END
