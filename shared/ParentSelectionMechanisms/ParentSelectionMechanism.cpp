//
//  ParentSelectionMechanism.cpp
//  RoombotController
//
//  Created by Berend Weel on 28/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "ParentSelectionMechanism.h"
#include "BestTwoParentSelection.h"
#include "BinaryTournamentParentSelection.h"
#include "RandomSelection.h"

std::unique_ptr<ParentSelectionMechanism> ParentSelectionMechanism::getParentSelectionMechanism() {
    std::string parentSelection = ParametersReader::get<std::string>("PARENT_SELECTION");
    if(parentSelection == "BESTTWO") {
        return std::unique_ptr<ParentSelectionMechanism>(new BestTwoParentSelection());
    } else if(parentSelection == "BINARY_TOURNAMENT"){
        return std::unique_ptr<ParentSelectionMechanism>(new BinaryTournamentParentSelection());
    } else if (parentSelection == "RANDOM") {
        return std::unique_ptr<ParentSelectionMechanism>(new RandomSelection());
    } else {
        std::cerr << "Unknown Parent Selection Mechanism: " << std::endl;
    }
    
    return std::unique_ptr<ParentSelectionMechanism>();
}
