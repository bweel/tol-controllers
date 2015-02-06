//
//  RLPowerFitness.h
//  RoombotController
//
//  Created by Berend Weel on 27/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef RoombotController_SpeedFitness_h
#define RoombotController_SpeedFitness_h

#include "FitnessMeasure.h"
#include "JGTL/JGTL_Vector3.h"

using namespace JGTL;

class SpeedFitness : public FitnessMeasure {
public:
    SpeedFitness(WorldModel &worldModel) : FitnessMeasure(worldModel) {
        
    }
    
    virtual double computeFitness() {
        double deltaTime = endTime - startTime;
        double deltaPosition = startPosition.distance(endPosition);
        double speed = deltaPosition / deltaTime;
        return speed;
    }
    
    virtual void markStart () {
        startPosition = worldModel.position;
        startTime = worldModel.now;
    }
    
    virtual void markEnd () {
        endPosition = worldModel.position;
        endTime = worldModel.now;
    }
        
private:
    double startTime;
    double endTime;
    Vector3<double> startPosition;
    Vector3<double> endPosition;
};

#endif
