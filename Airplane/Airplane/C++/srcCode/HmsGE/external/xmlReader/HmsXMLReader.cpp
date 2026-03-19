#include "HmsXMLReader.h"
#include <algorithm>
using namespace tinyxml2;

CHmsXMLReader::CHmsXMLReader(const std::string & strPath, const std::string & strRoot)
	: m_bOpen(false)
{
	m_xmlPath = strPath;
	m_strRootText = strRoot;
	m_eleRoot = nullptr;
}

CHmsXMLReader::~CHmsXMLReader(void)
{
}

void CHmsXMLReader::SetPath(const std::string & strPath)
{
	m_xmlPath = strPath;
}

void CHmsXMLReader::SetRootName(const std::string & rootName)
{
	m_strRootText = rootName;
}

bool CHmsXMLReader::Save()
{
	if (m_xmlDom.SaveFile(m_xmlPath.c_str()) == XMLError::XML_SUCCESS)
	{
		return true;
	}

	return false;
}

bool CHmsXMLReader::Open()
{
	bool bRet = false;

	XMLError err = m_xmlDom.LoadFile(m_xmlPath.c_str());
	if (err == XMLError::XML_SUCCESS)
	{
		m_eleRoot = m_xmlDom.RootElement();
		std::string strRootTag = m_eleRoot->Value();
		if (strRootTag == m_strRootText)
		{
			bRet = true;
		}
	}
	return bRet;
}

void CHmsXMLReader::CheckXML()
{
	if (!m_bOpen)
	{
		m_bOpen = Open();
	}

	if (!m_bOpen)
	{
		m_bOpen = CreateXml();
	}
}

XMLElement* CHmsXMLReader::FindXmlElement(const std::vector<std::string> & strList)
{
	XMLElement *curElement = nullptr;
	XMLElement *tempElement = nullptr;

	if (strList.size() == 0)
	{
		return nullptr;
	}

	if (m_eleRoot)
	{
		curElement = m_eleRoot;
		for (std::string str : strList)
		{
			tempElement = FindChildElement(curElement, str);
			if (tempElement != nullptr)
				curElement = tempElement;
			else
				return nullptr;
		}
	}

	return curElement;
}

XMLElement* CHmsXMLReader::FindXmlElement(const std::string & strKey)
{
	std::vector<std::string> splitStrList;
	SplitString(strKey, splitStrList, "/");

	XMLElement *find = FindXmlElement(splitStrList);

	return find;
}

XMLElement* CHmsXMLReader::FindXmlElement(const std::string & strKey, std::string & strLastGroup)
{
	std::vector<std::string> splitStrList;
	SplitString(strKey, splitStrList, "/");

	if (splitStrList.size() == 0)
	{
		return nullptr;
	}
	
	strLastGroup = splitStrList.back();
	splitStrList.pop_back();
	XMLElement *find = FindXmlElement(splitStrList);

	return find;
}

XMLElement* CHmsXMLReader::FindChildElement(XMLElement * parentElement, const std::string & strTagName)
{
	XMLElement *findElement = nullptr;
	if (parentElement)
	{
		XMLNode *n = parentElement->FirstChild();
		while (n != nullptr)
		{
			XMLElement *element = n->ToElement();
			if (element == nullptr)
			{
				break;
			}
			else if (strTagName.compare(element->Name()) == 0)
			{
				findElement = element;
				break;
			}
			n = n->NextSiblingElement();
		}
	}
	return findElement;
}

void CHmsXMLReader::SetElementText(XMLElement * domElement, const std::string & strData)
{
	XMLText * text = m_xmlDom.NewText(strData.c_str());
	domElement->InsertEndChild(text);
}

bool CHmsXMLReader::ConvertXmlAttribute2String(tinyxml2::XMLElement * element, const char * strAttribute, std::string & string)
{
	bool bRet = false;
	if (element)
	{
		const char * strTemp = element->Attribute(strAttribute);
		if (strTemp != nullptr)
		{
			string = strTemp;
			bRet = true;
		}
	}
	return bRet;
}

bool CHmsXMLReader::GetNodeText(tinyxml2::XMLElement * element, std::string & string)
{
	if (element)
	{
		const char * strText = element->GetText();
		if (strText)
		{
			string = strText;
			return true;
		}
	}
	return false;
}

std::string CHmsXMLReader::TrimString(std::string & str)
{
	static std::string whitespaces(" \t\f\v\n\r");
	auto pos = str.find_last_not_of(whitespaces);
	if (pos != std::string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(whitespaces);
		if (pos != std::string::npos) str.erase(0, pos);
	}
	else
		str.erase(str.begin(), str.end());

	str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
	return str;
}

bool CHmsXMLReader::CreateXml()
{
	const char* declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
	m_xmlDom.Parse(declaration);//会覆盖xml所有内容

	//添加申明可以使用如下两行
	//XMLDeclaration* declaration=doc.NewDeclaration();
	//doc.InsertFirstChild(declaration);

	m_eleRoot = m_xmlDom.NewElement(m_strRootText.c_str());
	m_xmlDom.InsertEndChild(m_eleRoot);

	if (m_xmlDom.SaveFile(m_xmlPath.c_str()) == XMLError::XML_SUCCESS)
	{
		return true;
	}

	return false;
}

void CHmsXMLReader::SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

std::string CHmsXMLReader::DoubleToString(double num)
{
	char str[256];
	std::sprintf(str, "%lf", num);
	std::string result = str;
	return result;
}

std::string CHmsXMLReader::IntToString(int num)
{
	char str[256];
	std::sprintf(str, "%d", num);
	std::string result = str;
	return result;
}