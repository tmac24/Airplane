#pragma once
#include "HmsGL.h"
#include "HmsAviMacros.h"
#include "base/Ref.h"
#include "math/HmsMath.h"
#include "math/HmsGeometry.h"
#include "base/HmsScheduler.h"
#include "base/HmsEventDispatcher.h"
#include "base/HmsActionMgr.h"
#include <stack>

extern "C" {
    struct ESContext;
}


NS_HMS_BEGIN

class Renderer;
class TextureCache;
class CHmsScreen;
class FrameBuffer;

/**
* Matrix stack type
*/
enum class MATRIX_STACK_TYPE
{
	/// Model view matrix stack
	MATRIX_STACK_MODELVIEW,

	/// projection matrix stack
	MATRIX_STACK_PROJECTION,

	/// texture matrix stack
	MATRIX_STACK_TEXTURE
};

class HMS_DLL CHmsAviDisplayUnit : public Ref

{
	friend class Camera;
public:
	CHmsAviDisplayUnit();
	~CHmsAviDisplayUnit();

	enum class Projection
	{
		_2D,/// Sets a 2D projection (orthogonal projection).
		_3D,/// Sets a 3D projection with a fovy=60, znear=0.5f and zfar=1500.
		CUSTOM,/// It calls "updateProjection" on the projection delegate.
		DEFAULT = _3D,/// Default projection is 3D projection.
	};

	/**
	*returns a shared instance of CHmsAviView
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
	static CHmsAviDisplayUnit * GetInstance();

	virtual bool Init();

    void SetESContext(ESContext * esContext){
        m_esContext = esContext;
    }

    ESContext * GetESContext(){ return m_esContext; }

	void SetViewSize(const HmsAviPf::Size & size);

	const Size & GetViewSize() const;

	void InitDisplayUnit();

	void SetDataUpdateCallBack(const std::function<void(float)> & callback){
		m_funcUpdateData = callback;
	}

public:
	void SetProjection(Projection projection);
	Projection GetProjection(){ return m_projection; }

	Renderer* GetRenderer() const { return m_pRenderer; }

	TextureCache * GetTextureCache(){ return m_pTextureCache; }

	CHmsScheduler * GetScheduler(){ return m_pScheduler; }

	CHmsEventDispatcher * GetEventDispatcher() const { return m_pEventDispatcher; }

	CHmsScreen * GetCurrentScreen(){ return m_pScreen; }

	CHmsActionMgr * GetActionMgr(){ return m_pActionMgr; }

	void InitMatrixStack();

	/**
	*Clones a specified type matrix and put it to the top of specified type of matrix stack.
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
	void PushMatrix(MATRIX_STACK_TYPE type);

	/**
	*Pops the top matrix of the specified type of matrix stack.
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
	void PopMatrix(MATRIX_STACK_TYPE type);

	/**
	*Adds an identity matrix to the top of specified type of matrix stack.
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
	void LoadIdentityMatrix(MATRIX_STACK_TYPE type);

	/**
	*Adds a matrix to the top of specified type of matrix stack.
	*param   type Matrix type.
	*param   mat The matrix that to be added.
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
	void LoadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat);

	/**
	*Multiplies a matrix to the top of specified type of matrix stack.
	*param   type Matrix type.
	*param   mat The matrix that to be multiplied.
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
	void MultiplyMatrix(MATRIX_STACK_TYPE type, const Mat4& mat);

	/**
	*Gets the top matrix of specified type of matrix stack.
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
	const Mat4& GetMatrix(MATRIX_STACK_TYPE type);

	/**
	*Clear all types of matrix stack, and add identity matrix to these matrix stacks.
	*param   
	*returns 
	*by [3/24/2017 song.zhang079] 
	*/
	void ResetMatrixStack();

	virtual void DrawScreen(float fDelta);

	//set the run screen
	void RunScreen(CHmsScreen * pScreen);

	//set the next screen
	void SetNextScreen(CHmsScreen * pScreen);

	/**
	* Converts a screen coordinate to an OpenGL coordinate.
	* Useful to convert (multi) touch coordinates to the current layout (portrait or landscape).
	*/
	Vec2 ConvertToGL(const Vec2& point);

	/**
	* Converts an OpenGL coordinate to a screen coordinate.
	* Useful to convert node points to window points for calls such as glScissor.
	*/
	Vec2 ConvertToUI(const Vec2& point);

private:
	/**
	*Gets the distance between camera and near clipping frame.
	*It is correct for default camera that near clipping frame is same as the screen.
	*param   
	*returns 
	*by [3/27/2017 song.zhang079] 
	*/
	float GetZEye() const;

protected:
	std::stack<Mat4>		m_matrixStackModeView;					//the mode view matrix stack
	std::stack<Mat4>		m_matrixStackProjection;				//the projection matrix stack
	std::stack<Mat4>		m_matrixStackTexture;					//the texture matrix stack

private:
	Projection				m_projection;

	Renderer				* m_pRenderer;

	CHmsScreen				* m_pScreen;
	CHmsScreen				* m_pNextScreen;

	TextureCache			* m_pTextureCache;

	CHmsScheduler			* m_pScheduler;

	CHmsActionMgr			* m_pActionMgr;

	HmsAviPf::Size			m_sizeViewInPoints;

	CHmsEventDispatcher		* m_pEventDispatcher;

	std::function<void(float)>	m_funcUpdateData;

	FrameBuffer				* m_pFrameBuffer;

    ESContext               * m_esContext;
};

NS_HMS_END