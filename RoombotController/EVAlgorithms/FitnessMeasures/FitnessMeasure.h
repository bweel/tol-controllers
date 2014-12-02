//
//  FitnessMeasure.h
//  RoombotController
//
//  Created by Berend Weel on 24/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef RoombotController_FitnessMeasure_h
#define RoombotController_FitnessMeasure_h

#include "WorldModel.h"
#include "JGTL/JGTL_LocatedException.h"

class FitnessMeasure {
public:
    FitnessMeasure(WorldModel &wm) : worldModel(wm) {
        
    }
    
    virtual double computeFitness() = 0;
    virtual void markStart() = 0;
    virtual void markEnd() = 0;
    
    static std::unique_ptr<FitnessMeasure> getFitnessMeasure(std::string learningAlgorithmType, WorldModel &worldModel);
    
protected:
    WorldModel &worldModel;
};

#endif
