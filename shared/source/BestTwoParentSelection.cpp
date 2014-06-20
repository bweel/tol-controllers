//
//  BestTwoParentSelection.cpp
//  EvolverController
//
//  Created by Berend Weel on 23/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "BestTwoParentSelection.h"

std::vector<id_t> BestTwoParentSelection::selectParents(std::vector<Organism> candidates) {
    
    std::vector<id_t> parents = std::vector<id_t>();
    
    if(candidates.size() == 0){
        return parents;
    }else if (candidates.size() == 1) {
        parents.push_back(candidates[0].getId());
        return parents;
    }
    
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
        return parents;
    }
    parents.push_back(candidates[bestPos].getId());
    
    
    candidates.erase(candidates.begin() + bestPos);
    
    if(candidates.size() == 0){
        return parents;
    }else if (candidates.size() == 1) {
        parents.push_back(candidates[0].getId());
        return parents;
    }

    // second
    bestPos = -1;
    bestFitness = -1;
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
        return parents;
    }
    parents.push_back(candidates[bestPos].getId());
    
    return parents;
}