//
//  MatingStrategy.h
//  RoombotController
//
//  Created by Berend Weel on 25/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef RoombotController_MatingStrategy_h
#define RoombotController_MatingStrategy_h

#include "WorldModel.h"
#include "Organism.h"
#include "MessageHandler.h"

class MatingStrategy {
public:
    MatingStrategy(WorldModel &wm, MessageHandler &mh) :
        worldModel(wm),
        messageHandler(mh)
    {
        
    }
    
    virtual void findMates() = 0;
    
    std::vector<Organism> getCandidates() {
        return organismsToMateWith;
    }
    
    static std::unique_ptr<MatingStrategy> getMatingStrategy(WorldModel &worldModel, MessageHandler &messageHandler);
    
protected:
    WorldModel &worldModel;
    MessageHandler &messageHandler;
    std::vector<Organism> organismsToMateWith;
};

#endif
