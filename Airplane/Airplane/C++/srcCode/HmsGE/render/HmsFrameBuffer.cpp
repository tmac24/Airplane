#include "render/HmsFrameBuffer.h"

NS_HMS_BEGIN

FrameBuffer* FrameBuffer::_defaultFBO = nullptr;
std::set<FrameBuffer*> FrameBuffer::_frameBuffers;

Viewport::Viewport(float left, float bottom, float width, float height)
: _left(left)
, _bottom(bottom)
, _width(width)
, _height(height)
{
}

Viewport::Viewport()
{
    _left = _bottom = 0.f;
    _width = _height = 1.0f;
}

RenderTargetBase::RenderTargetBase()
{
    
}

RenderTargetBase::~RenderTargetBase()
{
    
}

bool RenderTargetBase::init(unsigned int width, unsigned int height)
{
    _width = width;
    _height = height;
    return true;
}

RenderTarget* RenderTarget::create(unsigned int width, unsigned int height, Texture2D::PixelFormat format/* = Texture2D::PixelFormat::RGBA8888*/)
{
    auto result = new RenderTarget();
    if(result && result->init(width, height,format))
    {
        result->autorelease();
        return result;
    }
    else
    {
        HMS_SAFE_DELETE(result);
        return nullptr;
    }
}

bool RenderTarget::init(unsigned int width, unsigned int height, Texture2D::PixelFormat format)
{
    if(!RenderTargetBase::init(width, height))
    {
        return false;
    }
    
    _texture = new Texture2D();
    if(nullptr == _texture) return false;
    //TODO: FIX me, get the correct bit depth for pixelformat
    auto dataLen = width * height * 4;
    auto data = malloc(dataLen);
    if( nullptr == data) return false;
    
    memset(data, 0, dataLen);
    if(_texture->initWithData(data, dataLen, format, width, height, Size(width, height)))
    {
        _texture->autorelease();
        HMS_SAFE_RETAIN(_texture);
        free(data);
    }
    else
    {
        HMS_SAFE_DELETE(_texture);
        free(data);
        return false;
    }

    return true;
}

RenderTarget::RenderTarget()
: _texture(nullptr)
{
    _type = Type::Texture2D;
}

RenderTarget::~RenderTarget()
{
    HMS_SAFE_RELEASE_NULL(_texture);
}

RenderTargetRenderBuffer::RenderTargetRenderBuffer()
: _colorBuffer(0)
, _format(GL_RGBA8)   // ⭐ 改这里
{
    _type = Type::RenderBuffer;
}

RenderTargetRenderBuffer::~RenderTargetRenderBuffer()
{
    if(glIsRenderbuffer(_colorBuffer))
    {
        glDeleteRenderbuffers(1, &_colorBuffer);
        _colorBuffer = 0;
    }
}

bool RenderTargetRenderBuffer::init(unsigned int width, unsigned int height)
{
    if(!RenderTargetBase::init(width, height)) return false;
    GLint oldRenderBuffer(0);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRenderBuffer);
    
    //generate depthStencil
    glGenRenderbuffers(1, &_colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorBuffer);
    //todo: this could have a param
    glRenderbufferStorage(GL_RENDERBUFFER, _format, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);
    
    return true;
}


RenderTargetRenderBuffer* RenderTargetRenderBuffer::create(unsigned int width, unsigned int height)
{
    auto result = new  RenderTargetRenderBuffer();
    
    if(result && result->init(width, height))
    {
        result->autorelease();
        return result;
    }
    else
    {
        HMS_SAFE_DELETE(result);
        return nullptr;
    }
}

RenderTargetDepthStencil::RenderTargetDepthStencil()
: _depthStencilBuffer(0)
{
    _type = Type::RenderBuffer;
}

RenderTargetDepthStencil::~RenderTargetDepthStencil()
{
    if(glIsRenderbuffer(_depthStencilBuffer))
    {
        glDeleteRenderbuffers(1, &_depthStencilBuffer);
        _depthStencilBuffer = 0;
    }
}

bool RenderTargetDepthStencil::init(unsigned int width, unsigned int height)
{
    if(!RenderTargetBase::init(width, height)) return false;
    GLint oldRenderBuffer(0);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRenderBuffer);
    
    //generate depthStencil
    glGenRenderbuffers(1, &_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);
    
    return true;
}


RenderTargetDepthStencil* RenderTargetDepthStencil::create(unsigned int width, unsigned int height)
{
    auto result = new  RenderTargetDepthStencil();
    
    if(result && result->init(width, height))
    {
        result->autorelease();
        return result;
    }
    else
    {
        HMS_SAFE_DELETE(result);
        return nullptr;
    }
}

bool FrameBuffer::initWithGLView(GLView* view)
{
    GLint fbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);

    if (fbo == 0)
    {
        CCLOG("ERROR: No FBO bound when creating default FBO");
        return false;
    }

    _fbo = fbo;
    _isDefault = true;

    CCLOG("Captured default FBO = %d", _fbo);

    return true;
}

FrameBuffer* FrameBuffer::getOrCreateDefaultFBO(GLView* view)
{
    if(nullptr == _defaultFBO)
    {
        auto result = new  FrameBuffer();
        
        if(result && result->initWithGLView(view))
        {
            result->autorelease();
            result->_isDefault = true;
        }
        else
        {
            HMS_SAFE_DELETE(result);
        }
        
        _defaultFBO = result;
    }
    
    return _defaultFBO;
}

void FrameBuffer::applyDefaultFBO()
{
    if(_defaultFBO)
    {
        _defaultFBO->applyFBO();
    }
}

void FrameBuffer::clearAllFBOs()
{
    for (auto fbo : _frameBuffers)
    {
        fbo->clearFBO();
    }
}

FrameBuffer* FrameBuffer::create(uint8_t fid, unsigned int width, unsigned int height)
{
    auto result = new  FrameBuffer();
    if(result && result->init(fid, width, height))
    {
        result->autorelease();
        return result;
    }
    else
    {
        HMS_SAFE_DELETE(result);
        return nullptr;
    }
}

bool FrameBuffer::init(uint8_t fid, unsigned int width, unsigned int height)
{
    _fid = fid;
    _width = width;
    _height = height;
    
    GLint oldfbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldfbo);

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, oldfbo);
    
//    _rt = RenderTarget::create(width, height);
//    if(nullptr == _rt) return false;
    return true;
}

FrameBuffer::FrameBuffer()
: _clearColor(Color4F(0, 0, 0, 1))
, _clearDepth(1.0)
, _clearStencil(0)
, _fbo(0)
, _previousFBO(0)
, _rt(nullptr)
, _rtDepthStencil(nullptr)
, _fboBindingDirty(true)
, _isDefault(false)
{
    _frameBuffers.insert(this);
}

FrameBuffer::~FrameBuffer()
{
    if(!isDefaultFBO())
    {
        HMS_SAFE_RELEASE_NULL(_rt);
        HMS_SAFE_RELEASE_NULL(_rtDepthStencil);
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
        _frameBuffers.erase(this);
    }
}

void FrameBuffer::clearFBO()
{
    applyFBO();
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClearDepth(_clearDepth);
    glClearStencil(_clearStencil);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    applyDefaultFBO();
    restoreFBO();
}

void FrameBuffer::attachRenderTarget(RenderTargetBase* rt)
{
    if(isDefaultFBO())
    {
        CCLOG("Can not apply render target to default FBO");
        return;
    }
    HMS_ASSERT(rt);
    if(rt->getWidth() != _width || rt->getHeight() != _height)
    {
        CCLOG("Error, attach a render target with different size, Skip.");
        return;
    }
    HMS_SAFE_RETAIN(rt);
    HMS_SAFE_RELEASE(_rt);
    _rt = rt;
    _fboBindingDirty = true;
}

void FrameBuffer::applyFBO()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_previousFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    if (_fboBindingDirty && !isDefaultFBO())
    {
        if (_rt == nullptr)
            return;

        if (_rt->getType() == RenderTargetBase::Type::Texture2D)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D,
                                   _rt->getTexture()->getName(),
                                   0);
        }
        else
        {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_COLOR_ATTACHMENT0,
                                      GL_RENDERBUFFER,
                                      _rt->getBuffer());
        }

        GLuint ds = _rtDepthStencil ? _rtDepthStencil->getBuffer() : 0;

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ds);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ds);
        
        
        CCLOG("FBO is %d _fbo %d color, %d ds", _fbo, RenderTargetBase::Type::Texture2D == _rt->getType() ? _rt->getTexture()->getName() : _rt->getBuffer(), nullptr == _rtDepthStencil ? 0 : _rtDepthStencil->getBuffer());

        _fboBindingDirty = false;
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("FBO Error: %x\n", status);
    }
}

void FrameBuffer::restoreFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _previousFBO);
}

void FrameBuffer::attachDepthStencilTarget(RenderTargetDepthStencil* rt)
{
    if(isDefaultFBO())
    {
        CCLOG("Can not apply depth stencil target to default FBO");
        return;
    }
    
    if(nullptr != rt && (rt->getWidth() != _width || rt->getHeight() != _height))
    {
        CCLOG("Error, attach a render target Depth stencil with different size, Skip.");
        return;
    }
    HMS_SAFE_RETAIN(rt);
    HMS_SAFE_RELEASE(_rtDepthStencil);
    _rtDepthStencil = rt;
    _fboBindingDirty = true;
}

NS_HMS_END
