#ifndef OM_ROOMBOT_BUILDPLAN_H_
#define OM_ROOMBOT_BUILDPLAN_H_

#include "RelativePosition.h"
#include "Position.h"
#include "Bitmap.h"
#include "BuildPlan.h"

#include "JGTL_LocatedException.h"

#include <iostream>
#include <vector>
#include <exception>

#define POINT_SIZE 3

/**
 * A build-plan used to build organisms.
 * A build-plan consists of a vector of relative positions,
 * were every relative position represents a module to be placed.
 */
class RoombotBuildPlan : public BuildPlan
{
	std::vector<RelativePosition> positions;
	size_t gridSize;
    
	/**
	 * Returns the 'smallest' position of this buildplan.
	 * The minimal position is the position with the lowest x value,
	 * then the lowest y value and then the vertizal positions are
	 * smaller then horizontal positions.
	 *
	 * @return Returns the 'smallest' relative position of this builplan.
	 */
	RelativePosition getMinimalPosition() const;
    
	/**
	 * Checks wether the position indicated by x and z is
	 * occupied by (part of) a module in this buildplan.
	 *
	 * @param x The coordinate of provided position
	 * @param z The coordinate of provided position
	 * @return Return true if the position is occupied. Returns false otherwise.
	 */
	bool occupied(int x, int z) const;
    
	/**
	 * Checks if this buildplan contains the same relative position given as the argument.
	 *
	 * @param position The position to be searched for in this buildplan.
	 * @return Returns the index of position if the position is found. Returns positions.size() otherwise.
	 */
	size_t find(const RelativePosition& position, size_t start = 0) const;
    
	/**
	 * Checks if this buildplan is the same as the other buildplan.
	 * Order of modules is always ignored.
	 * By default shifts in positions are ignored.
	 *
	 * @param other The buildplan that this buildplan should be compared with.
	 * @param ignoreShift If true shifts in positions are ignored.
	 *   If false, a position shift will make two buildplans unequal.
	 * @return Returns true if this buildplan is equal to the other buildplan.
	 *   Returns false otherwise.
	 */
	bool equals(RoombotBuildPlan other, bool ignoreShift = true) const;
    
	/**
	 * Shifts all modules in this buildplan x steps on the x-axis
	 * and z steps on the z-axis.
	 *
	 * @param x The shift on the x axis.
	 * @param z The shift on the z axis.
	 */
	void shift(int x, int z);
	
	/**
	 * Flips this buildplan horizontally.
	 */
	void flipHorizontally();
	
	/**
	 * Flips this buildplan vertically.
	 */
	void flipVertically();
	
	/**
	 * Turns this buildplan clockwise and mirrors it.
	 */
	void turnAndMirror();
	
	/**
	 * Turns this buildplan clockwise.
	 */
	void turnNinetyDegreesClockwise();
	
	/**
	 * Turns this buildplan counterclockwise.
	 */
	void turnNinetyDegreesCounterClockwise();
    
public:
	/**
	 * De default constructor, no parameters needed.
	 */
	RoombotBuildPlan(size_t gridSize);
    
	/**
	 * Returns the number of modules in this build plan.
	 *
	 * @return Returns the size of this build-plan in number of modules.
	 */
	size_t size() const;
    
	/**
	 * Return the relative position at index.
	 *
	 * @param index The index for the relative position.
	 * @return Returns the relative position at index.
	 */
	RelativePosition getRelativePosition(size_t index) const;
    
 	/**
 	 * Takes the center of an organism and an index and returns the absolute position,
	 * including angular position, of the module at this index.
	 *
	 * @param center The center, used in combination with a relative position
	 *   to calculate a true position.
	 * @param index The index for the true position.
	 * @return A true position based on the provided center and the relative position at index.
 	 */
	Position getPosition(const Position& center, size_t index) const;
    
    
    //Side view:
    //             NORTH
    //y=0.055   +---------+
    //          |White /  |
    //          |     /   |
    //NORTH_WEST|    /    |NORTH_EAST
    //          |   /     |
    //          |  / black|
    //y=-0.055  +---------+
    //y=0.055   +---------+
    //          |White /  |
    //          |     /   |
    //SOUTH_WEST|    /    |SOUTH_EAST
    //          |   /     |
    //          |  / Black|
    //y=-0.055  +---------+
    //             SOUTH
    
    
	/**
	 * Checks whether the connector at connectorIndex of the module at index
	 * should be locked.
	 *
	 * @param index The index of the module to be checked for.
	 * @param connectorIndex The index of the connector to be checked for.
	 * @return Returns true if the connector should be locked, returns false otherwise.
	 */
	bool getConnector(size_t index, size_t connectorIndex) const;
    
	/**
	 * Adds a module, using a relative position, to this build plan.
	 *
	 * @param x The x coordinate of this position.
	 * @param z The z coordinate of this position.
	 * @param isHorizontal Indicates whether the module at this position should be horizontal
	 *   or vertical.
	 */
	void add(int x, int z, bool isHorizontal);
    
	/**
	 * Returns the number of neighbours of the module indicated by a relative position.
	 *
	 * @param position The position of the module.
	 * @return Returns the number of neighbours the provided module has in this build-plan.
	 */
	int numberOfNeighbours(const RelativePosition& position) const;
    
	/**
	 * Returns the number of neighbours of the module indicated by x, z and isHorizontal.
	 *
	 * @param x The x coordinate of the module.
	 * @param z The z coordinate of the module.
	 * @param isHorizontal Indicates whether the module at this position should be horizontal
	 *   or vertical.
	 * @return Returns the number of neighbours the provided module has in this build-plan.
	 */
	int numberOfNeighbours(int x, int z, bool isHorizontal) const;
	
	/**
	 * Returns the number of neighbours of the position indicated by x and z.
	 *
	 * @param x The x coordinate of the position.
	 * @param z The z coordinate of the position.
	 * @return Returns the number of neighbours the provided position has in this build-plan.
	 */
	int numberOfNeighbours(int x, int z) const;
    
	/**
	 * Checks whether this build-plan results in the same organism as other.
	 * By default mirrored or rotated versions of the same organism are considered to be the same.
	 *
	 * @param other The build-plan to compare with.
	 * @param ignoreRotation If true any rotated versions of the same organism will considered to be equal.
	 * @param ignoreMirror If true any mirrored versions of the same organism will be considered to be equal.
	 * @return Returns true if this build-plan represents an organism that is equal to the organism represented by other. Returns false otherwise.
	 */
	bool equalOrganisms(BuildPlan* const otherBuildPlan, bool ignoreRotation = true, bool ignoreMirror = true) const;
    
	/**
	 * Prints the relative positions
	 */
	void print() const;
    
	/**
	 * Prints the buildplan to a bitmap.
	 *
	 * @return Returns the bitmap depicting this buildplan.
	 */
	Bitmap* paint() const;
};

#endif /* OM_ROOMBOT_BUILDPLAN_H_ */
