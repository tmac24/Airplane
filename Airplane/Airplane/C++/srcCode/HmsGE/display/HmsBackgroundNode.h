
#ifndef __HMS_BACKGROUND_NODE_H__
#define __HMS_BACKGROUND_NODE_H__

#include <string>
// #include "2d/CCCHmsNode.h"
// #include "2d/CCDrawCHmsNode.h"
// #include "base/CCProtocols.h"
// #include "renderer/CCTextureAtlas.h"
// #include "renderer/CCTrianglesCommand.h"
// #include "renderer/CCCustomCommand.h"
// #include "2d/CCAutoPolygon.h"
#include "base/HmsNode.h"
#include "render/HmsTextureAtlas.h"
#include "render/HmsTrianglesCommand.h"
#include "render/HmsCustomCommand.h"

NS_HMS_BEGIN

class SpriteBatchNode;
class SpriteFrame;
class Animation;
class Rect;
class Size;
class Texture2D;
struct transformValues_;

#ifdef SPRITE_RENDER_IN_SUBPIXEL
#undef SPRITE_RENDER_IN_SUBPIXEL
#endif

#if 0
#define HMS_IMAGE_RENDER_IN_SUBPIXEL
#else
#define HMS_IMAGE_RENDER_IN_SUBPIXEL(__ARGS__) (ceil(__ARGS__))
#endif

class HMS_DLL CHmsBackgroundNode : public CHmsNode
{
public:
     /** Sprite invalid index on the SpriteBatchCHmsNode. */
    static const int INDEX_NOT_INITIALIZED = -1;

	/**
	* Creates an empty Image CHmsNode without texture. You can call setTexture method subsequently.
	*
	* @return An auto released Image CHmsNode object.
	*/
    static CHmsBackgroundNode* Create();

	/**
	* Creates a sprite with an image filename.
	*
	* After creation, the rect of Image CHmsNode will be the size of the image,
	* and the offset will be (0,0).
	*
	* @param   filename A path to image file, e.g., "scene1/monster.png".
	* @return  An auto released Image CHmsNode object.
	*/
	static CHmsBackgroundNode* Create(const std::string& filename);

	/**
	* Creates a sprite with an image filename and a rect.
	*
	* @param   filename A path to image file, e.g., "scene1/monster.png".
	* @param   rect     A subrect of the image file.
	* @return  An auto released Image CHmsNode object.
	*/
	static CHmsBackgroundNode* Create(const std::string& filename, const Rect& rect);

	/**
	* Creates a Image CHmsNode with a Texture2D object.
	*
	* After creation, the rect will be the size of the texture, and the offset will be (0,0).
	*
	* @param   texture    A pointer to a Texture2D object.
	* @return  An auto released Image CHmsNode object.
	*/
	static CHmsBackgroundNode* CreateWithTexture(Texture2D *texture);

	/**
	* Creates a Image CHmsNode with a texture and a rect.
	*
	* After creation, the offset will be (0,0).
	*
	* @param   texture     A pointer to an existing Texture2D object.
	*                      You can use a Texture2D object for many sprites.
	* @param   rect        Only the contents inside the rect of this texture will be applied for this sprite.
	* @param   rotated     Whether or not the rect is rotated.
	* @return  An auto released Image CHmsNode object.
	*/
	static CHmsBackgroundNode* CreateWithTexture(Texture2D *texture, const Rect& rect, bool rotated = false);

	/**
	* Creates a sprite with an sprite frame.
	*
	* @param   spriteFrame    A sprite frame which involves a texture and a rect.
	* @return  An auto released sprite object.
	*/
	static CHmsBackgroundNode* CreateWithSpriteFrame(SpriteFrame *spriteFrame);

	/**
	* Creates a sprite with an sprite frame name.
	*
	* A SpriteFrame will be fetched from the SpriteFrameCache by spriteFrameName param.
	* If the SpriteFrame doesn't exist it will raise an exception.
	*
	* @param   spriteFrameName A null terminated string which indicates the sprite frame name.
	* @return  An auto released sprite object.
	*/
	static CHmsBackgroundNode* CreateWithSpriteFrameName(const std::string& spriteFrameName);


	/// @{
	/// @name Texture / Frame methods

	/** Sets a new texture (from a filename) to the sprite.
	*
	*  @memberof Sprite
	*  It will call `setTextureRect()` with the texture's content size.
	*/
	virtual void SetTexture(const std::string &filename);

	/** @overload
	*
	*  The Texture's rect is not changed.
	*/
	virtual void SetTexture(Texture2D *texture);

	/** Returns the Texture2D object used by the sprite. */
	virtual Texture2D* GetTexture() const;

	/**
	* Updates the texture rect of the Sprite in points.
	*
	* It will call setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize) with \p rotated = false, and \p utrimmedSize = rect.size.
	*/
	virtual void SetTextureRect(const Rect& rect);

	/** @overload
	*
	* It will update the texture coordinates and the vertex rectangle.
	*/
	virtual void SetTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize);

	/**
	* Sets the vertex rect.
	*
	* It will be called internally by setTextureRect.
	* Useful if you want to create 2x images from SD images in Retina Display.
	* Do not call it manually. Use setTextureRect instead.
	*/
	virtual void SetVertexRect(const Rect& rect);

	/// @{
	/// @name Sprite Properties' setter/getters.

	/**
	* Whether or not the Sprite needs to be updated in the Atlas.
	*
	* @return True if the sprite needs to be updated in the Atlas, false otherwise.
	*/
	virtual bool IsDirty() const { return _dirty; }

	/**
	* Makes the Sprite to be updated in the Atlas.
	*/
	virtual void SetDirty(bool dirty) { _dirty = dirty; }

	/**
	* Returns the quad (tex coords, vertex coords and color) information.
	*/
	inline V3F_C4B_T2F_Quad GetQuad() const { return _quad; }

	/**
	* Returns whether or not the texture rectangle is rotated.
	*/
	inline bool IsTextureRectRotated() const { return _rectRotated; }

	/**
	* Returns the rect of the Sprite in points.
	*/
	inline const Rect& GetTextureRect() const { return _rect; }


	/**
	* Returns the flag which indicates whether the sprite is flipped horizontally or not.
	*
	* It only flips the texture of the sprite, and not the texture of the sprite's children.
	* Also, flipping the texture doesn't alter the anchorPoint.
	* If you want to flip the anchorPoint too, and/or to flip the children too use:
	* sprite->SetScaleX(sprite->getScaleX() * -1);
	*
	* @return true if the sprite is flipped horizontally, false otherwise.
	*/
	bool IsFlippedX() const;
	/**
	* Sets whether the sprite should be flipped horizontally or not.
	*
	* @param flippedX true if the sprite should be flipped horizontally, false otherwise.
	*/
	void SetFlippedX(bool flippedX);

	/**
	* Return the flag which indicates whether the sprite is flipped vertically or not.
	*
	* It only flips the texture of the sprite, and not the texture of the sprite's children.
	* Also, flipping the texture doesn't alter the anchorPoint.
	* If you want to flip the anchorPoint too, and/or to flip the children too use:
	* sprite->SetScaleY(sprite->getScaleY() * -1);
	*
	* @return true if the sprite is flipped vertically, false otherwise.
	*/
	bool IsFlippedY() const;
	/**
	* Sets whether the sprite should be flipped vertically or not.
	*
	* @param flippedY true if the sprite should be flipped vertically, false otherwise.
	*/
	void SetFlippedY(bool flippedY);


	inline void SetBlendFunc(const BlendFunc &blendFunc) { _blendFunc = blendFunc; }
	inline const BlendFunc& GetBlendFunc() const { return _blendFunc; }

	virtual std::string GetDescription() const;


	virtual void SetScaleX(float scaleX) override;
	virtual void SetScaleY(float scaleY) override;
	virtual void SetScale(float scaleX, float scaleY) override;
	virtual void SetPosition(const Vec2& pos) override;
	virtual void SetPosition(float x, float y) override;
	virtual void SetRotation(float rotation) override;

	virtual void RemoveChild(CHmsNode* child, bool cleanup) override;
	virtual void RemoveAllChildrenWithCleanup(bool cleanup) override;
	virtual void ReorderChild(CHmsNode *child, int zOrder) override;

	using CHmsNode::AddChild;
	virtual void AddChild(CHmsNode *child, int zOrder, int tag) override;
	virtual void SortAllChildren() override;
	virtual void SetScale(float scale) override;
	virtual void SetPositionZ(float positionZ) override;
	virtual void SetAnchorPoint(const Vec2& anchor) override;
	virtual void IgnoreAnchorPointForPosition(bool value) override;
	virtual void SetVisible(bool bVisible) override;
	virtual void Draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
	/// @}

public:
	CHmsBackgroundNode();
	virtual ~CHmsBackgroundNode();

	/* Initializes an empty sprite with nothing init. */
	virtual bool Init() override;

	/**
	* Initializes a sprite with a texture.
	*
	* After initialization, the rect used will be the size of the texture, and the offset will be (0,0).
	*
	* @param   texture    A pointer to an existing Texture2D object.
	*                      You can use a Texture2D object for many sprites.
	* @return  True if the sprite is initialized properly, false otherwise.
	*/
	virtual bool InitWithTexture(Texture2D *texture);

	/**
	* Initializes a sprite with a texture and a rect.
	*
	* After initialization, the offset will be (0,0).
	*
	* @param   texture    A pointer to an existing Texture2D object.
	*                      You can use a Texture2D object for many sprites.
	* @param   rect        Only the contents inside rect of this texture will be applied for this sprite.
	* @return  True if the sprite is initialized properly, false otherwise.
	*/
	virtual bool InitWithTexture(Texture2D *texture, const Rect& rect);

	/**
	* Initializes a sprite with a texture and a rect in points, optionally rotated.
	*
	* After initialization, the offset will be (0,0).
	* @note    This is the designated initializer.
	*
	* @param   texture    A Texture2D object whose texture will be applied to this sprite.
	* @param   rect        A rectangle assigned the contents of texture.
	* @param   rotated     Whether or not the texture rectangle is rotated.
	* @return  True if the sprite is initialized properly, false otherwise.
	*/
	virtual bool InitWithTexture(Texture2D *texture, const Rect& rect, bool rotated);

	/**
	* Initializes a sprite with an image filename.
	*
	* This method will find filename from local file system, load its content to Texture2D,
	* then use Texture2D to create a sprite.
	* After initialization, the rect used will be the size of the image. The offset will be (0,0).
	*
	* @param   filename The path to an image file in local file system.
	* @return  True if the sprite is initialized properly, false otherwise.
	* @lua     init
	*/
	virtual bool InitWithFile(const std::string& filename);

	/**
	* Initializes a sprite with an image filename, and a rect.
	*
	* This method will find filename from local file system, load its content to Texture2D,
	* then use Texture2D to create a sprite.
	* After initialization, the offset will be (0,0).
	*
	* @param   filename The path to an image file in local file system.
	* @param   rect        The rectangle assigned the content area from texture.
	* @return  True if the sprite is initialized properly, false otherwise.
	* @lua     init
	*/
	virtual bool InitWithFile(const std::string& filename, const Rect& rect);

protected:

	void UpdateColor() override;
	virtual void SetTextureCoords(Rect rect);
	virtual void UpdateBlendFunc();
	virtual void SetReorderChildDirtyRecursively();
	virtual void SetDirtyRecursively(bool value);
	virtual void SetOpacityModifyRGB(bool modify);
	virtual bool IsOpacityModifyRGB() const;

	bool                _dirty;             /// Whether the sprite needs to be updated
	bool                _recursiveDirty;    /// Whether all of the sprite's children needs to be updated
	bool                _shouldBeHidden;    /// should not be drawn because one of the ancestors is not visible

	//
	// Data used when the sprite is self-rendered
	//
	BlendFunc        _blendFunc;            /// It's required for TextureProtocol inheritance
	Texture2D*       _texture;              /// Texture2D object that is used to render the sprite
	SpriteFrame*     _spriteFrame;
	TrianglesCommand _trianglesCommand;     ///

	// texture
	Rect _rect;                             /// Rectangle of Texture2D
	bool   _rectRotated;                    /// Whether the texture is rotated

	// vertex coords, texture coords and color info
	V3F_C4B_T2F_Quad _quad;
	TrianglesCommand::Triangles m_triangles;

	// opacity and RGB protocol
	bool _opacityModifyRGB;

	// image is flipped
	bool _flippedX;                         /// Whether the sprite is flipped horizontally or not
	bool _flippedY;                         /// Whether the sprite is flipped vertically or not

	bool _insideBounds;                     /// whether or not the sprite was inside bounds the previous frame
private:
	HMS_DISALLOW_COPY_AND_ASSIGN(CHmsBackgroundNode);

	static unsigned short s_quadIndices[];
};

NS_HMS_END

#endif // __HMS_BACKGROUND_NODE_H__
