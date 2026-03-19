#include "HmsLanguageMananger.h"
#include "external/tinyxml2/tinyxml2.h"
#include "base/HmsFileUtils.h"
#include <iostream>
#include <sstream>
#include <HmsLog.h>

USING_NS_HMS;

std::string GetLanguageConfigPath()
{
	auto path = CHmsFileUtils::getInstance()->fullPathForFilename("LanguageConfig.xml");
	if (path.empty())
	{
		path = "LanguageConfig.xml";
	}
	HMSLOG("LanguageConfig.xml path=%s", path.c_str());
	return path;
}

CHmsLanguageMananger::CHmsLanguageMananger()
	: m_pCurrentLanuage(nullptr)
	, m_nLanguageID(-1)
	, m_bUpdate(false)
{
}


CHmsLanguageMananger::~CHmsLanguageMananger()
{
}

CHmsLanguageMananger * CHmsLanguageMananger::create()
{
	CHmsLanguageMananger * pMnger = new(std::nothrow) CHmsLanguageMananger;
	if (pMnger)
	{
		pMnger->Init();
	}
	return pMnger;
}

template <class T>
void GetData(const std::string & str, T & t)
{
	std::stringstream ss(str);
	ss >> t;
}

template <class T>
void GetXmlData(tinyxml2::XMLElement * element, const char * strAttribute, T & t)
{
	if (element)
	{
		std::string strText;
		strText = element->Attribute(strAttribute);
		if (!strText.empty())
		{
			GetData(strText, t);
		}
	}
}

std::string GetXmlString(tinyxml2::XMLElement * element, const char * strAttribute)
{
	std::string strOut;
	if (element)
	{
		std::string strText;
		strText = element->Attribute(strAttribute);
		if (!strText.empty())
		{
			strOut = strText;
		}
	}
	return strOut;
}

void CHmsLanguageMananger::Init()
{
	auto path = GetLanguageConfigPath();

	tinyxml2::XMLDocument doc1;
#if 1
	Data data = CHmsFileUtils::getInstance()->getDataFromFile(path);
	if (data.isNull())
		return;

	if (doc1.Parse((const char*)data.getBytes(), data.getSize()) != tinyxml2::XML_NO_ERROR)
		return;
#else
	if (doc1.LoadFile(path.c_str()) != tinyxml2::XML_NO_ERROR)
	{
		return;
	}
#endif

	auto root = doc1.RootElement();
	if (root != nullptr)
	{
		std::string strText = root->Name();
		auto languageSupport = root->FirstChildElement("LanguageSupport");
		if (nullptr == languageSupport && strText == "LanguageSupport")
		{
			languageSupport = root;
		}

		if (languageSupport)
		{
			auto language = languageSupport->FirstChildElement("Language");
			while (language)
			{
				LanguageDescribe ld;
				GetXmlData(language, "Id", ld.nID);
// 				GetXmlData(language, "Name", ld.strName);
// 				GetXmlData(language, "LanguagePath", ld.strPath);
				ld.strName = GetXmlString(language, "Name");
				ld.strPath = GetXmlString(language, "LanguagePath");

				m_mapLanguageDescribe[ld.nID] = ld;
				language = language->NextSiblingElement();
			}
		}
	}

	for (auto & c: m_mapLanguageDescribe)
	{	
		InitLanguageDescribe(c.second);
	}
	int a = 0;
}

CHmsLanguageMananger * CHmsLanguageMananger::GetInstance()
{
	static CHmsLanguageMananger * s_pLanguageMananger = nullptr;

	if (nullptr == s_pLanguageMananger)
	{
		s_pLanguageMananger = CHmsLanguageMananger::create();
	}

	return s_pLanguageMananger;
}

void CHmsLanguageMananger::InitLanguageDescribe(LanguageDescribe & ld)
{
#if defined(__linux)
    CHmsFileUtils::StringReplace(ld.strPath,"\\", "/");
#endif

	auto path = CHmsFileUtils::getInstance()->fullPathForFilename(ld.strPath);
    
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
    CHmsFileUtils::StringReplace(path,"\\", "/");
#endif

    

#if defined(__linux)
    CHmsFileUtils::StringReplace(path,"\\", "/");
#endif

	tinyxml2::XMLDocument doc1;
#if 1
	Data data = CHmsFileUtils::getInstance()->getDataFromFile(path);
	if(data.isNull())
		return;

	if( doc1.Parse((const char*)data.getBytes(), data.getSize()) != tinyxml2::XML_NO_ERROR)
		return;
#else
	if (doc1.LoadFile(path.c_str()) != tinyxml2::XML_NO_ERROR)
	{
		return;
	}
#endif

	auto root = doc1.RootElement();
	if (root != nullptr)
	{
		std::string strText = root->Name();
		auto language = root->FirstChildElement("Language");
		if (nullptr == language && strText == "Language")
		{
			language = root;
		}

		if (language)
		{
			auto item = language->FirstChildElement("LanguageItem");
			while (item)
			{
				std::string strKey;
				std::string value;

// 				GetXmlData(item, "Key", strKey);
// 				GetXmlData(item, "Word", value);
				strKey = GetXmlString(item, "Key");
				value = GetXmlString(item, "Word");

				if (strKey.empty() || value.empty())
				{
					
				}
				else
				{
					ld.mapObj[strKey] = value;
				}
				//auto dd = u2a(strKey.c_str());

				item = item->NextSiblingElement();
			}
		}
	}
}

void CHmsLanguageMananger::AddObject(CHmsLanguageObject * pObj)
{
	m_listLanguageObject.pushBack(pObj);
}

void CHmsLanguageMananger::RemoveObject(CHmsLanguageObject * pObj)
{
	m_listLanguageObject.eraseObject(pObj);
// 	auto it = m_listLanguageObject.begin();
// 	for (; it != m_listLanguageObject.end(); it++)
// 	{
// 		if (*it == pObj)
// 		{
// 			m_listLanguageObject.erase(it);
// 			return;
// 		}
// 	}
}

void CHmsLanguageMananger::SetLanguageID(int nID, bool AutoUpdate)
{
	CHmsCSLocker locker(m_cs);

	if (m_nLanguageID != nID)
	{
		auto itFind = m_mapLanguageDescribe.find(nID);
		if (itFind != m_mapLanguageDescribe.end())
		{
			m_pCurrentLanuage = &itFind->second;
		}

		if (AutoUpdate)
		{
			m_bUpdate = true;
		}

		m_sLanguageName = m_pCurrentLanuage->strName;
		m_nLanguageID = nID;
	}
}

void CHmsLanguageMananger::SetLanguageWithName(std::string name, bool AutoUpdate /*= false*/)
{
	if (m_sLanguageName != name)
	{
		std::map<int, LanguageDescribe>::iterator itera = m_mapLanguageDescribe.begin();
		while (itera != m_mapLanguageDescribe.end())
		{
			if (itera->second.strName == name)
			{
				SetLanguageID(itera->first);
				break;
			}
			itera++;
		}
	}
}

std::string CHmsLanguageMananger::GetCurLanguageText(const std::string & strText)
{
	CHmsCSLocker locker(m_cs);

	std::string strOut;
	if (m_pCurrentLanuage)
	{
		auto find = m_pCurrentLanuage->mapObj.find(strText);
		if (find != m_pCurrentLanuage->mapObj.end())
		{
			strOut = find->second;
		}
	}

	if (strOut.empty())
	{
		strOut = strText;
	}
	return strOut;
}

void CHmsLanguageMananger::CheckUpdate()
{
	if (m_bUpdate)
	{
		for (auto c : m_listLanguageObject)
		{
			c->UpdateLanguage();
		}
		m_bUpdate = false;
	}
}

std::string CHmsLanguageMananger::GetLanguageName()
{
	return m_sLanguageName;
}
