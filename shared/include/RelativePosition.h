#ifndef RELATIVEPOSITION_H_
#define RELATIVEPOSITION_H_

#include <stdlib.h>

/**
 * The RelativePosition class is used to indicate the position of a Roombot module in the Roombot build-plan.
 * As such, a relative position object describes which two cells in a grid are occupied by the Roombot module.
 * The first cell occupied is indicated by an x and z coordinate.
 * The second occupied cell is indicated by a boolean indicating whether
 * this Roombot module has a horizontal or vertical orientation on this grid.
 * If the module has a horizontal orientation it will occupy a cell to the right of the first cell.
 * If the module has a vertical orientation it will occupy a cell below the first cell.
 *
 * To convert a relative position to a real position in the Webots arena
 * the x and z values are multiplied by the width of the Roombot module,
 * which gives the organism the right scale,
 * and then a certain `center' position is added to the x and z values,
 * which gives the organism a places in the arena.
 * Note that `center' is a very poor name since the `center' position
 * is always at the bottom left of the organism.
 */
class RelativePosition
{

    public:
	
    int x; 			//The horizontal position a module should occupy in the grid
	int z; 			//The vertical position a module should occupy in the grid
	bool isHorizontal;	//True if this module should also occupy a cell to the right, false if this module should also occupy a cell below.
    
	/**
	 * Constructs the relative position for a module.
	 *
	 * @param x The horizontal position the module should occupy in the grid.
	 * @param z The vertical position the module should occupy in the grid.
	 * @param isHorizontal Indicates whether this module is placed horizontally, also occupying a cell to the right
	 * or vertically, also occupying a cell below.
	 */
	RelativePosition(int x, int z, bool isHorizontal);
    
	/**
	 * Calculates the number of cells of this relative position that
	 * are adjacent to the cells of the other relative position.
	 * Cells that are vertically adjacent are not counted.
	 * Overlapping cells are not counted as being adjacent.
	 * Because of the size of the Roombot, two relative positions can never
	 * have more than two adjacent cells.
	 *
	 * @param other The other relative position that we could be adjacent to.
	 * @return Returns the number of cells of this relative position that
	 * are adjacent to a cell of the other relative position.
	 */
	int nrOfAdjacentPositions(const RelativePosition& other) const;
    
	/**
	 * Calculates whether this relative position has at
	 * least one cell adjacent to the other relative position.
	 * Cells that are vertically adjacent are not counted.
	 * Overlapping cells are not counted as being adjacent.
	 *
	 * @param other The other relative position that we could be adjacent to.
	 * @return Returns true if this relative position has at least one cell adjacent
	 * to a cell of the other relative position. Returns false otherwise.
	 */
	bool adjacent(const RelativePosition& other) const;
    
	/**
	 * Calculates how many cells of this position are adjacent to the indicated cell.
	 * Cells that are vertically adjacent are not counted.
	 * Overlapping cells are not counted as being adjacent.
	 * Because of the shape of the relative position this relative
	 * position has at most one cell adjacent to the indicated cell.
	 *
	 * @param x The x coordinate of the indicated cell.
	 * @param y The y coordinate of the indicated cell.
	 * @return Returns one of this relative position is adjacent to the indicated cell.
	 * Returns zero otherwise.
	 */
	int nrOfAdjacentPositions(int x, int y) const;
    
	/**
	 * Calculates if the two indicated cells are adjacent to each other
	 * Cells that are vertically adjacent are not counted.
	 * Overlapping cells are not counted as being adjacent.
	 *
	 * @param firstX The x coordinate of the first cell.
	 * @param firstY The y coordinate of the first cell.
	 * @param secondX The x coordinate of the second cell.
	 * @param secondY The y coordinate of the second cell.
	 * @return Returns true if the first cell is adjacent to the second cell.
	 * Returns false otherwise.
	 */
	static bool adjacent(int firstX, int firstY, int secondX, int secondY);
    
	/**
	 * Returns the x coordinate of the first cell occupied by this relative position.
	 *
	 * @return Returns the x coordinate of the first cell occupied by this relative position.
	 */
	int getX() const;
    
	/**
	 * Returns the z coordinate of the first cell occupied by this relative position.
	 *
	 * @return Returns the z coordinate of the first cell occupied by this relative position.
	 */
	int getZ() const;
    
	/**
	 * Returns the x coordinate of the second cell occupied by this relative position.
	 *
	 * @return Returns the x coordinate of the second cell occupied by this relative position.
	 */
	int getX2() const;
    
	/**
	 * Returns the z coordinate of the second cell occupied by this relative position.
	 *
	 * @return Returns the z coordinate of the second cell occupied by this relative position.
	 */
	int getZ2() const;
    
	/**
	 * Compares this relative position with the other relative position and indicates
	 * whether this position is smaller than the other relative position.
	 * This relative position is smaller than the other relative position if, and only if,
	 * its x value is smaller than the x value of the other relative position,
	 * or the x value is the same as the x value of the other relative position
	 * but the z value of this position is smaller than the z value of the other relative position,
	 * or both the x and z values of this relative position are the same as the x and z values of the other relative position
	 * but this position is vertical while the other position is horizontal
	 * (vertical is considered smaller than horizontal).
	 *
	 * @param other The other relative position to compare this relative position with.
	 * @return Returns true if this relative position is smaller than the other relative position.
	 * Returns false otherwise.
	 */
	bool operator<(const RelativePosition &other) const;
    
	/**
	 * Compares this position with the other relative position and
	 * indicates whether his position is equal to the other position.
	 * Two positions are equal if, and only if,
	 * their x value, z value and horizontal orientation are the same.
	 *
	 * @param other The other relative position to compare this relative position with.
	 * @return Returns true if this relative position is equal to the other relative position.
	 * Returns false otherwise.
	 */
	bool operator==(const RelativePosition &other) const;
    
	/**
	 * Compares this position with the other relative position and
	 * indicates whether his position is equal to the other position.
	 * Two positions are equal if, and only if,
	 * their x value, z value and horizontal orientation are the same.
	 *
	 * @param other The other relative position to compare this relative position with.
	 * @return Returns true if this relative position is equal to the other relative position.
	 * Returns false otherwise.
	 */
	bool equals(const RelativePosition& other) const;
};

#endif /* RELATIVEPOSITION_H_ */
