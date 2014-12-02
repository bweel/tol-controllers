//
//  WorldModel.h
//  RoombotController
//
//  Created by Berend Weel on 20/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef RoombotController_WorldModel_h
#define RoombotController_WorldModel_h

#include <vector>
#include <sstream>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Defines.h"
#include "JGTL/JGTL_Vector3.h"

class WorldModel {
public:
    double TIME_STEP;
    
    unsigned int organismId;
    unsigned int robotIndex;
    unsigned int numMotors;
    unsigned int robotType;
    
    unsigned int timeToLive;
    unsigned int organismSize;
    
    double _time_start;                         // time - start
    double lifetimeStart;                        // time - offset
    double now;
    double _time_end;                           // time - end
    
    bool fertile;
    bool adult;
    
    std::string robotName;
    std::string simulationDateAndTime;
    std::string projectPath;
    std::string logDirectory;
    std::string parametersPath;
    std::string savePath;
    
    boost::property_tree::ptree parameters;  // parameters tree
    
    std::string mindGenome;
    std::string bodyGenome;
    
    JGTL::Vector3<double> position;

    // check if a module is root
    bool iAmRoot() {
        return robotType == ModuleTypes::M_ROOT;
    }
    
    void initialiseParameters(const std::string & path)
    {
        boost::property_tree::read_json(path, parameters);
        
        robotName               = parameters.get<std::string>("Robot.Name");
        organismSize            = parameters.get<unsigned int>("Robot.Modules_#");
        numMotors               = parameters.get<unsigned int>("Robot.Motor.Number");
        
        std::istringstream(robotName.substr(robotName.find("_") + 1, robotName.length())) >> organismId;
        
        timeToLive              = parameters.get<unsigned int>("Algorithm.Time_To_Live");
        simulationDateAndTime   = parameters.get<std::string>("Simulation");
        
        parametersPath          = parameters.get<std::string>("Algorithm.Parameters");
        savePath                = parameters.get<std::string>("Algorithm.Save");
        
        logDirectory            = parameters.get<std::string>("Algorithm.LogDir","");
        
        // GenomeTransmition
        bodyGenome              = parameters.get<std::string>("Genome");
        mindGenome              = parameters.get<std::string>("MindGenome");
    }
};

#endif
