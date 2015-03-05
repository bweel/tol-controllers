//
//  ProximityMating.cpp
//  RoombotController
//
//  Created by Berend Weel on 28/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "ProximityMating.h"
#include "MessagesManager.h"

ProximityMating::ProximityMating(WorldModel &worldModel, MessageHandler &messageHandler, MessageHandler &evolverMessageHandler) : MatingStrategy(worldModel, messageHandler, evolverMessageHandler) {
    lastFitnessSent = 0;
}

void ProximityMating::findMates() {
    if (worldModel.adult && worldModel.fertile) {
//        logger.debugStream() << "[" << worldModel.now << "] " << worldModel.robotName << " [findMates] ";
        
        broadcastGenome();
        
        receiveGenomes();
    }
}

void ProximityMating::mate() {
    storeNumberOfReceivedGenomes();
    logger.debugStream() << "[" << worldModel.now << "] " << worldModel.robotName << " trying to mate, number of candidates: " << organismsToMateWith.size() << log4cpp::eol;
    if (organismsToMateWith.size() > 0) {
        id_t mateId = selectMate();
        // send couple of genomes to evolver
        sendCoupleMessage(mateId);
        
        logger.debugStream() << worldModel.organismId << " chose to mate with " << mateId;
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
    logger.debugStream() << "[" << worldModel.now << "] " << worldModel.robotName << " [selectMate] " << log4cpp::eol;
    logger.debugStream() << " Organisms to Mate With: [";
    for(int i=0;i<organismsToMateWith.size();i++){
        logger.debugStream() << organismsToMateWith[i].getId() << ", ";
    }
    logger.debugStream() << "] " << log4cpp::eol;
    
    
    std::vector<id_t> selected = parentSelectionMechanism->selectParents(organismsToMateWith);
    if (selected.size() > 0)
    {
        return selected[0];
    }
    return 0;
}