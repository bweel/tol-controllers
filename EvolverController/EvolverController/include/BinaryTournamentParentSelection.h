//
//  BinaryTournamentParentSelection.h
//  EvolverController
//
//  Created by Berend Weel on 23/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef __EvolverController__BinaryTournamentParentSelection__
#define __EvolverController__BinaryTournamentParentSelection__

#include <iostream>
#include "ParentSelectionMechanism.h"

class BinaryTournamentParentSelection : public ParentSelectionMechanism {
    virtual std::vector<id_t> selectParents(std::vector<Organism> candidates);
};


#endif /* defined(__EvolverController__BinaryTournamentParentSelection__) */
