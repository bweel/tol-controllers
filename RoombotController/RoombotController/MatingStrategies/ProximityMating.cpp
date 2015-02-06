//
//  ProximityMating.cpp
//  RoombotController
//
//  Created by Berend Weel on 28/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "ProximityMating.h"
#include "MessagesManager.h"

void ProximityMating::findMates() {
    if (worldModel.adult && worldModel.fertile) {
        broadcastGenome();
        
        receiveGenomes();
    }
}

void ProximityMating::mate() {
    storeNumberOfReceivedGenomes();
    if (organismsToMateWith.size() > 0) {
        id_t mateId = selectMate();
        // send couple of genomes to evolver
        sendCoupleMessage(mateId);
        
        std::cout << worldModel.organismId << " chose to mate with " << mateId << std::endl;
        storeReproductionLocation(to_string(mateId), to_string(worldModel.position.x)+" "+to_string(worldModel.position.z));
    }
    
    organismsToMateWith = std::vector<Organism>();
}

void ProximityMating::receiveGenomes() {
    while (messageHandler.hasMessage()) {
        std::string message = messageHandler.receive();
        
        if (message.substr(0,23).compare("[GENOME_SPREAD_MESSAGE]") == 0) {
            id_t mateId;
            double mateFitness;
            std::string mateGenome;
            std::string mateMind;
            readMateMessage(message, mateId, mateFitness, mateGenome, mateMind);
            
            updateOrganismsToMateWithList(mateId, mateFitness, mateGenome, mateMind);
        }
        
        messageHandler.next();
    }
}

void ProximityMating::broadcastGenome() {
    if (worldModel.now - lastFitnessSent > SPREAD_FITNESS_INTERVAL)
    {
        // spread genome and fitness
        std::string genomeMessage = "[GENOME_SPREAD_MESSAGE]";
        genomeMessage = MessagesManager::add(genomeMessage, "ID", std::to_string(worldModel.organismId));
        genomeMessage = MessagesManager::add(genomeMessage, "FITNESS", std::to_string(worldModel.adultFitness));
        genomeMessage = MessagesManager::add(genomeMessage, "GENOME", worldModel.bodyGenome);
        genomeMessage = MessagesManager::add(genomeMessage, "MIND", worldModel.mindGenome);
        
        messageHandler.send(genomeMessage);
        
        lastFitnessSent = worldModel.now;
    }
}

void ProximityMating::readMateMessage(std::string message, id_t &mateId, double &mateFitness, std::string &mateGenome, std::string &mateMind) {
    mateId = std::atoi(MessagesManager::get(message, "ID").c_str());
    mateFitness = std::atof(MessagesManager::get(message, "FITNESS").c_str());
    mateGenome = MessagesManager::get(message, "GENOME");
    mateMind = MessagesManager::get(message, "MIND");
}

void ProximityMating::updateOrganismsToMateWithList(id_t mateId, double mateFitness, std::string mateGenome, std::string mateMind)
{
    for (int i = 0; i < organismsToMateWith.size(); i++)
    {
        if (organismsToMateWith[i].getId() == mateId)
        {
            organismsToMateWith[i].setFitness(mateFitness);
            return;
        }
    }
    
    Organism newMate = Organism(mateGenome, mateMind, mateId, mateFitness, 0, 0, std::vector<id_t>(), Organism::ADULT, true);
    organismsToMateWith.push_back(newMate);
    
    storeMatingLocation(to_string(mateId), to_string(worldModel.position.x)+" "+to_string(worldModel.position.y));
}

id_t ProximityMating::selectMate()
{
//    std::cout << "SELECT MATE - start" << std::endl;
    std::vector<id_t> selected = parentSelectionMechanism->selectParents(organismsToMateWith);
//    std::cout << "SELECT MATE - selected" << std::endl;
    if (selected.size() > 0)
    {
//        std::cout << "SELECT MATE - return first" << std::endl;
        return selected[0];
    }
//    std::cout << "SELECT MATE - return 0" << std::endl;
    return 0;
}