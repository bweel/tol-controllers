#ifndef OM_CPPNGENOME_H_
#define OM_CPPNGENOME_H_

#include "Defines.h"

#include "NEAT.h"


class CppnGenome {
	
    int size; //The size of the grid that should be used when translating this genome
    boost::shared_ptr<NEAT::GeneticIndividual> cppn; //Pointer to a Cppn
    
    
    public:
	
    /**
     * Constructs a random, minimal genome with a grid size of size.
     * The minimal genome will contain one output node, two input nodes
     * and a bias node. The network will be fully connected and all
     * nodes, except the bias node, will have a random activation function.
     *
     * @param size The grid size of this genome.
     */
    CppnGenome(int size);
    
    /**
     * Constructs a copy of genome1.
     * The genome will not be mutated.
     *
     * @param genome1 The genome to be copied.
     */
    CppnGenome(const CppnGenome& genome1);
    
    /**
     * Constructs the genome described by stream.
     * This constructor is able to process streams based on the output of the toString() function,
     * creating the exact same genome that returned the string.
     *
     * @param stream Stream containing the description of a CppnGenome.
     */
    CppnGenome(std::istream& stream);
    
    /**
     * Destroys the CppnGenome.
     * Does not actually perform any additional clean-up.
     */
    virtual ~CppnGenome();
    
    /**
     * Returns a string of this genome that contains enough information to create
     * an exact copy of this genome.
     * Used in combination with the stream based constructor to write the genome to file,
     * and then reconstruct for detailed analysis.
     * Useful for debugging, logging or saving genomes for later use.
     *
     * @return Returns a string containing all information to reconstruct the genome.
     */
    string toString() const;
    
    /**
     * Mutates this genome.
     * Mutation of the cppn depends on the NEAT global settings.
     * Mutation of size happens with a chance of SIZE_MUTATION_RATE
     * by adding a random number drawn from a normal distribution with a mean of zero
     * and a standard deviation of SIZE_MUTATION_STRENGTH
     */
    void mutate();
    
    /**
     * Combines this genome with the input genome and then mutates this genome.
     * Crossover and mutation of the cppn depends on the NEAT global settings.
     * Crossover of size happens by taking the average of both parents,
     * rounding up or down 50% of the time.
     * Mutation of size happens with a chance of SIZE_MUTATION_RATE
     * by adding a random number drawn from a normal distribution with a mean of zero
     * and a standard deviation of SIZE_MUTATION_STRENGTH
     *
     * @param genome The genome to use in the crossover.
     */
    void crossoverAndMutate(const CppnGenome& genome);
    
    /**
     * Returns a pointer to the cppn used by the Activation Value Matrix.
     *
     * @return Returns a shared pointer to the cppn that is used to get the values of the Activation Value Matrix
     */
    boost::shared_ptr<NEAT::GeneticIndividual> getCppn() const;
    
    /**
     * Returns the size of the Activation Value Matrix,
     * where the size is the width and height of the, always square, matrix.
     *
     * @return Returns the size of the Activation Value Matrix.
     */
    int getSize() const;
    
    
    private:
    
    /**
     * Mutates the size gene of this genome.
     * Mutation of size happens with a chance of SIZE_MUTATION_RATE
     * by adding a random number drawn from a normal distribution with a mean of zero
     * and a standard deviation of SIZE_MUTATION_STRENGTH
     */
    void mutateSize();
    
};

#endif /* OM_CPPNGENOME_H_ */
