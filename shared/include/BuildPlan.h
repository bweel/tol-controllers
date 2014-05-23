#ifndef OM_BUILDPLAN_H_
#define OM_BUILDPLAN_H_

#include "Bitmap.h"
#include "Position.h"


/**
 * Pure virtual BuildPlan class to be used as an interface.
 * The BuildPlan class is used by the Genome Manager
 * to communicate to the Organism Manager how an organism should
 * be constructed. While this class defines how positions and connectors
 * should be retrieved, it does not define any functions to add modules
 * to the BuildPlan. This can be done in any way suitable for the translation
 * process used by the Genome Manager.
 *
 * The equalOrganisms(), print() and paint() functions are optional for using the
 * LogReader and can be implemented as dummies.
 */
class BuildPlan {
public:
	/**
	 * Returns the number of modules in this build plan.
	 *
	 * @return Returns the size of this buildplan in number of modules.
	 */
	virtual size_t size() const = 0;
    
 	/**
 	 * Takes the center of an organism and an index and returns the absolute position,
	 * including angular position, of the module at this index.
	 *
	 * @param center The center, used in combination with a relative position
	 *   to calculate a true position.
	 * @param index The index for the true position.
	 * @return A true position based on the provided center and the relative position at index.
 	 */
	virtual Position getPosition(const Position& center, size_t index) const = 0;
    
	/**
	 * Checks whether the connector at connectorIndex of the module at index
	 * should be locked.
	 *
	 * @param index The index of the module to be checked for.
	 * @param connectorIndex The index of the connector to be checked for.
	 * @return Returns true if the connector should be locked, returns false otherwise.
	 */
	virtual bool getConnector(size_t index, size_t connectorIndex) const = 0;
    
	/**
	 * Checks whether this buildplan results in the same organism as other.
	 * By default mirrored or rotated versions of the same organism are considered to be the same.
	 *
	 * @param other The buildplan to compare with.
	 * @param ignoreRotation If true any rotated versions of the same organism will considered to be equal.
	 * @param ignoreMirror If true any mirrored versions of the same organism will be considered to be equal.
	 * @return Returns true if this BuildPlan represents an organism that is equal to the organism represented by other. Returns false otherwise.
	 */
	virtual bool equalOrganisms(BuildPlan* const other, bool ignoreRotation = true, bool ignoreMirror = true) const = 0;
    
    
	/**
	 * Prints the build-plan.
	 */
	virtual void print() const = 0;
    
	/**
	 * Prints the buildplan to a bitmap.
	 *
	 * @return Returns the bitmap depicting this buildplan.
	 */
	virtual Bitmap* paint() const = 0;
};

#endif /* OM_BUILDPLAN_H_ */
