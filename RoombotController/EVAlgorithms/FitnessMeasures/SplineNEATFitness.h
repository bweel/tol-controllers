//
//  SplineNEATFitness.h
//  RoombotController
//
//  Created by Berend Weel on 27/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef RoombotController_SplineNEATFitness_h
#define RoombotController_SplineNEATFitness_h

#include "FitnessMeasure.h"
#include "JGTL/JGTL_Vector3.h"

using namespace JGTL;

class SplineNEATFitness : public FitnessMeasure {
public:
    SplineNEATFitness(WorldModel &worldModel) : FitnessMeasure(worldModel) {
        
    }
    
    virtual double computeFitness() {
        double deltaPosition = startPosition.distanceSquared(endPosition);
        
        return std::pow(2,deltaPosition);
    }
    
    virtual void markStart () {
        startPosition = worldModel.position;
    }
    
    virtual void markEnd () {
        endPosition = worldModel.position;
    }
    
private:
    Vector3<double> startPosition;
    Vector3<double> endPosition;
};

#endif
