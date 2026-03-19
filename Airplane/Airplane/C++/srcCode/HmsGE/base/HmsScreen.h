#pragma once


#include "base/HmsNode.h"
#include "base/HmsScreenPointer.h"
NS_HMS_BEGIN

class Camera;

class HMS_DLL CHmsScreen : public CHmsNode
{
	friend class Camera;
public:
	//create a new CHmsScreen object
	static CHmsScreen * Create();

	//create a new CHmsScreen object by a assigned size
	static CHmsScreen * CreateWithSize(const Size & size);

	//Get all the cameras under the Screen
	const std::vector<Camera*> & GetCameras();

	Camera * GetDefaultCamera() const{ return m_cameraDefault; }

	//Render the screen
	void Render(Renderer * renderer);

	CHmsScreenPointer * GetScreenPointer(){ return m_screenPointer.get(); }

public:
	CHmsScreen();
	virtual ~CHmsScreen();

	bool Init() override;
	bool InitWithSize(const Size & size);

	void SetCameraOrderDirty(){ m_bDirtyCamera = true; }



private:
	std::vector<Camera*>	m_vCameras;					//vector store all the camera on the Screen
	Camera					* m_cameraDefault;			//the default camera
	bool					m_bDirtyCamera;				//signed camera whether dirty

	RefPtr<CHmsScreenPointer>		m_screenPointer;
private:
	HMS_DISALLOW_COPY_AND_ASSIGN(CHmsScreen);
};


NS_HMS_END
