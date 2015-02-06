//
//  MatingStrategy.cpp
//  RoombotController
//
//  Created by Berend Weel on 28/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "MatingStrategy.h"
#include "ProximityMating.h"
#include "MessagesManager.h"

MatingStrategy::MatingStrategy(WorldModel &wm, MessageHandler &mh,MessageHandler &emh) :
worldModel(wm),
messageHandler(mh),
evolverMessageHandler(emh)
{
    matingLocationFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/mating-locations.txt", ios::app);
    numberOfReceivedGenomesFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/organism_" + std::to_string(worldModel.organismId) + "/received_genomes.txt", ios::app);
    reproductionLocationFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/reproduction-locations.txt", ios::app);
    
    parentSelectionMechanism = ParentSelectionMechanism::getParentSelectionMechanism();
}

MatingStrategy::~MatingStrategy() {
    matingLocationFile.close();
    numberOfReceivedGenomesFile.close();
    reproductionLocationFile.close();
}

std::unique_ptr<MatingStrategy> MatingStrategy::getMatingStrategy(WorldModel &worldModel, MessageHandler &messageHandler, MessageHandler &evolverMessageHandler) {
    std::string mateSelection = ParametersReader::get<std::string>("MATING_SELECTION");

    if(mateSelection == "EVOLVER") {
        
    } else if(mateSelection == "ORGANISMS"){
        return std::unique_ptr<MatingStrategy>(new ProximityMating(worldModel, messageHandler, evolverMessageHandler));
    } else {
        std::cerr << "Unknown Mating Selection Mechanism: " << mateSelection << std::endl;
    }
    
    return std::unique_ptr<MatingStrategy>();
}

void MatingStrategy::sendCoupleMessage(id_t mateId) {
    int mateIndex = searchForOrganism(mateId);
    
    if(mateIndex >= 0) {
        std::string message = "[COUPLE_MESSAGE]";
        
        message = MessagesManager::add(message, "ID1", std::to_string(worldModel.organismId));
        message = MessagesManager::add(message, "FITNESS1", std::to_string(worldModel.adultFitness));
        message = MessagesManager::add(message, "GENOME1", worldModel.bodyGenome);
        message = MessagesManager::add(message, "MIND1", worldModel.mindGenome);
        
        message = MessagesManager::add(message, "ID2", std::to_string(mateId));
        message = MessagesManager::add(message, "FITNESS2", std::to_string(organismsToMateWith[mateIndex].getFitness()));
        message = MessagesManager::add(message, "GENOME2", organismsToMateWith[mateIndex].getGenome());
        message = MessagesManager::add(message, "MIND2", organismsToMateWith[mateIndex].getMind());
        
        evolverMessageHandler.send(message);
    } else {
        ofstream file;
        file.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/roombot_list_problems.txt", ios::app);
        file << "TIME: " << worldModel.now << std::endl;
        file << "MATE: " << mateId << std::endl;
        file << "LIST:\n" << std::endl;
        for (int i = 0; i < organismsToMateWith.size(); i++)
        {
            file << " " << organismsToMateWith[i].getId();
        }
        file << std::endl;
        file.close();
    }
}

void MatingStrategy::storeMatingLocation(std::string partner, std::string location) {
    matingLocationFile << worldModel.now << " " << worldModel.robotName << " mated with " << partner << " at " << location << std::endl;
}

void MatingStrategy::storeNumberOfReceivedGenomes() {
    numberOfReceivedGenomesFile << worldModel.now << " " << organismsToMateWith.size() << " ";
    for (int i = 0; i < organismsToMateWith.size(); i++)
    {
        numberOfReceivedGenomesFile << organismsToMateWith[i].getName() << " ";
    }
    numberOfReceivedGenomesFile << std::endl;
}

void MatingStrategy::storeReproductionLocation(std::string partner, std::string location)
{
    reproductionLocationFile << worldModel.now << " " << worldModel.robotName << " chose as partner: " << partner << " at " << location << std::endl;
}

int MatingStrategy::searchForOrganism(id_t organismId)
{
    for(int i = 0; i < organismsToMateWith.size(); i++)
    {
        if (organismsToMateWith[i].getId() == organismId)
        {
            return i;
        }
    }
    return -1;
}