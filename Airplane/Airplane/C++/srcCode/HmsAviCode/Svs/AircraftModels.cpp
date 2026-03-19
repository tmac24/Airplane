#include "AircraftModels.h"
#include "external/tinyxml2/tinyxml2.h"
#include "base/HmsFileUtils.h"
#include <sstream>

namespace aft_models {

    static std::vector<stru_model_filepath>  * s_pModels = nullptr;

    std::string GetModelConfigPath()
    {
        std::string path;
        do
        {
            path = HmsAviPf::CHmsFileUtils::getInstance()->fullPathForFilename("ModelConfig.xml");
            if (HmsAviPf::CHmsFileUtils::getInstance()->isFileExist(path))
            {
                break;
            }

            path = HmsAviPf::CHmsFileUtils::getInstance()->fullPathForFilename("ModelConfigNative.xml");
            if (HmsAviPf::CHmsFileUtils::getInstance()->isFileExist(path))
            {
                break;
            }

            return std::string("ModelConfig.xml");

        } while (0);

        return path;
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

    void InitModelVector()
    {
        auto path = GetModelConfigPath();
        tinyxml2::XMLDocument doc1;
#if 1
        HmsAviPf::Data data = HmsAviPf::CHmsFileUtils::getInstance()->getDataFromFile(path);
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
            auto model = root->FirstChildElement("Model");
            if (nullptr == model && strText == "Model")
            {
                model = root;
            }

            auto aircraftModel = model->FirstChildElement("AircraftModel");
            if (aircraftModel)
            {
                auto aircraft = aircraftModel->FirstChildElement("Aircraft");
                while (aircraft)
                {
                    stru_model_filepath ld;
                    GetXmlData(aircraft, "Id", ld._idx);
                    ld._modelName = GetXmlString(aircraft, "ModelName");
                    ld._filepath = GetXmlString(aircraft, "ModelPath");

                    if (s_pModels)
                        s_pModels->push_back(ld);
                    aircraft = aircraft->NextSiblingElement();
                }
            }
        }
    }

    std::vector<aft_models::stru_model_filepath> * GetSingleModelInfo()
    {
        if (nullptr == s_pModels)
        {
            s_pModels = new std::vector<stru_model_filepath>;
            InitModelVector();
        }
        return s_pModels;
    }

}