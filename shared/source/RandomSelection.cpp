//
//  RandomSelection.cpp
//  EvolverController
//
//  Created by Emanuele Crosato on 02/06/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "RandomSelection.h"
#include "Random.h"

std::vector<id_t> RandomSelection::selectParents(std::vector<Organism> candidates) {
    
    std::vector<id_t> parents = std::vector<id_t>();
    
    if(candidates.size() == 0){
        return parents;
    }else if (candidates.size() == 1) {
        parents.push_back(candidates[0].getId());
        return parents;
    }
    
    // Select parent 1
    
    int candidate1 = candidates[rand() % candidates.size()].getId();
    parents.push_back(candidate1);
    
    // Remove the chosen parent from the candidates
    
    candidates.erase(candidates.begin() + candidate1);
    
    if(candidates.size() == 0){
        return parents;
    }else if (candidates.size() == 1) {
        parents.push_back(candidates[0].getId());
        return parents;
    }
    
    // Select parent 2
   
    int candidate2 = candidates[rand() % candidates.size()].getId();
    parents.push_back(candidate2);
    
    return parents;
}