//
//  FitnessMeasure.cpp
//  RoombotController
//
//  Created by Berend Weel on 27/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "FitnessMeasure.h"
#include "HyperNEATFitness.h"
#include "SplineNEATFitness.h"
#include "RLPowerFitness.h"

std::unique_ptr<FitnessMeasure> FitnessMeasure::getFitnessMeasure(std::string learningAlgorithmType, WorldModel &worldModel) {
    if(learningAlgorithmType == "HyperNEAT") {
        return std::unique_ptr<FitnessMeasure>(new HyperNEATFitness(worldModel));
    } else if(learningAlgorithmType == "SplineNEAT") {
        return std::unique_ptr<FitnessMeasure>(new SplineNEATFitness(worldModel));
    } else if(learningAlgorithmType == "HyperNEAT") {
        return std::unique_ptr<FitnessMeasure>(new RLPowerFitness(worldModel));
    } else {
        throw CREATE_LOCATEDEXCEPTION_INFO("Unsupported Fitness Measure");
    }
}