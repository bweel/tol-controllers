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
#include "ParentSelectionMechanism.h"

class MatingStrategy {
public:
    MatingStrategy(WorldModel &wm, MessageHandler &mh, MessageHandler &emh);
    virtual ~MatingStrategy();
    
    virtual void findMates() = 0;
    virtual void mate() = 0;
    
    std::vector<Organism> getCandidates() {
        return organismsToMateWith;
    }
    
    static std::unique_ptr<MatingStrategy> getMatingStrategy(WorldModel &worldModel, MessageHandler &messageHandler, MessageHandler &evolverMessageHandler);
    
protected:
    void storeMatingLocation(std::string partner, std::string location);
    void storeNumberOfReceivedGenomes();
    void storeReproductionLocation(std::string partner, std::string location);
    
    void sendCoupleMessage(id_t mateId);
    
    int searchForOrganism(id_t organismId);
    
    WorldModel &worldModel;
    MessageHandler &messageHandler;
    MessageHandler &evolverMessageHandler;
    std::vector<Organism> organismsToMateWith;
    std::unique_ptr<ParentSelectionMechanism> parentSelectionMechanism;
    
    ofstream matingLocationFile;
    ofstream numberOfReceivedGenomesFile;
    ofstream reproductionLocationFile;
};

#endif
