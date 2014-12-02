//
//  ParentSelectionMechanism.h
//  EvolverController
//
//  Created by Berend Weel on 23/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef EvolverController_MateSelectionMechanism_h
#define EvolverController_MateSelectionMechanism_h

#include <vector>
#include "Organism.h"

class ParentSelectionMechanism {
public:
    virtual std::vector<id_t> selectParents(std::vector<Organism> candidates) = 0;
    
    static std::unique_ptr<ParentSelectionMechanism> getParentSelectionMechanism();
};

#endif
