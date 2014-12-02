//
//  LearningController.h
//  RoombotController
//
//  Created by Berend Weel on 21/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef __RoombotController__LearningController__
#define __RoombotController__LearningController__

#include "Defines.h"
#include "WorldModel.h"
#include "LearningAlgorithm.h"
#include "JGTL/JGTL_Vector3.h"
#include "FitnessMeasure.h"

class LearningController {
public:
    static const int GPS_LOG_PERIOD;                // gps logging period (a print every tot steps)
    
    LearningController(WorldModel &wm);
    virtual ~LearningController();
    void initialise();
    void finalise();
    
    void step();
    
    std::vector<std::vector<double> > computeAngles(const std::vector<std::vector<double> > & angles);
    
private:
    void logGPS();
    
    std::pair<double, std::string> computeFitness(double d_t, const JGTL::Vector3<double> distance);
    
    std::string initialiseLogDirectory(const std::string & directory, const std::string & name, const std::string & algorithm, const std::string &logdir);
    JGTL::Vector3<double> initialiseShapeSize(const boost::property_tree::ptree & root);
    std::vector<JGTL::Vector3<double> > initialiseModuleMapping(std::size_t offset, std::size_t size, const std::string & name, const boost::property_tree::ptree & root);
    
    WorldModel &worldModel;
    double motorAngularVelocity;
    
    unsigned int evaluationStep;
    unsigned int evaluationSteps;
    unsigned int recoverySteps;
    
    JGTL::Vector3<double> startPosition;
    
    std::unique_ptr<LearningAlgorithm> learningAlgorithm;
    std::unique_ptr<FitnessMeasure> fitnessMeasure;
    
    std::ofstream gpsLog; // GPS log
    std::ofstream fitnessLog;
};

#endif /* defined(__RoombotController__LearningController__) */
