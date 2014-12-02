//
//  MatingStrategy.cpp
//  RoombotController
//
//  Created by Berend Weel on 28/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "MatingStrategy.h"
#include "ProximityMating.h"

std::unique_ptr<MatingStrategy> MatingStrategy::getMatingStrategy(WorldModel &worldModel, MessageHandler &messageHandler) {
    std::string mateSelection = ParametersReader::get<std::string>("MATING_SELECTION");

    if(mateSelection == "EVOLVER") {
        
    } else if(mateSelection == "ORGANISMS"){
        return std::unique_ptr<MatingStrategy>(new ProximityMating(worldModel, messageHandler));
    } else {
        std::cerr << "Unknown Mating Selection Mechanism: " << mateSelection << std::endl;
    }
    
    return std::unique_ptr<MatingStrategy>();
}
