//
//  MatrixGenome.h
//  shared
//
//  Created by Berend Weel on 16/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef shared_MatrixGenome_h
#define shared_MatrixGenome_h

#include "Defines.h"

#include <iostream>
#include <random>

#include "Random.h"
#include "MindGenome.h"


class MatrixGenome : public MindGenome {
    
private:
    std::vector<std::vector<double> > genes;
    std::size_t x_size;
    std::size_t y_size;
    utils::Random *random;
    
public:
    
	
    /**
     * Default constructor, the genome will be completely empty.
     *
     */
    MatrixGenome();
    
    /**
     * Constructs a random, minimal genome with a matrix of size_x by size_y.
     * The minimal genome will contain one output node, two input nodes
     * and a bias node. The network will be fully connected and all
     * nodes, except the bias node, will have a random activation function.
     *
     * @param size_x The x size of this genome.
     * @param size_y The y size of this genome.
     */
    MatrixGenome(int size_x, int size_y);
    
    /**
     * Constructs a copy of genome1.
     * The genome will not be mutated.
     *
     * @param genome1 The genome to be copied.
     */
    MatrixGenome(const MatrixGenome& genome1);
    
    /**
     * Constructs the genome described by stream.
     * This constructor is able to process streams based on the output of the toString() function,
     * creating the exact same genome that returned the string.
     *
     * @param stream Stream containing the description of a CppnGenome.
     */
    MatrixGenome(std::istream& stream);
    
    /**
     * Destroys the CppnGenome.
     * Does not actually perform any additional clean-up.
     */
    virtual ~MatrixGenome();
    
    /**
     * Clone implementation for the use of ptr_vector
     *
     */
    MatrixGenome *clone () const;
    
    /**
     * Sets the parameters of the genome.
     * This overwrites any values that have been set before.
     *
     * @param The values
     */
    void setValues(std::vector<std::vector<double> > values);
    
    /**
     * Initialize this genome by reading a string
     *
     */
    virtual void readString(std::string genome);
       
    /**
     * Returns a string of this genome that contains enough information to create
     * an exact copy of this genome.
     * Used in combination with the stream based constructor to write the genome to file,
     * and then reconstruct for detailed analysis.
     * Useful for debugging, logging or saving genomes for later use.
     *
     * @return Returns a string containing all information to reconstruct the genome.
     */
    virtual std::string toString() const;
    
    /**
     * Mutates this genome.
     *
     * Mutation of happens with a chance of SIZE_MUTATION_RATE
     * by adding a random numbers drawn from a normal distribution with a mean of zero
     * and a standard deviation of SIZE_MUTATION_STRENGTH
     */
    void mutate();
    
    /**
     * Combines this genome with the input genome and then mutates this genome.
     *
     * Mutation of happens with a chance of SIZE_MUTATION_RATE
     * by adding random numbers drawn from a normal distribution with a mean of zero
     * and a standard deviation of SIZE_MUTATION_STRENGTH
     *
     * @param genome The genome to use in the crossover.
     */
    void crossoverAndMutate(boost::shared_ptr<MindGenome> genome);
    
    /**
     * Returns a copy of the matrix
     *
     * @return a copy of the matrix
     */
    std::vector<std::vector<double> > getMatrix();
    
    /**
     * Returns the x size of the matrix,
     *
     * @return Returns the size of the Activation Value Matrix.
     */
    int getSizeX() const;
    
    /**
     * Returns the y size of the matrix
     *
     * @return Returns the size of the Activation Value Matrix.
     */
    int getSizeY() const;   
};

#endif
