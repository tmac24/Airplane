#pragma once
#include "external/tinyxml2/tinyxml2.h"
#include <map>
#include <vector>
//#include "QStructDescription.h"
#include <functional>
#include <string>
#include "Warning/HmsWarningColorStyle.h"

struct HmsDbSubMapLayer
{
	int					xMin;
	int					xMax;
	int					yMin;
	int					yMax;
	std::string			strDbPath;
};

struct HmsDbMapLayer
{
	int									nLayer;					//the layer of the map
	bool								isSubdivision;			//whether cut-of the layer to multi database
	std::string							strDbPath;				//the path of the layer
	std::vector<HmsDbSubMapLayer>		vDbPath;

	HmsDbMapLayer()
	{
		nLayer = 0;
		isSubdivision = false;
	}
};

struct HmsDbMap
{
	std::string							strMapType;
    std::string                         strDisplayName;
	int									layerMin;
	int									layerMax;
	std::map<int, HmsDbMapLayer>		mapLayer;
};

struct HmsDeviceConnectInfo
{
	std::string	 strProtocols;
	int			 nRedirect;
	std::string  strServerIp;
};

struct HmsAdminAreaDbInfo
{
	bool bUsed;
	std::string strName;
	std::string strDataType;
	std::string strDbPath;
};

//using namespace tinyxml2;

class CHmsConfigXMLReader
{
public:
	CHmsConfigXMLReader();
	CHmsConfigXMLReader(const std::string & strPath);
	CHmsConfigXMLReader(const std::string & strPath, const std::string & strRootText);
	~CHmsConfigXMLReader(void);

	static CHmsConfigXMLReader * GetSingleConfig();
	static CHmsConfigXMLReader * GetSingleConfig(const std::string & strPath);
	static void ReleaseSingleConfig();

	std::string GetConfigFileFolder();
	std::string GetOtherConfigFilePath(const std::string & strFileName);

	void SetPath(const std::string & strPath);

	void SetRootName(const std::string & rootName);

	int GetConfigInfo(const std::string & strKey, int nDefault = 0);
	float GetConfigInfo(const std::string & strKey, float fDefault = 0.0);
	double GetConfigInfo(const std::string & strKey, double dDefault = 0.0);
	std::string GetConfigInfo(const std::string & strKey, const std::string & strDefault = "");

	bool GetMapConfigInfo(std::map<std::string, HmsDbMap> & mapMapConfig, std::string configName = "MapConfig");

    void SetMapPathModifyCallback(const std::function < std::string(const std::string &, const std::string &configName)> & cb){ m_funcModify = cb; }

	bool GetDeviceConnectInfo(const std::string & strKey, HmsDeviceConnectInfo & deviceInfo);
// 
// 	template<typename T>
// 	bool GetConfigInfo(const QString & strKey, QVector<T> & vT, QList<QStructDescription> & listDescription);
// 	template<typename T>
// 	void AddConfigInfo(const QString & strKey, const T & t, QList<QStructDescription> & listDescription); 
	void UpdateDynamicPathInfo(const std::function<std::string(const std::string &)> & callback);

	bool GetAdminAreasDbInfo(std::vector<HmsAdminAreaDbInfo> & vDbInfo);

    bool GetTawsColorStyleInfo(const std::string& xmlPath, std::vector<warningColorStyle*>& vWcs);
protected:
	bool Save();

	bool Open();

	void CheckXML();

	tinyxml2::XMLElement* FindXmlElement(const std::vector<std::string> & strList);
	tinyxml2::XMLElement* FindXmlElement(const std::string & strKey);
	tinyxml2::XMLElement* FindXmlElement(const std::string & strKey, std::string & strLastGroup);
	tinyxml2::XMLElement* FindChildElement(tinyxml2::XMLElement * parentElement, const std::string & strTagName);

	void SetElementText(tinyxml2::XMLElement * domElement, const std::string & strData);

	std::string GetString(tinyxml2::XMLElement * element, const char * strAttribute);

    bool GetMapConfigDetail(std::map<std::string, HmsDbMap> & mapMapConfig, std::string configPath, std::string configName, std::string displayName);

public:
	//string
	static std::string ReplaceParamPath(const std::string & str, const std::string & strParam, const std::string & strData);
	static void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
	static std::string DoubleToString(double num);
	static std::string IntToString(int num);

	std::string ReplaceDynamicPath(const std::string & strPath);

protected:
	bool CreateXml();
	void DeCodeDynamicInfo();
	

public:
	std::string				m_xmlPath;
	std::string				m_strRootText;
	bool					m_bOpen;

	tinyxml2::XMLDocument			m_xmlDom;
	tinyxml2::XMLElement			*m_eleRoot;

	static CHmsConfigXMLReader * s_pConfigXMLReader;

    std::function<std::string(const std::string &, const std::string &configName)>  m_funcModify;

	std::map<std::string, std::string>												m_mapDynamicPath;
	bool																			m_bDynamicPathDirty;
};

#if 0
template<typename T>
bool CHmsConfigXMLReader::GetConfigInfo(const QString & strKey, QVector<T> & vT, QList<QStructDescription> & listDescription)
{
	bool bRet = false;

	if (listDescription.count() <= 0)
	{
		return bRet;
	}

	QString strGroup;
	QDomElement domKey = FindElement(strKey, strGroup);

	if (domKey.isNull())
	{
		return bRet;
	}

	QDomNode n = domKey.firstChild();
	while(!n.isNull())
	{
		QDomElement element = n.toElement();
		if (element.isNull())
		{
			break;
		}
		else if (element.tagName() == strGroup)
		{
			T t;
			bool bData = false;
			foreach(QStructDescription des, listDescription)
			{
				QDomElement tempDom = FindChildElement(element, des.strKey);
				if (tempDom.isNull())
				{
					continue;
				}

				switch(des.field)
				{
				case QStructDescription::QStruct_Int:
					*((int*)((char*)&t + des.offset)) = tempDom.text().toInt();
					break;
				case QStructDescription::QStruct_Float:
					*((float*)((char*)&t + des.offset)) = tempDom.text().toFloat();
					break;
				case QStructDescription::QStruct_Double:
					*((double*)((char*)&t + des.offset)) = tempDom.text().toDouble();
					break;
				case QStructDescription::QStruct_String:
					*((QString*)((char*)&t + des.offset)) = tempDom.text();
					break;
				}

				bData = true;
			}

			vT.push_back(t);
		}
		n = n.nextSibling();
	}			 
	return bRet;
}

template<typename T>
void CHmsConfigXMLReader::AddConfigInfo(const QString & strKey, const T & t, QList<QStructDescription> & listDescription)
{
	if (listDescription.count() <= 0)
	{
		return;
	}

	QString strGroup;
	QString strWrite;
	QDomElement domKey = FindElement(strKey, strGroup);

	if (domKey.isNull())
	{
		return;
	}

	QDomElement domGroup = m_doc.createElement(strGroup);
	domKey.appendChild(domGroup);

	foreach(QStructDescription des, listDescription)
	{
		QDomElement tempDom = m_doc.createElement(des.strKey);
		domGroup.appendChild(tempDom);
		strWrite.clear();

		switch(des.field)
		{
		case QStructDescription::QStruct_Int:
			strWrite.setNum(*((int*)((char*)&t + des.offset)));
			break;
		case QStructDescription::QStruct_Float:
			strWrite.setNum(*((float*)((char*)&t + des.offset)));
			break;
		case QStructDescription::QStruct_Double:
			strWrite.setNum(*((double*)((char*)&t + des.offset)));
			break;
		case QStructDescription::QStruct_String:
			strWrite = *((QString*)((char*)&t + des.offset));
			break;
		}
		SetText(tempDom, strWrite);
	}

	Save();
}

#endif