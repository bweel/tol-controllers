//
//  RandomSelection.h
//  EvolverController
//
//  Created by Emanuele Crosato on 02/06/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef EvolverController_RandomSelection_h
#define EvolverController_RandomSelection_h

#include <iostream>
#include "ParentSelectionMechanism.h"

class RandomSelection : public ParentSelectionMechanism {
    virtual std::vector<id_t> selectParents(std::vector<Organism> candidates);
};

#endif
