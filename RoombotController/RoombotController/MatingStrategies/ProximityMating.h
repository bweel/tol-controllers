//
//  ProximityMating.h
//  RoombotController
//
//  Created by Berend Weel on 28/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef __RoombotController__ProximityMating__
#define __RoombotController__ProximityMating__

#include "MatingStrategy.h"
#include "MessageHandler.h"

class ProximityMating : public MatingStrategy {
public:
    double SPREAD_FITNESS_INTERVAL;
    
    ProximityMating(WorldModel worldModel, MessageHandler &messageHandler) : MatingStrategy(worldModel, messageHandler) {
        lastFitnessSent = 0;
    }
    
    virtual void findMates();
    
private:
    void readMateMessage(std::string message, id_t &mateId, double &mateFitness, std::string &mateGenome, std::string &mateMind);
    void updateOrganismsToMateWithList(id_t mateId, double mateFitness, std::string mateGenome, std::string mateMind);
    id_t selectMate();
    int searchForOrganism(id_t organismId);
    
    double lastFitnessSent;
};

#endif /* defined(__RoombotController__ProximityMating__) */
