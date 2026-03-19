#include "HmsConfigXMLReader.h"
#include "math/HmsMath.h"
#include "base/HmsFileUtils.h"
#include "HmsLog.h"
using namespace tinyxml2;

#include <sstream>

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

CHmsConfigXMLReader * CHmsConfigXMLReader::s_pConfigXMLReader = NULL;

CHmsConfigXMLReader::CHmsConfigXMLReader(const std::string & strPath)
: m_bOpen(false), m_bDynamicPathDirty(true), m_strRootText("AVIConfig")
{
    m_xmlPath = strPath;
}

CHmsConfigXMLReader::CHmsConfigXMLReader()
: m_bOpen(false), m_bDynamicPathDirty(true), m_xmlPath("HmsAviConfig.xml"), m_strRootText("AVIConfig")
{

}

CHmsConfigXMLReader::CHmsConfigXMLReader(const std::string & strPath, const std::string & strRootText)
: m_bOpen(false), m_bDynamicPathDirty(true), m_xmlPath(strPath), m_strRootText(strRootText)
{

}

CHmsConfigXMLReader::~CHmsConfigXMLReader(void)
{
}

CHmsConfigXMLReader * CHmsConfigXMLReader::GetSingleConfig()
{
    if (NULL == s_pConfigXMLReader)
    {
        s_pConfigXMLReader = new CHmsConfigXMLReader();
    }
    return s_pConfigXMLReader;
}

CHmsConfigXMLReader * CHmsConfigXMLReader::GetSingleConfig(const std::string & strPath)
{
    if (NULL == s_pConfigXMLReader)
    {
        s_pConfigXMLReader = new CHmsConfigXMLReader(strPath);
    }
    return s_pConfigXMLReader;
}

void CHmsConfigXMLReader::ReleaseSingleConfig()
{
    if (s_pConfigXMLReader)
    {
        delete s_pConfigXMLReader;
        s_pConfigXMLReader = NULL;
    }
}

std::string CHmsConfigXMLReader::GetConfigFileFolder()
{
    std::string strOut;
    std::string::size_type pos = 0;
    if (m_xmlPath.size() > 0)
    {
        pos = m_xmlPath.length();

        std::string::size_type pos1 = m_xmlPath.rfind('\\', pos);
        std::string::size_type pos2 = m_xmlPath.rfind('/', pos);

        if ((pos1 != std::string::npos) && (pos2 != std::string::npos))
        {
            pos = HMS_MAX(pos1, pos2);
        }
        else if (pos1 != std::string::npos)
        {
            pos = pos1;
        }
        else if (pos2 != std::string::npos)
        {
            pos = pos2;
        }
        else
        {
            pos = 0;
        }
    }

    if (pos > 0)
    {
        strOut = m_xmlPath.substr(0, pos + 1);
    }

    return strOut;
}

std::string CHmsConfigXMLReader::GetOtherConfigFilePath(const std::string & strFileName)
{
    return GetConfigFileFolder() + strFileName;
}

void CHmsConfigXMLReader::SetPath(const std::string & strPath)
{
    m_xmlPath = strPath;
}

void CHmsConfigXMLReader::SetRootName(const std::string & rootName)
{
    m_strRootText = rootName;
}

int CHmsConfigXMLReader::GetConfigInfo(const std::string & strKey, int nDefault /*= 0*/)
{
    int nRet = nDefault;
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement *find = FindXmlElement(splitStrList);
    const char *temp;

    if (find != nullptr)
    {
        temp = find->GetText();
        if (temp == nullptr)
        {
            std::string strValue = IntToString(nDefault);
            SetElementText(find, strValue);
            Save();
        }
        else
        {
            nRet = std::atoi(temp);
        }
    }
    return nRet;
}

float CHmsConfigXMLReader::GetConfigInfo(const std::string & strKey, float fDefault /*= 0.0*/)
{
    float fRet = fDefault;
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement *find = FindXmlElement(splitStrList);
    const char *temp;

    if (find != nullptr)
    {
        temp = find->GetText();
        if (temp == nullptr)
        {
            std::string strValue = DoubleToString(fDefault);
            SetElementText(find, strValue);
            Save();
        }
        else
        {
            fRet = std::atof(temp);
        }
    }
    return fRet;
}

double CHmsConfigXMLReader::GetConfigInfo(const std::string & strKey, double dDefault /*= 0.0*/)
{
    double dRet = dDefault;
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement *find = FindXmlElement(splitStrList);
    const char *temp;

    if (find != nullptr)
    {
        temp = find->GetText();
        if (temp == nullptr)
        {
            std::string strValue = DoubleToString(dDefault);
            SetElementText(find, strValue);
            Save();
        }
        else
        {
            dRet = std::atof(temp);
        }
    }
    return dRet;
}

std::string CHmsConfigXMLReader::GetConfigInfo(const std::string & strKey, const std::string & strDefault /*= ""*/)
{
    std::string strRet = strDefault;
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement* find = FindXmlElement(splitStrList);
    const char *temp;

    if (find != nullptr)
    {
        temp = find->GetText();
        if (temp == nullptr)
        {
            SetElementText(find, strRet);
            Save();
        }
        else
        {
            strRet = temp;
        }
    }
    return strRet;
}

bool CHmsConfigXMLReader::Save()
{
    if (m_xmlDom.SaveFile(m_xmlPath.c_str()) == XMLError::XML_SUCCESS)
    {
        return true;
    }

    return false;
}

bool CHmsConfigXMLReader::Open()
{
    bool bRet = false;

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
    HmsAviPf::Data data = HmsAviPf::CHmsFileUtils::getInstance()->getDataFromFile(m_xmlPath);
    if (data.isNull())
        return bRet;

    XMLError err = m_xmlDom.Parse((const char*)data.getBytes(), data.getSize());
#else
    XMLError err = m_xmlDom.LoadFile(m_xmlPath.c_str());
#endif
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

void CHmsConfigXMLReader::CheckXML()
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

std::string CHmsConfigXMLReader::GetString(tinyxml2::XMLElement * element, const char * strAttribute)
{
    std::string strOut;
    if (element)
    {
        const char * str = element->Attribute(strAttribute);
        if (str != nullptr)
        {
            strOut = str;
        }
    }
    return strOut;
};

XMLElement* CHmsConfigXMLReader::FindXmlElement(const std::vector<std::string> & strList)
{
    XMLElement *curElement = nullptr;
    XMLElement *tempElement = nullptr;

    if (strList.size() == 0)
    {
        return nullptr;
    }

    CheckXML();
    curElement = m_eleRoot;
    for (std::string str : strList)
    {
        tempElement = FindChildElement(curElement, str);
        if (tempElement == nullptr)
        {
#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID) || (HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS)
            return nullptr;
#else
            tempElement = m_xmlDom.NewElement(str.c_str());
            curElement->InsertEndChild(tempElement);
#endif
        }
        curElement = tempElement;
    }
    return curElement;
}

XMLElement* CHmsConfigXMLReader::FindXmlElement(const std::string & strKey)
{
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement *find = FindXmlElement(splitStrList);

    return find;
}

XMLElement* CHmsConfigXMLReader::FindXmlElement(const std::string & strKey, std::string & strLastGroup)
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

XMLElement* CHmsConfigXMLReader::FindChildElement(XMLElement * parentElement, const std::string & strTagName)
{
    XMLElement *findElement = nullptr;
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
    return findElement;
}

void CHmsConfigXMLReader::SetElementText(XMLElement * domElement, const std::string & strData)
{
    XMLText * text = m_xmlDom.NewText(strData.c_str());
    domElement->InsertEndChild(text);
}

std::string CHmsConfigXMLReader::ReplaceParamPath(const std::string & str, const std::string & strParam, const std::string & strData)
{
    std::string strOut = str;
    std::string::size_type pos = 0;
    std::string::size_type srcLen = strParam.size();

    pos = strOut.find(strParam, pos);
    if ((pos != std::string::npos))
    {
        strOut.replace(pos, srcLen, strData);
    }
    return strOut;
}

bool CHmsConfigXMLReader::CreateXml()
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


void CHmsConfigXMLReader::DeCodeDynamicInfo()
{
    if (m_bDynamicPathDirty)
    {
        m_bDynamicPathDirty = false;
        m_mapDynamicPath.clear();
    }
    else
    {
        return;
    }
    
    std::string aviDataPath = HmsAviPf::CHmsFileUtils::getInstance()->getAviDataPath();

    std::string strKey = "DynamicResInfo";
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement* item = FindXmlElement(splitStrList);
    if (item != nullptr)
    {
        item = item->FirstChildElement();
    }

    while (item != nullptr)
    {
        std::string name = item->Name();
        std::string path = item->GetText();

        if (!name.empty() && !path.empty())
        {
            path = aviDataPath + path;
            m_mapDynamicPath[name] = path;
        }

        item = item->NextSiblingElement();
    }
}

std::string CHmsConfigXMLReader::ReplaceDynamicPath(const std::string & strPathIn)
{
    std::string::size_type posStart, posEnd;
    std::string strPath = strPathIn;

    do
    {
        posStart = strPath.find("${");
        if (std::string::npos == posStart)
        {
            break;
        }

        posEnd = strPath.find("}", posStart);
        if (std::string::npos == posEnd)
        {
            break;
        }

        std::string strSyntax = strPath.substr(posStart, posEnd - posStart + 1);
        if (strSyntax.size() > 3)
        {
            std::string strParam = strSyntax.substr(2, strSyntax.size() - 3);
            auto itFind = m_mapDynamicPath.find(strParam);
            if (itFind != m_mapDynamicPath.end())
            {
                strPath = ReplaceParamPath(strPath, strSyntax, itFind->second);
            }
            else
            {
                HMSLOG("XML can't find the DynamicPath=%s\n", strSyntax.c_str());
                break;
            }
        }

    } while (true);

#if defined(__linux)
    HmsAviPf::CHmsFileUtils::StringReplace(strPath, "\\", "/");
    HmsAviPf::CHmsFileUtils::StringReplace(strPath, "//", "/");
#endif
    return strPath;
}

void CHmsConfigXMLReader::SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
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

std::string CHmsConfigXMLReader::DoubleToString(double num)
{
    char str[256];
    std::sprintf(str, "%lf", num);
    std::string result = str;
    return result;
}

std::string CHmsConfigXMLReader::IntToString(int num)
{
    char str[256];
    std::sprintf(str, "%d", num);
    std::string result = str;
    return result;
}

bool CHmsConfigXMLReader::GetMapConfigDetail(std::map<std::string, HmsDbMap> & mapMapConfig, std::string configPath, 
    std::string configName, std::string displayName)
{
    bool bRet = false;

    tinyxml2::XMLDocument docMapDetail;
    HmsAviPf::Data data = HmsAviPf::CHmsFileUtils::getInstance()->getDataFromFile(configPath);
    if (data.isNull())
    {
        return bRet;
    }
    XMLError err = docMapDetail.Parse((const char*)data.getBytes(), data.getSize());
    if (err != XMLError::XML_SUCCESS)
    {
        return bRet;
    }
    auto root = docMapDetail.RootElement();
    if (!root)
    {
        return bRet;
    }

    XMLElement* item = root->FirstChildElement("MapItem");

    auto DecodeMapTable = [this, configName](XMLElement * mapTable, HmsDbMap & dbMap)
    {
        HmsDbMapLayer mapLayer;
        int nTempLayer = 1;
        mapLayer.strDbPath = GetString(mapTable, "Path");
        mapLayer.strDbPath = ReplaceDynamicPath(mapLayer.strDbPath);

        int nMin = 1;
        int nMax = 1;
        GetXmlData(mapTable, "LayerMin", nMin);
        GetXmlData(mapTable, "LayerMax", nMax);

        if (nMin == nMax)
        {
            nTempLayer = nMin;
            XMLElement * mapSubLayer = mapTable->FirstChildElement("MapLayer");
            while (mapSubLayer)
            {
                HmsDbSubMapLayer subData;
                int nLayer = 1;
                subData.strDbPath = GetString(mapSubLayer, "Path");
                GetXmlData(mapSubLayer, "Layer", nLayer);
                GetXmlData(mapSubLayer, "XMin", subData.xMin);
                GetXmlData(mapSubLayer, "XMax", subData.xMax);
                GetXmlData(mapSubLayer, "YMin", subData.yMin);
                GetXmlData(mapSubLayer, "YMax", subData.yMax);

                subData.strDbPath = ReplaceDynamicPath(subData.strDbPath);

                if (nLayer == nTempLayer)
                {
                    mapLayer.vDbPath.push_back(subData);
                    mapLayer.isSubdivision = true;
                }

                mapSubLayer = mapSubLayer->NextSiblingElement();
            }
            mapLayer.nLayer = nTempLayer;
            dbMap.mapLayer[nTempLayer] = mapLayer;
        }
        else
        {
            mapLayer.isSubdivision = false;
            for (int i = nMin; i <= nMax; i++)
            {
                mapLayer.nLayer = i;
                dbMap.mapLayer[i] = mapLayer;
            }
        }
    };

    while (item != nullptr)
    {
        HmsDbMap dbMap;
        dbMap.strDisplayName = displayName;
        dbMap.strMapType = GetString(item, "MapType");
        XMLElement * mapTable = item->FirstChildElement("MapTable");

        while (mapTable)
        {
            DecodeMapTable(mapTable, dbMap);

            mapTable = mapTable->NextSiblingElement();
        }

        if (!dbMap.mapLayer.empty())
        {
            dbMap.layerMin = dbMap.mapLayer.begin()->first;
            dbMap.layerMax = (--dbMap.mapLayer.end())->first;
        }
        else
        {
            dbMap.layerMin = 0;
            dbMap.layerMax = 0;
        }
        mapMapConfig[dbMap.strMapType] = dbMap;
        bRet = true;

        item = item->NextSiblingElement();
    }

    return bRet;
}

bool CHmsConfigXMLReader::GetMapConfigInfo(std::map<std::string, HmsDbMap> & mapMapConfig, std::string configName)
{
    bool bRet = false;

    std::string strKey = configName + "/MapItem";
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement* itemPath = FindXmlElement(splitStrList);
    while (itemPath)
    {
        std::string displayName = GetString(itemPath, "displayName");
        std::string configPath = GetString(itemPath, "ConfigPath");
        int nUsed = 0;
        GetXmlData(itemPath, "IsUse", nUsed);
        if (nUsed != 0)
        {
            GetMapConfigDetail(mapMapConfig, configPath, configName, displayName);
        }
        itemPath = itemPath->NextSiblingElement();
        bRet = true;
    }

    return bRet;
}

bool CHmsConfigXMLReader::GetDeviceConnectInfo(const std::string & strKey, HmsDeviceConnectInfo & deviceInfo)
{
    bool bRet = false;

    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement* item = FindXmlElement(splitStrList);

    if (item != nullptr)
    {
        deviceInfo.strProtocols = GetString(item, "Protocols");
        deviceInfo.strServerIp = GetString(item, "ServerIp");
        GetXmlData(item, "Redirect", deviceInfo.nRedirect);

        bRet = true;
    }

    return bRet;
}

void CHmsConfigXMLReader::UpdateDynamicPathInfo(const std::function<std::string(const std::string &)> & callback)
{
    DeCodeDynamicInfo();
    if (callback)
    {
        for (auto & c : m_mapDynamicPath)
        {
            c.second = callback(c.second);
        }
    }
}

bool CHmsConfigXMLReader::GetAdminAreasDbInfo(std::vector<HmsAdminAreaDbInfo> & vDbInfo)
{
    bool bRet = false;

    DeCodeDynamicInfo();

    std::string strKey = "AdministrativeConfig/AdminItem";
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement* item = FindXmlElement(splitStrList);

    while (item)
    {
        HmsAdminAreaDbInfo aa;

        int nUsed = 0;
        GetXmlData(item, "IsUse", nUsed);
        aa.strName = GetString(item, "Name");
        aa.strDataType = GetString(item, "DataType");
        aa.strDbPath = GetString(item, "Path");
        aa.bUsed = (nUsed == 0) ? false : true;

        aa.strDbPath = ReplaceDynamicPath(aa.strDbPath);
        vDbInfo.push_back(aa);
        bRet = true;

        item = item->NextSiblingElement();
    }
    return bRet;
}

bool CHmsConfigXMLReader::GetTawsColorStyleInfo(const std::string& xmlPath, std::vector<warningColorStyle*>& vWcs)
{
    bool bRet = false;

    //std::string strKey = "TAWS_WarningColorConfig/Profile";
    std::string strKey = xmlPath;
    std::vector<std::string> splitStrList;
    SplitString(strKey, splitStrList, "/");

    XMLElement* item = FindXmlElement(splitStrList);

    while (item)
    {
        std::vector<float> valt;
        std::vector<HmsAviPf::Vec4> vclr;

        HmsAviPf::Vec4 rgb0;
        GetXmlData(item, "r", rgb0.x); GetXmlData(item, "g", rgb0.y); GetXmlData(item, "b", rgb0.z); rgb0.w = 255.0f;
        rgb0 = rgb0 / 255.0f;
        XMLElement* itemLevel = item->FirstChildElement();
        while (itemLevel)
        {
            float alt;
            HmsAviPf::Vec4 rgb1;
            GetXmlData(itemLevel, "relAlt", alt);
            GetXmlData(itemLevel, "r", rgb1.x); GetXmlData(itemLevel, "g", rgb1.y); GetXmlData(itemLevel, "b", rgb1.z); rgb1.w = 255.0f;
            rgb1 = rgb1 / 255.0f;

            valt.push_back(alt);
            vclr.push_back(rgb1);

            itemLevel = itemLevel->NextSiblingElement();

            bRet = true;
        }
        vclr.push_back(rgb0);

        auto pst = warningColorStyle::create(valt, vclr);
        vWcs.push_back(pst);

        item = item->NextSiblingElement();
    }
    return bRet;
}
