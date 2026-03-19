#pragma once
#include "external/tinyxml2/tinyxml2.h"
#include <map>
#include <vector>
#include <sstream>
//using namespace tinyxml2;

class CHmsXMLReader
{
public:
	CHmsXMLReader(const std::string & strPath, const std::string & strRoot);
	~CHmsXMLReader(void);

	void SetPath(const std::string & strPath);

	void SetRootName(const std::string & rootName);

protected:
	bool Save();

	bool Open();

	void CheckXML();

	tinyxml2::XMLElement* FindXmlElement(const std::vector<std::string> & strList);
	tinyxml2::XMLElement* FindXmlElement(const std::string & strKey);
	tinyxml2::XMLElement* FindXmlElement(const std::string & strKey, std::string & strLastGroup);
	tinyxml2::XMLElement* FindChildElement(tinyxml2::XMLElement * parentElement, const std::string & strTagName);

	void SetElementText(tinyxml2::XMLElement * domElement, const std::string & strData);

	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

	std::string DoubleToString(double num);

	std::string IntToString(int num);


	template <class T>
	void ConvertString2Data(const std::string & str, T & t)
	{
		std::stringstream ss(str);
		ss >> t;
	}

	template <class T>
	bool ConvertXmlAttribute2Data(tinyxml2::XMLElement * element, const char * strAttribute, T & t)
	{
		bool bRet = false;
		if (element)
		{
			const char * strTemp = element->Attribute(strAttribute);
			if (strTemp != nullptr)
			{
				std::string strText = strTemp;
				if (!strText.empty())
				{
					ConvertString2Data(strText, t);
					return bRet;
				}
			}
		}
		return bRet;
	}

	bool ConvertXmlAttribute2String(tinyxml2::XMLElement * element, const char * strAttribute, std::string & string);

	bool GetNodeText(tinyxml2::XMLElement * element, std::string & string);

	std::string TrimString(std::string & str);

protected:
	bool CreateXml();

public:
	std::string						m_xmlPath;
	std::string						m_strRootText;
	bool							m_bOpen;

	tinyxml2::XMLDocument			m_xmlDom;
	tinyxml2::XMLElement			* m_eleRoot;
};