#pragma once
#include "HmsAviMacros.h"
#include "HmsStdC.h"
#include "base/Ref.h"
#include "base/HmsVector.h"
#include "base/HmsTypes.h"
#include "base/HmsTouch.h"
#include "math/HmsMath.h"
#include "math/AffineTransform.h"
#include "math/TransformUtils.h"

NS_HMS_BEGIN

class Renderer;
class GLProgram;
class GLProgramState;
class CHmsScreen;
class CHmsNode;
class CHmsAviDisplayUnit;
class CHmsAction;

bool HMS_DLL HmsNodeComparisonLess(CHmsNode* n1, CHmsNode* n2);

class HMS_DLL CHmsNode : public Ref
{
public:
	/** Default tag used for all the nodes */
	static const int INVALID_TAG = -1;

	enum {
		FLAGS_TRANSFORM_DIRTY = (1 << 0),
		FLAGS_CONTENT_SIZE_DIRTY = (1 << 1),
		FLAGS_RENDER_AS_3D = (1 << 3),

		FLAGS_DIRTY_MASK = (FLAGS_TRANSFORM_DIRTY | FLAGS_CONTENT_SIZE_DIRTY),
	};

	CHmsNode();
	virtual ~CHmsNode();

	CREATE_FUNC(CHmsNode);

	/**
	*override this method to draw you own node.
	*param   renderer				a given renderer
	*param	 parentTransform		a transform matrix
	*param	 parentFlags			renderer flag
	*returns
	*by [3/27/2017 song.zhang079]
	*/
	virtual void Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags);
	virtual void Draw() final;

	/**
	*visits this node's children and Draw them recursively.
	*param   renderer				a given renderer
	*param	 parentTransform		a transform matrix
	*param	 parentFlags			renderer flag
	*returns 
	*by [3/27/2017 song.zhang079] 
	*/
	virtual void Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags);
	virtual void Visit() final;

	virtual Vector<CHmsNode*>& GetChildren(){ return m_vChild; }
	virtual const Vector<CHmsNode*>& GetChildren() const{ return m_vChild; }
	virtual CHmsNode * GetChildByTag(int nTag) const;

	CHmsNode * GetChildByName(const std::string & strName) const;

	virtual ssize_t GetChildrenCount() const;

	virtual void SetParent(CHmsNode * parent);

	virtual CHmsNode * GetParent(){ return m_pParent; }
	virtual const CHmsNode * GetParent() const { return m_pParent; }

	virtual void SetAnchorPoint(const Vec2 & anchorPoint);
	virtual void SetAnchorPoint(float xOffset, float yOffset);
	virtual const Vec2 & GetAnchorPoint() const;

	virtual void SetContentSize(const Size & size);
	virtual Size GetContentSize() const;


	virtual void AddChild(CHmsNode * child);
	virtual void AddChild(CHmsNode * child, int localZOrder);
	virtual void AddChild(CHmsNode * child, int localZOrder, int tag);

	virtual void RemoveFromParent();
	virtual void RemoveFromParentAndCleanup(bool bCleanup);
	virtual void RemoveChild(CHmsNode * child, bool bCleanup = true);
	virtual void RemoveChildByTag(int tag, bool bCleanup = true);
	virtual void RemoveChildByName(const std::string & strName, bool bCleanup = true);
	virtual void RemoveAllChildren();
	virtual void RemoveAllChildrenWithCleanup(bool bCleanup);
	virtual void SortAllChildren();
	/**
	* Reorders a child according to a new z value.
	*
	*param child     An already added child node. It MUST be already added.
	*param localZOrder Z order for drawing priority. Please refer to setLocalZOrder(int).
	*/
	//virtual void ReorderChild(CHmsNode * child, int localZOrder);

	virtual void SetLocalZOrder(int localZOrder);
	virtual int GetLocalZOrder() const { return m_localZOrder; }

	virtual void SetGlobalZOrder(float globalZOrder);
	virtual float GetGlobalZOrder() const { return m_globalZOrder; }

	virtual int GetTag() const;
	virtual void SetTag(int tag);

	void SetName(const std::string & strName){ m_strName = strName; }
	std::string GetName(){ return m_strName; }

	virtual void * GetUserData(){ return m_pUserData; }
	virtual const void * GetUserData() const { return m_pUserData; }
	virtual void SetUserData(void * userData);

	virtual Ref * GetUserObject(){ return m_pUserObject; }
	virtual const Ref * GetUserObject() const { return m_pUserObject; }
	virtual void SetUserObject(Ref * userObject);

	GLProgram * GetGLProgram() const;
	virtual void SetGLProgram(GLProgram * glProgram);
	GLProgramState * GetGLProgramState() const;
	virtual void SetGLProgramState(GLProgramState * glProgramState);

	virtual bool Init();

	virtual void OnEnter();
	virtual void OnExit();

	virtual void Cleanup();

	/**
	* Set rotation by quaternion. You should make sure the quaternion is normalized.
	*
	*param quat The rotation in quaternion, note that the quat must be normalized.
	*/
	virtual void SetRotationQuat(const Quaternion& quat);

	/**
	* Return the rotation by quaternion, Note that when _rotationZ_X == _rotationZ_Y, the returned quaternion equals to RotationZ_X * RotationY * RotationX,
	* it equals to RotationY * RotationX otherwise.
	*
	*return The rotation in quaternion.
	*/
	virtual Quaternion GetRotationQuat() const;

	virtual void SetRotation(float rotation);
	virtual float GetRotation() const;

	virtual Vec3 GetRotation3D() const;
	virtual void SetRotation3D(const Vec3 & rot);


	virtual void SetPosition(float x, float y);
	virtual void SetPosition(const Vec2 & pos);
	virtual void SetPositionX(float x);
	virtual void SetPositionY(float y);
	virtual void SetPositionZ(float z);
	virtual void SetPosition3D(const Vec3 & pos);

	virtual Vec2 GetPoistion() const;
	virtual float GetPoistionX() const;
	virtual float GetPoistionY() const;
	virtual float GetPoistionZ() const;
	virtual Vec3 GetPoistion3D() const;

	virtual void SetScaleX(float scaleX);
	virtual void SetScaleY(float scaleY);
	virtual void SetScaleZ(float scaleZ);
	virtual void SetScale(float scaleX, float scaleY);
	virtual void SetScale(float scale);

	virtual float GetScaleX() const;
	virtual float GetScaleY() const;
	virtual float GetScaleZ() const;
	virtual float GetScale() const;

	virtual void Update(float delta);

	virtual void IgnoreAnchorPointForPosition(bool ignore);
	virtual bool IsIgnoreAnchorPointForPosition() const;

	virtual const Color3B& GetColor() const;
	virtual GLubyte GetOpacity() const;

	virtual void SetColor(const Color3B& color);
	virtual void SetColorWithChild(const Color3B& color);
	virtual void SetOpacity(GLubyte opacity);
	virtual void SetOpacityWithChild(GLubyte opacity);

	/**
	*Returns the matrix that transform the node's (local) space coordinates into the parent's space coordinates.
	*The matrix is in Pixels.
	*param   
	*returns The transformation matrix.
	*by [3/29/2017 song.zhang079] 
	*/
	virtual const Mat4& GetNodeToParentTransform() const;
	virtual AffineTransform GetNodeToParentAffineTransform() const;

	
	/**
	* Returns the matrix that transform the node's (local) space coordinates into the parent's space coordinates.
	* The matrix is in Pixels.
	* Note: If ancestor is not a valid ancestor of the node, the API would return the same value as @see getNodeToWorldTransform
	*
	*param ancestor The parent's node pointer.
	*return The transformation matrix.
	*/
	virtual Mat4 GetNodeToParentTransform(CHmsNode* ancestor) const;
	/**
	* Returns the affine transform matrix that transform the node's (local) space coordinates into the parent's space coordinates.
	* The matrix is in Pixels.
	*
	* Note: If ancestor is not a valid ancestor of the node, the API would return the same value as @see getNodeToWorldAffineTransform
	*
	*param ancestor The parent's node pointer.
	*return The affine transformation matrix.
	*/
	virtual AffineTransform GetNodeToParentAffineTransform(CHmsNode* ancestor) const;

	/**
	* Sets the transformation matrix manually.
	*
	*param transform A given transformation matrix.
	*/
	virtual void SetNodeToParentTransform(const Mat4& transform);

	/**
	* Returns the matrix that transform parent's space coordinates to the node's (local) space coordinates.
	* The matrix is in Pixels.
	*
	*return The transformation matrix.
	*/
	virtual const Mat4& GetParentToNodeTransform() const;
	virtual AffineTransform GetParentToNodeAffineTransform() const;

	/**
	* Returns the world affine transform matrix. The matrix is in Pixels.
	*
	*return transformation matrix, in pixels.
	*/
	virtual Mat4 GetNodeToWorldTransform() const;
	virtual AffineTransform GetNodeToWorldAffineTransform() const;

	/**
	* Returns the inverse world affine transform matrix. The matrix is in Pixels.
	*
	*return The transformation matrix.
	*/
	virtual Mat4 GetWorldToNodeTransform() const;
	virtual AffineTransform GetWorldToNodeAffineTransform() const;

	/**
	* Converts a Vec2 to node (local) space coordinates. The result is in Points.
	*
	*param worldPoint A given coordinate.
	*return A point in node (local) space coordinates.
	*/
	Vec2 ConvertToNodeSpace(const Vec2& worldPoint) const;

	/**
	* Converts a Vec2 to world space coordinates. The result is in Points.
	*
	*param nodePoint A given coordinate.
	*return A point in world space coordinates.
	*/
	Vec2 ConvertToWorldSpace(const Vec2& nodePoint) const;

	/**
	* Converts a Vec2 to node (local) space coordinates. The result is in Points.
	* treating the returned/received node point as anchor relative.
	*
	*param worldPoint A given coordinate.
	*return A point in node (local) space coordinates, anchor relative.
	*/
	Vec2 ConvertToNodeSpaceAR(const Vec2& worldPoint) const;

	/**
	* Converts a local Vec2 to world space coordinates.The result is in Points.
	* treating the returned/received node point as anchor relative.
	*
	*param nodePoint A given coordinate.
	*return A point in world space coordinates, anchor relative.
	*/
	Vec2 ConvertToWorldSpaceAR(const Vec2& nodePoint) const;

	/**
	* convenience methods which take a Touch instead of Vec2.
	*
	* @param touch A given touch.
	* @return A point in world space coordinates.
	*/
	Vec2 ConvertTouchToNodeSpace(Touch * touch) const;

	/**
	* converts a Touch (world coordinates) into a local coordinate. This method is AR (Anchor Relative).
	*
	* @param touch A given touch.
	* @return A point in world space coordinates, anchor relative.
	*/
	Vec2 ConvertTouchToNodeSpaceAR(Touch * touch) const;

	Vec2 ConvertOriginPointToAR(const Vec2 & posOrigin);
	Vec2 ConvertARPointToOrigin(const Vec2 & posAR);

	/**
	*  Sets an additional transform matrix to the node.
	*
	*  In order to remove it, call it again with the argument `nullptr`.
	*
	*note The additional transform will be concatenated at the end of getNodeToParentTransform.
	*        It could be used to simulate `parent-child` relationship between two nodes (e.g. one is in BatchNode, another isn't).
	*
	*param additionalTransform An additional transform matrix.
	*/
	void SetAdditionalTransform(Mat4* additionalTransform);
	void SetAdditionalTransform(const AffineTransform& additionalTransform);

	/**
	* Sets the arrival order when this node has a same ZOrder with other children.
	*
	* A node which called addChild subsequently will take a larger arrival order,
	* If two children have the same Z order, the child with larger arrival order will be drawn later.
	*
	*warning This method is used internally for localZOrder sorting, don't change this manually
	*
	*param orderOfArrival   The arrival order.
	*/
	void SetOrderOfArrival(int orderOfArrival);

	/**
	* Returns the arrival order, indicates which children is added previously.
	*
	*see `setOrderOfArrival(unsigned int)`
	*
	*return The arrival order.
	*/
	int GetOrderOfArrival() const;

	bool IsVisible(){ return m_bVisible; }
	virtual void SetVisible(bool bVisible);

	CHmsScreen * GetScreen() const;

	virtual bool IsUpdateAble(){ return m_bVisible&&m_bRunning; }

	void SetScheduleUpdate();
	void SetUnSchedule();
	void SetScheduleOnce(const HmsSchedulerFunc & schedule);

	Rect GetRectFromParent();
	bool ContainParentPosition(const Vec2 & pos);

	Rect GetRectByAnchorPoint();
	virtual Rect GetRectByOrigin();
	Rect ConvertOriginRectToAR(const Rect & rectOrigin);
	Rect ConvertARRectToOrigin(const Rect & rectAR);

    Vec2 GetAnchorPointInPoints(){ return m_anchorPointInPoints; };

	void RunAction(CHmsAction * action);
	void StopActioin(CHmsAction * action);
	void StopActionByTag(int nTag);
	void StopAllActions();

	void SetCameraMask(uint32_t mask){
		m_maskCamera = mask;
	}

	void ForceUpdate(){ SetTransformNeedToUpdate(); }
	void ForceUpdateAllChild();

protected:
	Mat4 GetTransform(const Mat4 &parentTransform);
	uint32_t ProcessParentFlags(const Mat4& parentTransform, uint32_t parentFlags);
	bool IsVisitableByVisitingCamera() const;
	void UpdateRotationQuat();
	void UpdateRotation3D();
	inline void SetTransformNeedToUpdate(){ m_transformUpdated = m_transformDirty = m_inverseDirty = true; };
	//remove a child from the child array , and call child->OnExit()
	void DetachChild(CHmsNode * child, ssize_t childIndex, bool doCleanUp);
	void InsertChild(CHmsNode * child, int zOrder);

	virtual void ReorderChild(CHmsNode * child, int zOrder);
	virtual void UpdateColor() {}

protected:
	CHmsAviDisplayUnit *			m_pDisplayUnit;

	Vector<CHmsNode*>				m_vChild;			//the child node
	CHmsNode *						m_pParent;			//the parent node

	GLProgramState					* m_pGlProgramState;

	bool							m_reorderChildDirty;

	void							* m_pUserData;
	Ref								* m_pUserObject;

	bool							m_bVisible;			//is visible
	bool							m_bRunning;
	bool							m_bUpdateSchedule;	//recode whether update schedule

	float							m_rotationX;               //rotation on the X-axis
	float							m_rotationY;               //rotation on the Y-axis
	float							m_rotationZ;			   //rotation on the Z-axis

	Quaternion						m_rotationQuat;				//rotation using quaternion, if _rotationZ_X == _rotationZ_Y, _rotationQuat = RotationZ_X * RotationY * RotationX, else _rotationQuat = RotationY * RotationX

	float							m_scaleX;                  //scaling factor on x-axis
	float							m_scaleY;                  //scaling factor on y-axis
	float							m_scaleZ;                  //scaling factor on z-axis

	Vec2							m_pos;				//position of the node
	float							m_posZ;				//the z order
	Vec2							m_anchorPoint;		//anchor point normalized (not in points)
	Vec2							m_anchorPointInPoints;
	bool							m_ignoreAnchorPointForPosition;

	Size							m_sizeContent;              // untransformed size of the node
	bool							m_sizeContentDirty;         // whether or not the contentSize is dirty

	Mat4							m_transformModeView;		// mode view transform of the node

	//"cache" variables are allowed to be mutable
	mutable Mat4					m_transform;				// transform
	mutable bool					m_transformDirty;			// transform dirty flag
	mutable Mat4					m_inverse;					// inverse transform
	mutable bool					m_inverseDirty;				// inverse transform dirty flag
	mutable Mat4					m_additionalTransform;		// transform
	bool							m_useAdditionalTransform;   // The flag to check whether the additional transform is dirty
	bool							m_transformUpdated;         // Whether or not the Transform object was updated since the last frame

	int32_t							m_localZOrder;
	int32_t							m_orderOfArrival;

	float							m_globalZOrder;

	uint32_t						m_maskCamera;
	int32_t							m_nTag;
	std::string						m_strName;

	Color3B							m_colorDisplay;			//color used for display
	GLubyte							m_opacityDisplay;		//opacity used for display
	
private:
	HMS_DISALLOW_COPY_AND_ASSIGN(CHmsNode);

	static int32_t s_nGlobalOrderOfArrivalCounter;				//counter for order of arrival
};

NS_HMS_END