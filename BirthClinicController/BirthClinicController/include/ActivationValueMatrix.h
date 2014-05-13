#ifndef OM_ACTIVATION_VALUE_MATRIX_H_
#define OM_ACTIVATION_VALUE_MATRIX_H_

#include "Defines.h"

#include "NEAT.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <set>

using namespace boost::numeric::ublas;

#define NEIGHBOURS 4

/**
 * Coordinate class used by Activation Value Matrix to return, for example, maximal coordinates,
 * with their x and y values, the value of the matrix at the indicated coordinate,
 * and whether the function actually managed to find a suitable coordinate.
 * Because it is used to find maximal values, Coordinate is a comparable class,
 * based solely on the x and y values of the object.
 */
class Coordinate
{

    public:
	
    bool found;     //Boolean indicating if the search for this Coordinate succeeded
	int x;          //The X value of this coordinate
	int y;          //The Y value of this coordinate
	double value;	//The value at coordinate (X,Y)
	
	/**
	 * Constructs a new Coordinate.
	 * Found is set to false for new coordinates.
	 *
	 * @param x The X value of this coordinate
	 * @param y The Y value of this coordinate
	 * @param value The value at coordinate (X,Y)
	 */
	Coordinate(int x = 0, int y = 0, double value = 0);
	
	/**
	 * Compares this Coordinate with an other Coordinate.
	 *
	 * @param other The other Coordinate to compare with
	 * @return Returns true if this Coordinate has a smaller X value than other,
	 * or if this Coordinate has an X value that is equal to other
	 * and a Y value that is smaller than the Y value of other.
	 * Returns false otherwise.
	 */
	bool operator<(const Coordinate &other) const;
};

/**
 * Virtual matrix containing the activation values produced by a CPPN.
 * The matrix is centred around zero, and the difference between adjacent cells is one.
 * When querying the matrix the input X and Y values are modified such that the center of the matrix corresponds to (0,0)
 * The modified X' and Y' values are then set as input for the X and Y inputs of the CPPN network.
 * The output of the network will then be returned as the activation value for the original X and Y query.
 *
 * For example, a matrix with a width and height of two will contain the activation values of
 * (-0.5, -0.5), (0.5, -0.5), (-0.5, 0.5) and (0.5, 0.5) in the cells
 * (   0,    0), (  1,    0), (  0,    1) and (  1,   1) respectively.
 *
 * The Activation Value Matrix is also used to keep track of which cells have been considered for the organism
 * and which cells should still be considered.
 * As a result the Activation Value Matrix contains a boolean Matrix keeping track of which cells have already been considered, or `used',
 * and it keeps track of all coorindate that are adjacent to the current organism,
 * added to the Activation Value Matrix via the addModuleCoordinate function,
 * which are the only cells that need to be considered while extending the organism.
 */
class ActivationValueMatrix
{
	int width;                              //The width of the matrix
	int height;                             //The height of the matrix
	double xModifier;                       //Modifier to substract from the X value to get the input value of the X input node
	double yModifier;                       //Modifier to substract from the Y value to get the input value of the Y input node
	mutable NEAT::FastNetwork<double> cppn; //The CPPN used to generate the matrix values.
    //Made mutable because the `changes' made do not actually change the state of the activation value matrix.
	matrix<bool>* used;                     //Boolean matrix indicating which cells have been set to `used'
	set<Coordinate> adjacentCoordinates;	//The complete set of coordinates adjacent to the current organism.
    
    public:
    
	/**
	 * Constructs an activation value matrix for the supplied genome, width and height.
	 *
	 * @param genome The genome used to construct the CPPN that supplies the values for the matrix.
	 * @param _width The width of the Activation Value Matrix.
	 * @param _height The height of the Activation Value Matrix.
	 */
	ActivationValueMatrix(boost::shared_ptr<const NEAT::GeneticIndividual> genome, int width, int height);
    
	/**
	 * Destructs the Activation Value Matrix.
	 * Deletes the used matrix that is created on construction.
	 */
	virtual ~ActivationValueMatrix();
    
	/**
	 * Returns the Coordinate of the cell with the highest activation value in the Activation Value Matrix,
	 * from all cells that are not used and that have an activation value higher than THRESHOLD.
	 * If there exist no unused cells with an activation value higher than THRESHOLD the function will return
	 * an `unfound' Coordinate instead.
	 *
	 * @return If there exists at least one unused cell in the Activation Value Matrix with an activation value higher than THRESHOLD,
	 * it will return the Coordinate of the cell with the highest activation value among them.
	 * Else it will return a Coordinate where found is false and x, y and value have no meaning.
	 */
	Coordinate getMax() const;
    
	/**
	 * Returns the Coordinate of the cell with the highest activation value in the Activation Value Matrix,
	 * from all neighbours of the input coordinate that are not used and that have an activation value higher than THRESHOLD.
	 * If there exist no unused neighbours with an activation value higher than THRESHOLD the function will return
	 * an `unfound' Coordinate instead.
	 *
	 * @param coordinate The coordinate of the cell for which we want to find the neighbour with the highest activation value.
	 * @return If there exists at least one unused neighbour with an activation value higher than THRESHOLD,
	 * it will return the Coordinate of the cell with the highest activation value among them.
	 * Else it will return a Coordinate where found is false and x, y and value have no meaning.
	 */
	Coordinate getMaxNeighbour(const Coordinate& coordinate) const;
    
	/**
	 * Returns the Coordinate of the cell with the highest activation value in the Activation Value Matrix,
	 * from all cells adjacent to one of the placed modules that are not used and that have an activation value higher than THRESHOLD.
	 * If there exist no unused cells with an activation value higher than THRESHOLD the function will return
	 * an `unfound' Coordinate instead.
	 *
	 * @return If there exists at least one unused cell adjacent to one of the placed modules
	 * in the Activation Value Matrix with an activation value higher than THRESHOLD,
	 * it will return the Coordinate of the cell with the highest activation value among them.
	 * Else it will return a Coordinate where found is false and x, y and value have no meaning.
	 */
	Coordinate getMaxAdjacent() const;
    
	/**
	 * Sets the cell indicated by coordinate to the new used value.
	 * Used cells are omitted when searching for cells with the highest activation value.
	 *
	 * @param coordinate The coordinate of the cell which will get the new used value.
	 * @param newUsed The new value for used, where true means the cell is used and will be omitted,
	 * while false means the cell is free and should be considered for all purposes.
	 */
	void setUsed(const Coordinate& coordinate, bool newUsed);
    
	/**
	 * `Adds' a module to coordinate.
	 * Modules added this way will be considered for the purpose of determining
	 * which cells are adjacent to currently placed modules
	 * in the getMaxAdjacent() function.
	 *
	 * These modules are always considered to occupy one square of the Matrix.
	 * This means that, if your modules take more than one square of the matrix,
	 * you will have to add each square occupied by your module separately.
	 *
	 * @param coordinate The coordinate that will be considered occupied by the module.
	 */
	void addModuleCoordinate(const Coordinate& coordinate);
    
    
    
    private:
	
    /**
	 * Indicates whether a cell has been `used'.
	 *
	 * @param x The x coordinate of the queried cell.
	 * @param y The y coordinate of the queried cell.
	 * @return Returns true when the cell at (x,y) is set to used.
	 * Returns false otherwise.
	 */
	bool getUsed(int x, int y) const;
    
	/**
	 * Indicates whether a cell has been `used'.
	 *
	 * @param coordinate The coordinate of the cell being queried.
	 * @return Returns true of the cell indicated by coordinate has been set to used.
	 * Returns false otherwise.
	 */
	bool getUsed(const Coordinate& coordinate) const;
    
	/**
	 * Returns the activation value of the queried cell.
	 *
	 * @param x The x coordinate of the queried cell.
	 * @param y The y coordinate of the queried cell.
	 * @return Returns the activation value of the cell at (x,y).
	 */
	double get(int x, int y) const;
    
	/**
	 * Get activation values for an organisms that only contains a single module.
	 * This function is used for parameter adjustment experiments
	 */
	double getOne(int x, int y) const;
    
	/**
	 * Returns the activation value of the queried cell.
	 *
	 * @param coordinate The coordinate of the cell being queried.
	 * @return Returns the activation value of the cell indicated by coordinate.
	 */
	double get(const Coordinate& coordinate) const;
    
	/**
	 * Checks whether the x and y input values would result in a valid cell of the matrix.
	 * Valid cells have x and y indexes that are greater than or equal to zero,
	 * an x index smaller than width,
	 * and a y index smaller than height.
	 *
	 * @param x The x index.
	 * @param y The y index.
	 * @return Returns true if:
	 * both x and y are greater than or equal to zero,
	 * x is smaller than width,
	 * and y is smaller than height.
	 * Returns false otherwise.
	 */
	bool inBounds(int x, int y) const;
    
};

#endif /* OM_ACTIVATION_VALUE_MATRIX_H_ */
