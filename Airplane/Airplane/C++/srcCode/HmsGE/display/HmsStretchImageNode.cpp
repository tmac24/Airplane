#include "display/HmsStretchImageNode.h"
#include <algorithm>
#include "render/HmsTextureCache.h"
#include "render/HmsTexture2D.h"
#include "render/HmsRenderer.h"
#include "HmsAviDisplayUnit.h"
#include "base/HmsCamera.h"

#define  HMS_USE_CULLING 1

NS_HMS_BEGIN

//  0---1---2---3
//  |   |   |   |
//  4---5---6---7
//  |   |   |   |
//  8---9---10--11
//  |   |   |   |
// 12---13--14--15
unsigned short CHmsStretchImageNode::s_quadIndices[] = { 0, 1, 4,   1, 4, 5,    1, 2, 5,    2, 5, 6,     2, 3, 6,     3, 6, 7
												 , 4, 5, 8,   5, 8, 9,    5, 6, 9,    6, 9, 10,    6, 7, 10,    7, 10, 11
												 , 8, 9, 12,  9, 12, 13,  9, 10, 13,  10, 13, 14,  10, 11, 14,  11, 14,15};

// MARK: create, init, dealloc
CHmsStretchImageNode* CHmsStretchImageNode::CreateWithTexture(Texture2D *texture)
{
	CHmsStretchImageNode *imageNode = new STD_NOTHROW CHmsStretchImageNode();
    if (imageNode && imageNode->InitWithTexture(texture))
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsStretchImageNode* CHmsStretchImageNode::CreateWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
{
	CHmsStretchImageNode *imageNode = new STD_NOTHROW CHmsStretchImageNode();
    if (imageNode && imageNode->InitWithTexture(texture, rect, rotated))
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsStretchImageNode* CHmsStretchImageNode::Create(const std::string& filename)
{
	CHmsStretchImageNode *imageNode = new STD_NOTHROW CHmsStretchImageNode();
    if (imageNode && imageNode->InitWithFile(filename))
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsStretchImageNode* CHmsStretchImageNode::Create(const std::string& filename, const Rect& rect)
{
	CHmsStretchImageNode *imageNode = new STD_NOTHROW CHmsStretchImageNode();
    if (imageNode && imageNode->InitWithFile(filename, rect))
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsStretchImageNode* CHmsStretchImageNode::Create()
{
	CHmsStretchImageNode *imageNode = new STD_NOTHROW CHmsStretchImageNode();
    if (imageNode && imageNode->Init())
    {
        imageNode->autorelease();
        return imageNode;
    }
    HMS_SAFE_DELETE(imageNode);
    return nullptr;
}

CHmsStretchImageNode* CHmsStretchImageNode::Create(const std::string& filename, const Size& assignedSize)
{
	return Create(filename, Rect(Vec2(0, 0), assignedSize));
}

CHmsStretchImageNode* CHmsStretchImageNode::Create(const std::string& filename, float assignedWidth, float assignedHeight)
{
	return Create(filename, Rect(0, 0, assignedWidth, assignedHeight));
}



bool CHmsStretchImageNode::Init(void)
{
    return InitWithTexture(nullptr, Rect::ZERO );
}

bool CHmsStretchImageNode::InitWithTexture(Texture2D *texture)
{
    HMSASSERT(texture != nullptr, "Invalid texture for imageNode");

    Rect rect = Rect::ZERO;
	rect.size = texture->getContentSize();

    return InitWithTexture(texture, rect);
}

bool CHmsStretchImageNode::InitWithTexture(Texture2D *texture, const Rect& rect)
{
    return InitWithTexture(texture, rect, false);
}

bool CHmsStretchImageNode::InitWithFile(const std::string& filename)
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

bool CHmsStretchImageNode::InitWithFile(const std::string &filename, const Rect& rect)
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
bool CHmsStretchImageNode::InitWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
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
        memset(&_verts, 0, sizeof(_verts));
        
        // Atlas: Color, by default the color is white
		Color4B colorInit(m_colorDisplay);
		for (int index = 0; index < m_triangles.vertCount; index++)
		{
			_verts[index].colors = colorInit;
		}

        // shader state
        SetGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));

        // update texture (calls updateBlendFunc)
        SetTexture(texture);
		
		{
			Rect rectTexture = Rect::ZERO;
			rectTexture.size = texture->getContentSize();
			SetTextureRect(rectTexture, rotated, rect.size);
		}
        
        
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

CHmsStretchImageNode::CHmsStretchImageNode()
	: _shouldBeHidden(false)
	, _texture(nullptr)
	, _insideBounds(true)
{
	m_triangles.indexCount = 54;
	m_triangles.vertCount = 16;
	m_triangles.verts = _verts;
	m_triangles.indices = s_quadIndices;
}

CHmsStretchImageNode::~CHmsStretchImageNode(void)
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
void CHmsStretchImageNode::SetTexture(const std::string &filename)
{
	Texture2D *texture = CHmsAviDisplayUnit::GetInstance()->GetTextureCache()->addImage(filename);
    SetTexture(texture);

    Rect rect = Rect::ZERO;
    if (texture)
		rect.size = texture->getContentSize();
    SetTextureRect(rect);
}

void CHmsStretchImageNode::SetTexture(Texture2D *texture)
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

Texture2D* CHmsStretchImageNode::GetTexture() const
{
    return _texture;
}

void CHmsStretchImageNode::SetTextureRect(const Rect& rect)
{
    SetTextureRect(rect, false, rect.size);
}

void CHmsStretchImageNode::SetTextureRect(const Rect& rect, bool rotated, const Size& assignedSize)
{
    _rectRotated = rotated;

	SetContentSize(assignedSize);
    SetTextureOrgRect(rect);
    SetTextureCoords(rect);
	UpdateVertsPos(assignedSize);
}

// override this method to generate "double scale" imageNodes
void CHmsStretchImageNode::SetTextureOrgRect(const Rect& rect)
{
    _rect = rect;
}

void CHmsStretchImageNode::SetTextureCoords(Rect rect)
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

//         _verts.bl.texCoords.u = left;
//         _verts.bl.texCoords.v = top;
//         _verts.br.texCoords.u = left;
//         _verts.br.texCoords.v = bottom;
//         _verts.tl.texCoords.u = right;
//         _verts.tl.texCoords.v = top;
//         _verts.tr.texCoords.u = right;
//         _verts.tr.texCoords.v = bottom;
    }
    else
    {
//         left    = rect.origin.x/atlasWidth;
//         right    = (rect.origin.x + rect.size.width) / atlasWidth;
//         top        = rect.origin.y/atlasHeight;
//         bottom    = (rect.origin.y + rect.size.height) / atlasHeight;

//         if(_flippedX)
//         {
//             std::swap(left, right);
//         }
// 
//         if(_flippedY)
//         {
//             std::swap(top, bottom);
//         }


		float qtrU = _rect.size.height / 3.0f / _rect.size.height;
		float qtrV = _rect.size.width / 3.0f / rect.size.width;
		float row0V = 0.0;
		float row1V = row0V + qtrV;
		float row2V = row1V + qtrV;
		float row3V = 1.0;
		float col0U = 0.0;
		float col1U = col0U + qtrU;
		float col2U = col1U + qtrU;
		float col3U = 1.0;

		_verts[0].texCoords.u = col0U;
		_verts[1].texCoords.u = col1U;
		_verts[2].texCoords.u = col2U;
		_verts[3].texCoords.u = col3U;
		_verts[4].texCoords.u = col0U;
		_verts[5].texCoords.u = col1U;
		_verts[6].texCoords.u = col2U;
		_verts[7].texCoords.u = col3U;
		_verts[8].texCoords.u = col0U;
		_verts[9].texCoords.u = col1U;
		_verts[10].texCoords.u = col2U;
		_verts[11].texCoords.u = col3U;
		_verts[12].texCoords.u = col0U;
		_verts[13].texCoords.u = col1U;
		_verts[14].texCoords.u = col2U;
		_verts[15].texCoords.u = col3U;

		_verts[0].texCoords.v = row0V;
		_verts[1].texCoords.v = row0V;
		_verts[2].texCoords.v = row0V;
		_verts[3].texCoords.v = row0V;
		_verts[4].texCoords.v = row1V;
		_verts[5].texCoords.v = row1V;
		_verts[6].texCoords.v = row1V;
		_verts[7].texCoords.v = row1V;
		_verts[8].texCoords.v = row2V;
		_verts[9].texCoords.v = row2V;
		_verts[10].texCoords.v = row2V;
		_verts[11].texCoords.v = row2V;
		_verts[12].texCoords.v = row3V;
		_verts[13].texCoords.v = row3V;
		_verts[14].texCoords.v = row3V;
		_verts[15].texCoords.v = row3V;
    }
}

// draw

void CHmsStretchImageNode::Draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
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
		//UpdateColor();//add line by wzh
		_trianglesCommand.init(m_globalZOrder, _texture->getName(), GetGLProgramState(), _blendFunc, m_triangles, transform, flags);
        renderer->addCommand(&_trianglesCommand);
    }
}

// MARK: visit, draw, transform

void CHmsStretchImageNode::AddChild(CHmsNode *child, int zOrder, int tag)
{
    CHmsNode::AddChild(child, zOrder, tag);
}

void CHmsStretchImageNode::ReorderChild(CHmsNode *child, int zOrder)
{
    HMSASSERT(child != nullptr, "child must be non null");
    HMSASSERT(_children.contains(child), "child does not belong to this");

    if(!m_reorderChildDirty)
    {
        SetReorderChildDirtyRecursively();
    }

    CHmsNode::ReorderChild(child, zOrder);
}

void CHmsStretchImageNode::RemoveChild(CHmsNode *child, bool cleanup)
{
    CHmsNode::RemoveChild(child, cleanup);
}

void CHmsStretchImageNode::RemoveAllChildrenWithCleanup(bool cleanup)
{
    CHmsNode::RemoveAllChildrenWithCleanup(cleanup);
}

void CHmsStretchImageNode::SortAllChildren()
{
	CHmsNode::SortAllChildren();
}

//
// CHmsNode property overloads
// used only when parent is CHmsImageNodeBatchCHmsNode
//

void CHmsStretchImageNode::SetReorderChildDirtyRecursively(void)
{
    //only set parents flag the first time
    if ( ! m_reorderChildDirty )
    {
		m_reorderChildDirty = true;
        CHmsNode* node = static_cast<CHmsNode*>(m_pParent);
		while (node)
        {
			static_cast<CHmsStretchImageNode*>(node)->SetReorderChildDirtyRecursively();
			node = node->GetParent();
        }
    }
}

void CHmsStretchImageNode::SetDirtyRecursively(bool bValue)
{
    _recursiveDirty = bValue;
    SetDirty(bValue);

    for(const auto &child: m_vChild) {
		CHmsStretchImageNode* sp = dynamic_cast<CHmsStretchImageNode*>(child);
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

void CHmsStretchImageNode::SetPosition(const Vec2& pos)
{
    CHmsNode::SetPosition(pos);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetPosition(float x, float y)
{
    CHmsNode::SetPosition(x, y);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetRotation(float rotation)
{
    CHmsNode::SetRotation(rotation);
    
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetScaleX(float scaleX)
{
    CHmsNode::SetScaleX(scaleX);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetScaleY(float scaleY)
{
    CHmsNode::SetScaleY(scaleY);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetScale(float fScale)
{
    CHmsNode::SetScale(fScale);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetScale(float scaleX, float scaleY)
{
    CHmsNode::SetScale(scaleX, scaleY);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetPositionZ(float fVertexZ)
{
    CHmsNode::SetPositionZ(fVertexZ);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetAnchorPoint(const Vec2& anchor)
{
    CHmsNode::SetAnchorPoint(anchor);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::IgnoreAnchorPointForPosition(bool value)
{
	CHmsNode::IgnoreAnchorPointForPosition(value);
}

void CHmsStretchImageNode::SetVisible(bool bVisible)
{
    CHmsNode::SetVisible(bVisible);
    SET_DIRTY_RECURSIVELY();
}

void CHmsStretchImageNode::SetFlippedX(bool flippedX)
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

bool CHmsStretchImageNode::IsFlippedX(void) const
{
    return _flippedX;
}

void CHmsStretchImageNode::SetFlippedY(bool flippedY)
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

bool CHmsStretchImageNode::IsFlippedY(void) const
{
    return _flippedY;
}

// MARK: Texture protocol

void CHmsStretchImageNode::UpdateBlendFunc(void)
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

std::string CHmsStretchImageNode::GetDescription() const
{
    return "CHms9ImageNode";
}

void CHmsStretchImageNode::UpdateColor()
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

void CHmsStretchImageNode::SetOpacityModifyRGB(bool modify)
{
	if (_opacityModifyRGB != modify)
	{
		_opacityModifyRGB = modify;
		UpdateColor();
	}
}

bool CHmsStretchImageNode::IsOpacityModifyRGB() const
{
	return _opacityModifyRGB;
}

void CHmsStretchImageNode::SetContentSize(const Size &size)
{
	UpdateVertsPos(size);
	CHmsNode::SetContentSize(size);
}

void CHmsStretchImageNode::UpdateVertsPos(const Size & size)
{
	// self rendering

	// Atlas: Vertex
	float qtrHieght = _rect.size.height / 3.0f;
	float qtrWidth = _rect.size.width / 3.0f;
	float row0 = 0.0f;
	float row1 = row0 + qtrHieght;
	float row3 = row0 + size.height;
	float row2 = row3 - qtrHieght;

	float col0 = 0.0f;
	float col1 = col0 + qtrWidth;
	float col3 = col0 + size.width;
	float col2 = col3 - qtrWidth;


	// Don't update Z.
	_verts[0].vertices.set(col0, row3, 0.0f);
	_verts[1].vertices.set(col1, row3, 0.0f);
	_verts[2].vertices.set(col2, row3, 0.0f);
	_verts[3].vertices.set(col3, row3, 0.0f);

	_verts[4].vertices.set(col0, row2, 0.0f);
	_verts[5].vertices.set(col1, row2, 0.0f);
	_verts[6].vertices.set(col2, row2, 0.0f);
	_verts[7].vertices.set(col3, row2, 0.0f);

	_verts[8].vertices.set(col0, row1, 0.0f);
	_verts[9].vertices.set(col1, row1, 0.0f);
	_verts[10].vertices.set(col2, row1, 0.0f);
	_verts[11].vertices.set(col3, row1, 0.0f);

	_verts[12].vertices.set(col0, row0, 0.0f);
	_verts[13].vertices.set(col1, row0, 0.0f);
	_verts[14].vertices.set(col2, row0, 0.0f);
	_verts[15].vertices.set(col3, row0, 0.0f);

}







NS_HMS_END
