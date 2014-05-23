//
//  BinaryTournamentParentSelection.cpp
//  EvolverController
//
//  Created by Berend Weel on 23/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "Random.h"
#include "BinaryTournamentParentSelection.h"

std::vector<id_t> BinaryTournamentParentSelection::selectParents(std::vector<Organism> candidates) {
    std::vector<id_t> parents;
    
    if(candidates.size() == 0){
        return parents;
    }else if (candidates.size() == 1) {
        parents.push_back(candidates[0].getId());
        return parents;
    }
    
    // Select parent 1
    int candidate1 = utils::Random::getInstance()->uniform_integer(0, candidates.size());
    int candidate2 = utils::Random::getInstance()->uniform_integer(0, candidates.size());
    
    if(candidates[candidate1].getFitness() > candidates[candidate2].getFitness()){
        parents.push_back(candidates[candidate1].getId());
    } else {
        parents.push_back(candidates[candidate2].getId());
    }

    // Select parent 2
    candidate1 = utils::Random::getInstance()->uniform_integer(0, candidates.size());
    candidate2 = utils::Random::getInstance()->uniform_integer(0, candidates.size());

    if(candidates[candidate1].getFitness() > candidates[candidate2].getFitness()){
        parents.push_back(candidates[candidate1].getId());
    } else {
        parents.push_back(candidates[candidate2].getId());
    }
    
    return parents;
}