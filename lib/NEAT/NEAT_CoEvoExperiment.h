#ifndef NEAT_COEVOEXPERIMENT_H_INCLUDED
#define NEAT_COEVOEXPERIMENT_H_INCLUDED

#ifdef EPLEX_INTERNAL

#include "NEAT_Defines.h"

namespace NEAT
{
    class NEAT_DLL_EXPORT CoEvoExperiment
    {
    protected:

    public:
        virtual pair<double,double> playGame(
            boost::shared_ptr<GeneticIndividual> ind1,
            boost::shared_ptr<GeneticIndividual> ind2) = 0;

        virtual ~CoEvoExperiment() {}
    };
}

#endif

#endif // NEAT_COEVOEXPERIMENT_H_INCLUDED
