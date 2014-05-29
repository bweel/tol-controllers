//
//  BestTwoParentSelection.cpp
//  EvolverController
//
//  Created by Berend Weel on 23/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "BestTwoParentSelection.h"

std::vector<id_t> BestTwoParentSelection::selectParents(std::vector<Organism> candidates) {
    
    std::vector<id_t> forMating = std::vector<id_t>();
    
    // first
    int bestPos = -1;
    double bestFitness = -1;
    for(int i = 0; i < candidates.size(); i++)
    {
        if (candidates[i].getFitness() > bestFitness)
        {
            bestFitness = candidates[i].getFitness();
            bestPos = i;
        }
    }
    if(bestPos == -1)
    {
        return forMating;
    }
    forMating.push_back(candidates[bestPos].getId());
    
    Organism backupBest = candidates[bestPos];
    candidates.push_back(backupBest);

    // second
    bestPos = -1;
    bestFitness = -1;
    for(int i = 0; i < candidates.size(); i++)
    {
        if (candidates[i].getId() != backupBest.getId() &&
            candidates[i].getFitness() > bestFitness)
        {
            bestFitness = candidates[i].getFitness();
            bestPos = i;
        }
    }
    
    if(bestPos == -1)
    {
        return forMating;
    }
    
    forMating.push_back(candidates[bestPos].getId());
    return forMating;
}