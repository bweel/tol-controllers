#ifndef OM_GENOMEMANAGER_H_
#define OM_GENOMEMANAGER_H_

#include "Defines.h"
#include "CppnGenome.h"


/**
 * Pure virtual class used to create multiple Genome Managers that are compatible with
 * the Organism Manager design. A genome Manager is responsible for creating, storing and
 * translating genomes.
 */
class GenomeManager
{
    public:
        
	/**
	 * Function that should create a new genome based on a vector of genome ids.
	 * The function should store the new genome and return an id that can later be used to
	 * retrieve the genome.
	 * The intention is that an empty vector creates a random genome,
	 * a vector with one id creates a mutation of the genome with this id
	 * and a vector with more than one id create a crossover of genomes
	 * associated with these ids. The Genome Manager might not support every
	 * amount of ids possible, but it should support all amounts that might be passed to it.
	 *
	 * @param genomeIDs An array of genome ids.
	 * @return The id of the newly created genome.
	 */
	virtual CppnGenome createGenome(const std::vector<CppnGenome>& parentsGenomes) = 0;
    
	virtual std::string genomeToString(CppnGenome genome) const = 0;
    
    virtual CppnGenome getGenomeFromStream(std::istream& stream) = 0;
};
#endif /* OM_GENOMEMANAGER_H_ */
