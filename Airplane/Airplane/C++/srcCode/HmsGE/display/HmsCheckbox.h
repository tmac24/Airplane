#pragma once
#include "HmsButton.h"
#include "HmsStretchImageNode.h"
#include "base/RefPtr.h"
#include "HmsLabel.h"

NS_HMS_BEGIN

class CHmsStretchImageNode;
class CHmsImageNode;

class CHmsCheckBox : public CHmsButton
{
public:
	//状态改变回调函数：sender，状态 
	typedef std::function<void(CHmsCheckBox*, bool)> onStateChangedCallback;

	CHmsCheckBox();
	~CHmsCheckBox();

	static CHmsCheckBox* Create();

	static CHmsCheckBox* Create(const std::string &normalImage,
							  const std::string &selectedImage);

	static CHmsCheckBox* Create(const std::string &normalImage,
							  const std::string &selectedImage,
							  const onClickCallback& callback);

	static CHmsCheckBox* Create(const std::string &normalImage,
							  const std::string &selectedImage,
							  const std::string &disableImage,
							  const onClickCallback& callback);

	virtual bool Init() override;

	virtual void Selected() override;

	virtual void UnSelected() override;

	virtual void SetSize(const Size &size) override;

	void OnReleased(const Vec2 & posParent) override;

	void SetStateChangedCallback(onStateChangedCallback stateChanged);

	onStateChangedCallback OnStateChanged;
protected:
	virtual void ContentLayout() override;
};

NS_HMS_END