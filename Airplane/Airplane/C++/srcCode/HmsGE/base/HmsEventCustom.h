#pragma once

#include "HmsAviMacros.h"
#include "base/HmsEvent.h"
#include <string>

NS_HMS_BEGIN

class CHmsNode;

class HMS_DLL CHmsEventCustom : public CHmsEvent
{
public:
	CHmsEventCustom(const std::string& eventName);

	void setUserData(void* data) { _userData = data; }

	void* getUserData() const { return _userData; }

	const std::string& getEventName() const { return _eventName; }

protected:
	void* _userData;       ///< User data
	std::string _eventName;
};
NS_HMS_END
