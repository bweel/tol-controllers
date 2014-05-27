#ifndef shared_ParametersReader_h
#define shared_ParametersReader_h

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>


class ParametersReader {
private:
    static boost::property_tree::ptree * parametersTree;
    
public:
    
    template<typename T>
    static T get(std::string parameterName)
    {
        if(parametersTree == NULL){
            std::string path = "../../parameters.json";
        
            parametersTree = new boost::property_tree::ptree();
            boost::property_tree::read_json(path, (* parametersTree));
        }
        
        T parameter = parametersTree->get<T>(parameterName);
        
        return parameter;
    }
    
};

#endif