#pragma once
#include "HmsAviContext.h"
#include "math/HmsGeometry.h"

class CHmsUserContext : public CHmsAviContext
{
public:
	~CHmsUserContext();

    static CHmsUserContext* GetInstance();

    virtual void InitFrame();
	virtual void InitScreen();
	virtual void BeforeRuning();

    void SetPathCfg(std::string str);
    void SetPathAviRes(std::string str);
    void SetPathSvs(std::string str);
    void SetPathTerrain(std::string str);
    void SetPathNav(std::string str);
    void SetPathNavSvsImage(std::string str);
    void SetWritePath(std::string str);
    void SetDesignSize(HmsAviPf::Size s);
	void SetViewportSize(HmsAviPf::Size s);
    void SetPathAviData(std::string str);

	void StartAppNetwork();
	void BeforeAviRuning();

    void CheckFilesPath();

	void CreateAndShowWelcomeScreen();
	void CreateAndShowAviScreen();
    
    void InitDesignSize(float w, float h);
    void SetViewSize(float width, float height);

private:
    CHmsUserContext();
    void LoadXml();

private:
	float m_scale;
    std::string m_stringPathCfg;
    std::string m_stringPathAviRes;
    std::string m_stringPathSvs;
    std::string m_stringPathTerrain;
    std::string m_stringPathNav;
    std::string m_stringNavSvsImagePath;
    std::string m_stringWritePath;
    
    std::string m_stringAviDataPath;
    
    HmsAviPf::Size m_designSize;
	HmsAviPf::Size m_viewportSize;
};

