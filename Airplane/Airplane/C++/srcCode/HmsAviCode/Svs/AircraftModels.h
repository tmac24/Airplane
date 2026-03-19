#include <unordered_map>
#include <string>
#include <vector>

namespace aft_models {

    struct stru_model_filepath
    {
        int _idx;
        std::string _modelName;
        std::string _filepath;
    };
   
	std::vector<stru_model_filepath> * GetSingleModelInfo();
}