#include "display/HmsBackgroundNode.h"
#include <algorithm>
#include "render/HmsTextureCache.h"
#include "render/HmsTexture2D.h"
#include "render/HmsRenderer.h"
#include "HmsAviDisplayUnit.h"
#include "base/HmsCamera.h"

#define  HMS_USE_CULLING 1

NS_HMS_BEGIN

unsigned short CHmsBackgroundNode::s_quadIndices[] = { 0, 1, 2, 3, 2, 1 };

// MARK: create, init, dealloc
CHmsBackgroundNode* CHmsBackgroundNode::CreateWithTexture(Texture2D *texture)
{
    CHmsBackgroundNode *imageNode = new STD_NOTHROW CHmsBackgroundNode();
    if (imageNode && imageNode->InitWithTexture(texture))
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsBackgroundNode* CHmsBackgroundNode::CreateWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
{
    CHmsBackgroundNode *imageNode = new STD_NOTHROW CHmsBackgroundNode();
    if (imageNode && imageNode->InitWithTexture(texture, rect, rotated))
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsBackgroundNode* CHmsBackgroundNode::Create(const std::string& filename)
{
    CHmsBackgroundNode *imageNode = new STD_NOTHROW CHmsBackgroundNode();
    if (imageNode && imageNode->InitWithFile(filename))
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsBackgroundNode* CHmsBackgroundNode::Create(const std::string& filename, const Rect& rect)
{
    CHmsBackgroundNode *imageNode = new STD_NOTHROW CHmsBackgroundNode();
    if (imageNode && imageNode->InitWithFile(filename, rect))
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsBackgroundNode* CHmsBackgroundNode::Create()
{
    CHmsBackgroundNode *imageNode = new STD_NOTHROW CHmsBackgroundNode();
    if (imageNode && imageNode->Init())
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

bool CHmsBackgroundNode::Init(void)
{
    return InitWithTexture(nullptr, Rect::ZERO );
}

bool CHmsBackgroundNode::InitWithTexture(Texture2D *texture)
{
    HMSASSERT(texture != nullptr, "Invalid texture for imageNode");

    Rect rect = Rect::ZERO;
	rect.size = texture->getContentSize();

    return InitWithTexture(texture, rect);
}

bool CHmsBackgroundNode::InitWithTexture(Texture2D *texture, const Rect& rect)
{
    return InitWithTexture(texture, rect, false);
}

bool CHmsBackgroundNode::InitWithFile(const std::string& filename)
{
    HMSASSERT(filename.size()>0, "Invalid filename for imageNode");

	Texture2D *texture = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(filename);
    if (texture)
    {
        Rect rect = Rect::ZERO;
		rect.size = texture->getContentSize();
        return InitWithTexture(texture, rect);
    }

    return false;
}

bool CHmsBackgroundNode::InitWithFile(const std::string &filename, const Rect& rect)
{
    HMSASSERT(filename.size()>0, "Invalid filename");

	Texture2D *texture = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(filename);
    if (texture)
    {
        return InitWithTexture(texture, rect);
    }

    return false;
}

// designated initializer
bool CHmsBackgroundNode::InitWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
{
    bool result;
    if (CHmsNode::Init())
    {   
        _recursiveDirty = false;
        SetDirty(false);
        
        _opacityModifyRGB = true;
        
        _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
        
        _flippedX = _flippedY = false;
        
        // default transform anchor: center
        SetAnchorPoint(Vec2(0.5f, 0.5f));

        // clean the Quad
        memset(&_quad, 0, sizeof(_quad));
        
        // Atlas: Color
        _quad.bl.colors = Color4B::WHITE;
        _quad.br.colors = Color4B::WHITE;
        _quad.tl.colors = Color4B::WHITE;
        _quad.tr.colors = Color4B::WHITE;
        
        // shader state
        SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));

        // update texture (calls updateBlendFunc)
        SetTexture(texture);
        SetTextureRect(rect, rotated, rect.size);
        
        result = true;
    }
    else
    {
        result = false;
    }
    _recursiveDirty = true;
    SetDirty(true);
    return result;
}

CHmsBackgroundNode::CHmsBackgroundNode()
	: _shouldBeHidden(false)
	, _texture(nullptr)
	, _insideBounds(true)
{
	m_triangles.indexCount = 6;
	m_triangles.vertCount = 4;
	m_triangles.verts = (V3F_C4B_T2F*)&_quad;
	m_triangles.indices = s_quadIndices;
}

CHmsBackgroundNode::~CHmsBackgroundNode(void)
{
    HMS_SAFE_RELEASE(_texture);
}

/*
 * Texture methods
 */

/*
 * This array is the data of a white image with 2 by 2 dimension.
 * It's used for creating a default texture when imageNode's texture is set to nullptr.
 * Supposing codes as follows:
 *
 *   auto sp = new STD_NOTHROW CHmsImageNode();
 *   sp->init();  // Texture was set to nullptr, in order to make opacity and color to work correctly, we need to create a 2x2 white texture.
 *
 * The test is in "TestCpp/CHmsImageNodeTest/CHmsImageNode without texture".
 */
static unsigned char HMS_2x2_white_image[] = {
    // RGBA8888
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

#define HMS_2x2_WHITE_IMAGE_KEY  "/HMS_2x2_white_image"

// MARK: texture
void CHmsBackgroundNode::SetTexture(const std::string &filename)
{
	Texture2D *texture = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(filename);
    SetTexture(texture);

    Rect rect = Rect::ZERO;
    if (texture)
		rect.size = texture->getContentSize();
    SetTextureRect(rect);
}

void CHmsBackgroundNode::SetTexture(Texture2D *texture)
{
    if (texture == nullptr)
    {
        // Gets the texture by key firstly.
		texture = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->getTextureForKey(HMS_2x2_WHITE_IMAGE_KEY);

        // If texture wasn't in cache, create it from RAW data.
        if (texture == nullptr)
        {
            Image* image = new STD_NOTHROW Image();
            bool isOK = image->initWithRawData(HMS_2x2_white_image, sizeof(HMS_2x2_white_image), 2, 2, 8);
            HMS_UNUSED_PARAM(isOK);

			texture = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(image, HMS_2x2_WHITE_IMAGE_KEY);
            HMS_SAFE_RELEASE(image);
        }
    }

    if (_texture != texture)
    {
        HMS_SAFE_RETAIN(texture);
        HMS_SAFE_RELEASE(_texture);
        _texture = texture;
        UpdateBlendFunc();
    }
}

Texture2D* CHmsBackgroundNode::GetTexture() const
{
    return _texture;
}

void CHmsBackgroundNode::SetTextureRect(const Rect& rect)
{
    SetTextureRect(rect, false, rect.size);
}

void CHmsBackgroundNode::SetTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize)
{
    _rectRotated = rotated;

    SetContentSize(untrimmedSize);
    SetVertexRect(rect);
    SetTextureCoords(rect);
	
    {
        // self rendering
        
        // Atlas: Vertex
		float x1 = 0.0f + (m_sizeContent.width - _rect.size.width) / 2;
		float y1 = 0.0f + (m_sizeContent.height - _rect.size.height) / 2;
        float x2 = x1 + _rect.size.width;
        float y2 = y1 + _rect.size.height;

        // Don't update Z.
        _quad.bl.vertices.set(x1, y1, 0.0f);
        _quad.br.vertices.set(x2, y1, 0.0f);
        _quad.tl.vertices.set(x1, y2, 0.0f);
        _quad.tr.vertices.set(x2, y2, 0.0f);
    }
}

// override this method to generate "double scale" imageNodes
void CHmsBackgroundNode::SetVertexRect(const Rect& rect)
{
    _rect = rect;
}

void CHmsBackgroundNode::SetTextureCoords(Rect rect)
{
	Texture2D *tex = _texture;
    if (! tex)
    {
        return;
    }

    float atlasWidth = (float)tex->getPixelsWide();
    float atlasHeight = (float)tex->getPixelsHigh();

    float left, right, top, bottom;

    if (_rectRotated)
    {
        left    = rect.origin.x/atlasWidth;
        right   = (rect.origin.x+rect.size.height) / atlasWidth;
        top     = rect.origin.y/atlasHeight;
        bottom  = (rect.origin.y+rect.size.width) / atlasHeight;

        if (_flippedX)
        {
            std::swap(top, bottom);
        }

        if (_flippedY)
        {
            std::swap(left, right);
        }

        _quad.bl.texCoords.u = left;
        _quad.bl.texCoords.v = top;
        _quad.br.texCoords.u = left;
        _quad.br.texCoords.v = bottom;
        _quad.tl.texCoords.u = right;
        _quad.tl.texCoords.v = top;
        _quad.tr.texCoords.u = right;
        _quad.tr.texCoords.v = bottom;
    }
    else
    {
        left    = rect.origin.x/atlasWidth;
        right    = (rect.origin.x + rect.size.width) / atlasWidth;
        top        = rect.origin.y/atlasHeight;
        bottom    = (rect.origin.y + rect.size.height) / atlasHeight;

        if(_flippedX)
        {
            std::swap(left, right);
        }

        if(_flippedY)
        {
            std::swap(top, bottom);
        }

        _quad.bl.texCoords.u = left;
        _quad.bl.texCoords.v = bottom;
        _quad.br.texCoords.u = right;
        _quad.br.texCoords.v = bottom;
        _quad.tl.texCoords.u = left;
        _quad.tl.texCoords.v = top;
        _quad.tr.texCoords.u = right;
        _quad.tr.texCoords.v = top;
    }
}

// draw

void CHmsBackgroundNode::Draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
#if HMS_USE_CULLING
    // Don't do calculate the culling if the transform was not updated
    auto visitingCamera = Camera::getVisitingCamera();
    auto defaultCamera = Camera::getDefaultCamera();
    if (visitingCamera == defaultCamera) {
        _insideBounds = ((flags & FLAGS_TRANSFORM_DIRTY)|| visitingCamera->isViewProjectionUpdated()) ? renderer->checkVisibility(transform, m_sizeContent) : _insideBounds;
    }
    else
    {
        _insideBounds = renderer->checkVisibility(transform, m_sizeContent);
    }

    if(_insideBounds)
#endif
    {
		_trianglesCommand.init(m_globalZOrder, _texture->getName(), GetGLProgramState(), _blendFunc, m_triangles, transform, flags);
        renderer->addCommand(&_trianglesCommand);
    }
}

// MARK: visit, draw, transform

void CHmsBackgroundNode::AddChild(CHmsNode *child, int zOrder, int tag)
{
    CHmsNode::AddChild(child, zOrder, tag);
}

void CHmsBackgroundNode::ReorderChild(CHmsNode *child, int zOrder)
{
    HMSASSERT(child != nullptr, "child must be non null");
    HMSASSERT(_children.contains(child), "child does not belong to this");

    if(!m_reorderChildDirty)
    {
        SetReorderChildDirtyRecursively();
    }

    CHmsNode::ReorderChild(child, zOrder);
}

void CHmsBackgroundNode::RemoveChild(CHmsNode *child, bool cleanup)
{
    CHmsNode::RemoveChild(child, cleanup);
}

void CHmsBackgroundNode::RemoveAllChildrenWithCleanup(bool cleanup)
{
    CHmsNode::RemoveAllChildrenWithCleanup(cleanup);
}

void CHmsBackgroundNode::SortAllChildren()
{
	CHmsNode::SortAllChildren();
}

//
// CHmsNode property overloads
// used only when parent is CHmsImageNodeBatchCHmsNode
//

void CHmsBackgroundNode::SetReorderChildDirtyRecursively(void)
{
    //only set parents flag the first time
    if ( ! m_reorderChildDirty )
    {
		m_reorderChildDirty = true;
        CHmsNode* node = static_cast<CHmsNode*>(m_pParent);
		while (node)
        {
			static_cast<CHmsBackgroundNode*>(node)->SetReorderChildDirtyRecursively();
			node = node->GetParent();
        }
    }
}

void CHmsBackgroundNode::SetDirtyRecursively(bool bValue)
{
    _recursiveDirty = bValue;
    SetDirty(bValue);

    for(const auto &child: m_vChild) {
        CHmsBackgroundNode* sp = dynamic_cast<CHmsBackgroundNode*>(child);
        if (sp)
        {
            sp->SetDirtyRecursively(true);
        }
    }
}

// FIXME: HACK: optimization
#define SET_DIRTY_RECURSIVELY() {                       \
                    if (! _recursiveDirty) {            \
                        _recursiveDirty = true;         \
                        SetDirty(true);                 \
                        if (!m_vChild.empty())         \
                            SetDirtyRecursively(true);  \
                        }                               \
                    }

void CHmsBackgroundNode::SetPosition(const Vec2& pos)
{
    CHmsNode::SetPosition(pos);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetPosition(float x, float y)
{
    CHmsNode::SetPosition(x, y);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetRotation(float rotation)
{
    CHmsNode::SetRotation(rotation);
    
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetScaleX(float scaleX)
{
    CHmsNode::SetScaleX(scaleX);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetScaleY(float scaleY)
{
    CHmsNode::SetScaleY(scaleY);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetScale(float fScale)
{
    CHmsNode::SetScale(fScale);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetScale(float scaleX, float scaleY)
{
    CHmsNode::SetScale(scaleX, scaleY);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetPositionZ(float fVertexZ)
{
    CHmsNode::SetPositionZ(fVertexZ);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetAnchorPoint(const Vec2& anchor)
{
    CHmsNode::SetAnchorPoint(anchor);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::IgnoreAnchorPointForPosition(bool value)
{
	CHmsNode::IgnoreAnchorPointForPosition(value);
}

void CHmsBackgroundNode::SetVisible(bool bVisible)
{
    CHmsNode::SetVisible(bVisible);
    SET_DIRTY_RECURSIVELY();
}

void CHmsBackgroundNode::SetFlippedX(bool flippedX)
{
    if (_flippedX != flippedX)
    {
        _flippedX = flippedX;
        for (ssize_t i = 0; i < m_triangles.vertCount; i++) {
			auto& v = m_triangles.verts[i].vertices;
            v.x = m_sizeContent.width -v.x;
        }
    }
}

bool CHmsBackgroundNode::IsFlippedX(void) const
{
    return _flippedX;
}

void CHmsBackgroundNode::SetFlippedY(bool flippedY)
{
    if (_flippedY != flippedY)
    {
        _flippedY = flippedY;
		for (ssize_t i = 0; i < m_triangles.vertCount; i++) {
			auto& v = m_triangles.verts[i].vertices;
            v.y = m_sizeContent.height -v.y;
        }
    }
}

bool CHmsBackgroundNode::IsFlippedY(void) const
{
    return _flippedY;
}

// MARK: Texture protocol

void CHmsBackgroundNode::UpdateBlendFunc(void)
{
    // it is possible to have an untextured imageNode
    if (! _texture || ! _texture->hasPremultipliedAlpha())
    {
        _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
		SetOpacityModifyRGB(false);
    }
    else
    {
        _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
		SetOpacityModifyRGB(true);
    }
}

std::string CHmsBackgroundNode::GetDescription() const
{
    return "CHmsImageNode";
}

void CHmsBackgroundNode::UpdateColor()
{
	Color4B color4(m_colorDisplay.r, m_colorDisplay.g, m_colorDisplay.b, m_opacityDisplay);

	// special opacity for premultiplied textures
	if (_opacityModifyRGB)
	{
		color4.r *= m_opacityDisplay / 255.0f;
		color4.g *= m_opacityDisplay / 255.0f;
		color4.b *= m_opacityDisplay / 255.0f;
	}

	for (ssize_t i = 0; i < m_triangles.vertCount; i++) {
		m_triangles.verts[i].colors = color4;
	}
}

void CHmsBackgroundNode::SetOpacityModifyRGB(bool modify)
{
	if (_opacityModifyRGB != modify)
	{
		_opacityModifyRGB = modify;
		UpdateColor();
	}
}

bool CHmsBackgroundNode::IsOpacityModifyRGB() const
{
	return _opacityModifyRGB;
}





NS_HMS_END
