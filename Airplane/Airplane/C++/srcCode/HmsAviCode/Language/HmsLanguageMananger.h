#pragma once
#include "HmsLanguageObject.h"
#include "HmsCS.h"
#include <string.h>
#include <map>
#include "base/HmsVector.h"

struct LanguageDescribe
{
	int				nID;
	std::string		strName;
	std::string		strPath;
	std::map<std::string, std::string>  mapObj;
};

class CHmsLanguageMananger
{
protected:
	CHmsLanguageMananger();

	void Init();
public:
	~CHmsLanguageMananger();

	static CHmsLanguageMananger * create();
	static CHmsLanguageMananger * GetInstance();

	void AddObject(CHmsLanguageObject * pObj);
	void RemoveObject(CHmsLanguageObject * pObj);

	void SetLanguageID(int nID, bool AutoUpdate = false);

	void SetLanguageWithName(std::string name, bool AutoUpdate = false);

	std::string GetCurLanguageText(const std::string & strText);

	void CheckUpdate();

	std::string GetLanguageName();

protected:
	void InitLanguageDescribe(LanguageDescribe & ld);

private:
	std::map<int, LanguageDescribe>			m_mapLanguageDescribe;
	LanguageDescribe *						m_pCurrentLanuage;

	//std::vector<CHmsLanguageObject*>		m_listLanguageObject;
	HmsAviPf::Vector<CHmsLanguageObject*>		m_listLanguageObject;

	CHmsCS									m_cs;

	int										m_nLanguageID;
	std::string								m_sLanguageName;
	bool									m_bUpdate;
};

