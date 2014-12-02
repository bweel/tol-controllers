#ifndef shared_ParametersReader_h
#define shared_ParametersReader_h

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>


class ParametersReader {
private:
    static boost::property_tree::ptree * parametersTree;
    
public:
    
    ParametersReader() {
        parametersTree = NULL;
    }
    
    ~ParametersReader() {
        if(parametersTree != NULL){
            delete parametersTree;
            parametersTree = NULL;
        }
    }
    
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
    
    template<typename T>
    static T get(std::string parameterName, T defaultValue)
    {
        if(parametersTree == NULL){
            std::string path = "../../parameters.json";
            
            parametersTree = new boost::property_tree::ptree();
            boost::property_tree::read_json(path, (* parametersTree));
        }
        
        T parameter = parametersTree->get<T>(parameterName, defaultValue);
        
        return parameter;
    }
    
};

#endif