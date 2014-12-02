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
    if (worldModel.adult && worldModel.fertile)
    {
        // sending
        if (worldModel.now - lastFitnessSent > SPREAD_FITNESS_INTERVAL)
        {
            // spread genome and fitness
            std::string genomeMessage = "[GENOME_SPREAD_MESSAGE]";
            genomeMessage = MessagesManager::add(genomeMessage, "ID", std::to_string(worldModel.organismId));
            genomeMessage = MessagesManager::add(genomeMessage, "FITNESS", std::to_string(fitness.first));
            genomeMessage = MessagesManager::add(genomeMessage, "GENOME", worldModel.genome);
            genomeMessage = MessagesManager::add(genomeMessage, "MIND", worldModel.mindGenome);
            
            genomeEmitter->send(genomeMessage.c_str(), (int)genomeMessage.length()+1);
            
            lastFitnessSent = worldModel.now;
        }
        
        // receiving
        
        while (genomeReceiver->getQueueLength() > 0)
        {
            std::string message = (char*)genomeReceiver->getData();
            
            if (message.substr(0,23).compare("[GENOME_SPREAD_MESSAGE]") == 0)
            {
                id_t mateId;
                double mateFitness;
                std::string mateGenome;
                std::string mateMind;
                readMateMessage(message, & mateId, & mateFitness, & mateGenome, & mateMind);
                
                updateOrganismsToMateWithList(mateId, mateFitness, mateGenome, mateMind);
            }
            
            genomeReceiver->nextPacket();
        }
        
        // mate
        
        if (now - lastMating > INDIVIDUAL_MATING_TIME)
        {
            logNumberOfReceivedGenomes();
            
            if (organismsToMateWith.size() > 0)
            {
                id_t mateId = selectMate();
                int mateIndex = searchForOrganism(mateId);
                
                if (mateIndex >= 0)
                {
                    // send couple of genomes to evolver
                    int backup_channel = _emitter->getChannel();
                    _emitter->setChannel(EVOLVER_CHANNEL);
                    
                    std::string message = "[COUPLE_MESSAGE]";
                    message = MessagesManager::add(message, "ID1", std::to_string(organismId));
                    message = MessagesManager::add(message, "FITNESS1", std::to_string(fitness.first));
                    message = MessagesManager::add(message, "GENOME1", genome);
                    message = MessagesManager::add(message, "MIND1", mindGenome);
                    message = MessagesManager::add(message, "ID2", std::to_string(mateId));
                    message = MessagesManager::add(message, "FITNESS2", std::to_string(organismsToMateWith[mateIndex].getFitness()));
                    message = MessagesManager::add(message, "GENOME2", organismsToMateWith[mateIndex].getGenome());
                    message = MessagesManager::add(message, "MIND2", organismsToMateWith[mateIndex].getMind());
                    
                    _emitter->send(message.c_str(), (int)message.length()+1);
                    _emitter->setChannel(backup_channel);
                    
                    std::cout << organismId << " chose to mate with " << mateId << std::endl;
                    const double * values = _gps->getValues();
                    storeReproductionLocation(to_string(mateId), to_string(values[0])+" "+to_string(values[2]));
                }
                else
                {
                    ofstream file;
                    file.open(RESULTS_PATH + simulationDateAndTime + "/roombot_list_problems.txt", ios::app);
                    file << "TIME: " << getTime() << std::endl;
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
            
            organismsToMateWith = std::vector<Organism>();
            
            lastMating = getTime();
        }
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
    std::cout << "SELECT MATE - start" << std::endl;
    std::vector<id_t> selected = parentSelectionMechanism->selectParents(organismsToMateWith);
    std::cout << "SELECT MATE - selected" << std::endl;
    if (selected.size() > 0)
    {
        std::cout << "SELECT MATE - return first" << std::endl;
        return selected[0];
    }
    std::cout << "SELECT MATE - return 0" << std::endl;
    return 0;
}


int ProximityMating::searchForOrganism(id_t organismId)
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