#ifndef __GENETICSPECIES_H__
#define __GENETICSPECIES_H__

#include "NEAT_Globals.h"
#include "tinyxmlplus.h"

namespace NEAT
{
    /*
     * GeneticSpecies: This class is responsible for handling a species: a group of similar individuals
     */
    class GeneticSpecies
    {
        boost::shared_ptr<GeneticIndividual> bestIndividualEver;

        int ID;

        //Individuals currently in the species.
        vector<boost::shared_ptr<GeneticIndividual> > currentIndividuals;

        double multiplier;

        int age;

        int ageOfLastImprovement;

        int offspringCount;

        bool reproduced;

        double speciesFitness;

        double oldAverageFitness;
    public:
        NEAT_DLL_EXPORT GeneticSpecies(boost::shared_ptr<GeneticIndividual> firstIndividual);

        NEAT_DLL_EXPORT virtual ~GeneticSpecies();

        inline boost::shared_ptr<GeneticIndividual> getBestIndividual()
        {
            return bestIndividualEver;
        }

        NEAT_DLL_EXPORT void setBestIndividual(boost::shared_ptr<GeneticIndividual> ind);

        //This is a hack which is important to make sure the best overall species
        //doesn't get hit with the no-improvement penalty
        void updateAgeOfLastImprovement()
        {
            ageOfLastImprovement = age;
        }

        inline int getID()
        {
            return ID;
        }

        inline const int &getAge()
        {
            return age;
        }

        inline const int &getAgeOfLastImprovement()
        {
            return ageOfLastImprovement;
        }

        inline int getIndividualCount()
        {
            return (int)currentIndividuals.size();
        }

        inline void resetIndividuals()
        {
            currentIndividuals.clear();
        }

        inline void addIndividual(boost::shared_ptr<GeneticIndividual> ind)
        {
            currentIndividuals.push_back(ind);
        }

        inline void setOffspringCount(int _offspringCount)
        {
            offspringCount = _offspringCount;
        }

        inline int getOffspringCount()
        {
            return offspringCount;
        }

        inline void decrementOffspringCount()
        {
            offspringCount--;
        }

        void setMultiplier();

        inline double getMultiplier()
        {
            return multiplier;
        }

        inline bool isReproduced()
        {
            return reproduced;
        }

        inline void setReproduced(bool _reproduced)
        {
            reproduced = _reproduced;
        }

        inline double getFitness()
        {
            return speciesFitness;
        }

        NEAT_DLL_EXPORT void setFitness();

        inline double getAdjustedFitness()
        {
            return speciesFitness*multiplier;
        }

        NEAT_DLL_EXPORT void incrementAge();

        NEAT_DLL_EXPORT void makeBabies(vector<boost::shared_ptr<GeneticIndividual> > &babies);

        NEAT_DLL_EXPORT void dump(TiXmlElement *speciesElement);
    };

}

#endif
