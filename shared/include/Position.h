#ifndef OM_POSITION_H_
#define OM_POSITION_H_

#include "Defines.h"

#include <iostream>

#define X_AXIS 0			//The index of the x-axis in a translation array
#define Y_AXIS 1			//The index of the y-axis in a translation array
#define Z_AXIS 2			//The index of the z-axis in a translation array
#define TRANSLATION_LENGTH 3 //The length of a translation (position) array

#define ROTATION_X_INDEX 0
#define ROTATION_Y_INDEX 1
#define ROTATION_Z_INDEX 2
#define ROTATION_ANGLE_INDEX 3
#define ROTATION_LENGTH 4 //The length of a rotation array

#define HORIZONTAL_ROTATION {0.57735027, 0.57735027, -0.57735027, 2.09}
#define VERTICAL_ROTATION {1,0,0,1.57079633}

typedef double WB_Rotation[ROTATION_LENGTH];
typedef double Translation[TRANSLATION_LENGTH];

/**
 * The Position class is used to transfer both rotation and translation information between functions.
 * The Position class also contains a number of useful position related functions such as calculating
 * the distance between to positions. Position objects are comparable.
 */
class Position {
	WB_Rotation rotation;		//The rotation associated with this position.
	Translation translation;	//The translastion assocaietd with this position.
    
public:
	/**
	 * Constructs a position object based on separate x, y and z values,
	 * and a boolean orientation, being either horizontal or vertical.
	 *
	 * @param x The x coordinate of the position.
	 * @param y The y coordinate of the position.
	 * @param z The z coordinate of the position.
	 * @param isHorizontal A boolean where true means the position comes with a horizontal orientation,
	 * while false means the position comes with a vertical orientation.
	 * These orientations are assumed to be from a top perspective,
	 * so vertical means along the z-axis, not the y-axis.
	 */
	Position(double x = 0, double y = 0, double z = 0, bool isHorizontal = true);
    
	/**
	 * Constructs a position object based on a translation and rotation array.
	 * The position and translation arrays are how Webots handles positional information
	 * and how the Position object stores the translation and rotation internally.
	 *
	 * @param _translation The translation part of the position.
	 * The translation array always consists of three doubles, containing the x, y and z values.
	 * @param _rotation The rotational part of the position.
	 * The rotation array always contains four doubles,
	 * the x, y and z rotations and the angle.
	 */
	Position(const Translation _translation, const WB_Rotation _rotation);
    
	/**
	 * Returns the value of the translation array at the indicated index.
	 * The function does not contain any boundary checking,
	 * meaning that any index higher than two or lower than zero might result in errors.
	 *
	 * @param index The index for which the value should be returned.
	 * Zero returns the x value, one the y value and two the z value.
	 * Other values are unspecified.
	 * @return Returns the value at index of the translation array.
	 */
	double get(const int &index) const;
    
	/**
	 * Sets the x value for this position.
	 *
	 * @param x The new x value for this position.
	 */
	void setX(double x);
    
	/**
	 * Returns the x value of this position.
	 *
	 * @return Returns the x value of this position.
	 */
	double getX() const;
    
	/**
	 * Sets the y value for this position.
	 *
	 * @param y The new y value for this position.
	 */
	void setY(double y);
    
	/**
	 * Returns the y value of this position.
	 *
	 * @return Returns the y value of this position.
	 */
	double getY() const;
    
	/**
	 * Sets the z value for this position.
	 *
	 * @param z The new z value for this position.
	 */
	void setZ(double z);
    
	/**
	 * Returns the z value of this position.
	 *
	 * @return Returns the z value of this position.
	 */
	double getZ() const;
    
	/**
	 * Returns a pointer to the translation array.
	 *
	 * @return Returns a pointer to the translation array.
	 */
	double * getTransaltion();
    
	/**
	 * Returns a pointer to the rotation array.
	 *
	 * @return Returns a pointer to the rotation array.
	 */
	double * getRotation();
    
	/**
	 * Checks whether this position is still within arena boundaries.
	 * This function needs the arena size and assumes that the arena is centred
	 * around zero and that the arena is square. Only checks boundaries in two dimensions,
	 * the x-axis and the z-axis.
	 *
	 * @param arenaSize The size of the arena as the length of one of its sides in Webots meters.
	 * @return Returns true if this position is within the boundaries of the arena.
	 * Returns false otherwise.
	 */
	bool checkBounds(double arenaSize) const;
    
	/**
	 * Calculates the euclidean distance between two positions.
	 * Considers all three dimensions for calculating distance,
	 * as opposed to some other functions like checkBounds() who only consider two dimensions.
	 *
 	 * @param other The other position to calculate the distance to.
	 * @return Returns the euclidean distance between this position and the other position.
	 */
 	double distance(const Position &other) const;
    
	/**
	 * Returns this position as a string.
	 * Used mainly for logging and debugging.
	 *
	 * @return Returns this position as a string.
	 */
 	std::string toString() const;
	
	/**
	 * Compares this position with an other position, indicating whether this position is smaller than the other position.
	 * When comparing first the x values are compared,
	 * then the y values and then z values.
	 * When the value being compared of this position is smaller than the value of the other position, this position will be considered smaller.
	 * When the value being compared of this position is larger than the value of the other position, this position will be considered greater.
	 * Only when all compared values are the same will the objects be considered equal, meaning this object is not smaller than the other object.
	 *
	 * @param other The other position to compare this position with.
	 * @return Returns true if this position is smaller than the other position.
	 * Returns false otherwise.
	 */
	bool operator<(const Position &other) const;
    
	/**
	 * Prints the position to standard out.
	 * Only used for debugging.
	 */
	void print() const;
};

#endif /* OM_POSITION_H_ */