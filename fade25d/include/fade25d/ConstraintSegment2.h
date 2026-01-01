// Copyright (C) Geom Software e.U, Bernhard Kornberger, Graz/Austria
//
// This file is part of the Fade2D library. The student license is free
// of charge and covers personal, non-commercial research. Licensees
// holding a commercial license may use this file in accordance with
// the Commercial License Agreement.
//
// This software is provided AS IS with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE.
//
// Please contact the author if any conditions of this licensing are
// unclear to you.
//
// Support: https://www.geom.at/contact/
// Project: https://www.geom.at/fade2d/html/

/// \file ConstraintSegment2.h
#pragma once
#include <set>

#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

class Point2; // FWD
class ConstraintGraph2; // FWD
class ConstraintMgr; // FWD

/**
 * \brief Enum defining the available strategies for inserting constraints.
 *
 * This enum specifies the strategies available for inserting constraint edges
 * into a triangulation. The only supported strategy is `CIS_CONSTRAINED_DELAUNAY`.
 * The others are legacy strategies that are retained for backward compatibility,
 * but should no longer be used in favor of newer techniques.
 */
 enum ConstraintInsertionStrategy
{
	CIS_CONFORMING_DELAUNAY=0,///< @deprecated Deprecated in favor of ConstraintGraph2::makeDelaunay()
	CIS_CONSTRAINED_DELAUNAY=1, ///< The only recommended insertion strategy
#if GEOM_PSEUDO3D==GEOM_TRUE
	CIS_CONFORMING_DELAUNAY_SEGMENT_LEVEL=2, ///< @deprecated Instead use `bUseHeightOfLatest=true` when calling Fade_2D::createConstraint()
#endif
	CIS_KEEP_DELAUNAY=0, ///< @deprecated Deprecated in favor of ConstraintGraph2::makeDelaunay()
	CIS_IGNORE_DELAUNAY=1 ///< @deprecated Name replaced by CIS_CONSTRAINED_DELAUNAY
};


/**
 * \brief A ConstraintSegment2 represents a constraint edge between two vertices
 * of a Delaunay triangulation.
 *
 * @image html constraint-edge.png "A Constraint Edge in a Constrained Delaunay triangulation" width=30%
 * @image latex constraint-edge.png "A Constraint Edge in a Constrained Delaunay triangulation" width=0.35\textwidth

 * - A `ConstraintSegment2` represents a constraint edge between two vertices (`Point2`
 * objects) of a Fade_2D instance.
 * - It can belong to multiple ConstraintGraph2 objects, and thus it does not
 * carry any orientation information. Instead, the ConstraintGraph2 objects
 * it belongs to can provide its orientation.
 * - A ConstraintSegment2 can be split. In this case it is dead and has two
 * children (which can be the case recursively).
*/
class ConstraintSegment2
{
private:
	// ConstraintSegment2 objects are only made by ConstraintMgr/ConstraintGraph2
	ConstraintSegment2(Point2* p0_,Point2* p1_,ConstraintInsertionStrategy cis_);
	friend class ConstraintMgr;
	friend class ConstraintGraph2;
public:
	~ConstraintSegment2();

	const std::vector<ConstraintSegment2*>& getParents() const;

	/**
	 * \brief Get the first endpoint of the constraint segment.
	 *
	 * \return A pointer to the first endpoint (`Point2` object).
	 */
	CLASS_DECLSPEC
	Point2* getSrc() const;

	/**
	 * \brief Get the second endpoint of the constraint segment.
	 *
	 * \return A pointer to the second endpoint (`Point2` object).
	 */
	CLASS_DECLSPEC
	Point2* getTrg() const;

	/// \private
	/** \brief Marks the segment as dead (internal use). */
	void kill() // Only for internal use!
	{
		bAlive=false;
	}

	/// \private
	/** \brief Checks if the segment has a direction (internal use). */
	bool dbg_hasDirection();


	/// \private
	/** \brief Reanimates the segment (internal use). */
	void unkill() // Only for internal use!
	{
		bAlive=true;
	}

	/**
	 * \brief Checks if the segment is alive.
	 *
	 * This function checks whether the current `ConstraintSegment2` object is "alive" or "dead."
	 *
	 * \return Returns `true` if the segment is alive, `false` otherwise.
	 */
	CLASS_DECLSPEC
	bool isAlive() const;


	/**
	 * \brief Retrieves the constraint insertion strategy (CIS) of this segment.
	 *
	 * This function returns the constraint insertion strategy of the present
	 * ConstraintSegment2. CIS_CONSTRAINED_DELAUNAY is the only supported
	 * strategy. Other possible values are deprecated in favor of newer techniques.
	 *
	 * \return The `ConstraintInsertionStrategy` of this segment.
	 */
	ConstraintInsertionStrategy getCIS() const;



	/**
	 * \brief Comparison operator for sorting segments.
	 *
	 * This operator compares two `ConstraintSegment2` objects for
	 * sorting segments. The comparison is based on the pointers to
	 * their endpoints (`p0` and `p1`), not the length of the ConstraintSegment2.
	 *
	 * \param pOther Another `ConstraintSegment2` to compare.
	 * \return `true` if the current segment is less than the other segment, `false` otherwise.
	 */
	inline bool operator<(const ConstraintSegment2& pOther) const
	{
		if(p0<pOther.p0) return true;
		if(p0>pOther.p0) return false;
		return p1<pOther.p1;
	}

	/**
	* \brief Splits a constraint segment at a specified point.
	*
	* This method splits the `ConstraintSegment2` at the given \p splitPoint.
	* After the operation, the current ConstraintSegment2 is marked as "dead"
	* and has two child segments.
	*
	* \param[in] splitPoint The point at which to split the segment.
	*
	* \return A pointer to the inserted `Point2` (the \p splitPoint), or `NULL`
	* if the operation fails.
	*
	* \note Always use this method to split a `ConstraintSegment2`. Attempting to split
	*       the segment by simply inserting the \p splitPoint (e.g., using `Fade_2D::insert(splitPoint)`)
	*       will fail if the \p splitPoint and the two endpoints of the `ConstraintSegment2` are not
	*       exactly collinear e.g., due to floating-point precision limitations.
	*       In contrast, this method accepts a non-collinear \p splitPoint as long as it has
	*       an orthogonal projection onto the segment.
	*/
	CLASS_DECLSPEC
	Point2* insertAndSplit(const Point2& splitPoint);

	/** \private Split a constraint segment combinatorially. (Internal use only)*/
	CLASS_DECLSPEC
	bool split_combinatorialOnly(Point2* pSplit);

	/// @private (Internal use)
	CLASS_DECLSPEC
	bool splitAndRemovePrev(const Point2& split);


	//Sets a specific ConstraintGraph2 as owner of the current ConstraintSegment2.
	/// @private (Internal use)
	void addOwnerRecursive(ConstraintGraph2* pOwner);

	// Removes a specific ConstraintGraph2 as owner of the current ConstraintSegment2.
	/// @private (Internal use)
	void removeOwner(ConstraintGraph2* pOwner);

	/**
	* \brief Retrieves all children of this constraint segment recursively.
	*
	* This method recursively collects all child `ConstraintSegment2`s of the
	* current one that are still alive, meaning they have not been marked as
	* "dead" due to being split.
	*
	* \param vChildConstraintSegments A vector that will be populated with the child segments.
	*/
	CLASS_DECLSPEC
	void getChildrenRec(std::vector<ConstraintSegment2*>& vChildConstraintSegments);

	/**
	* \brief Retrieves the direct children of this constraint segment
	*
	* This method retrieves the direct children of the current `ConstraintSegment2`s,
	* regardless whether they are dead or alive.
	*
	* \param vChildConstraintSegments A vector that will be populated with the child segments.
	*/
	CLASS_DECLSPEC
	void getDirectChildren(std::vector<ConstraintSegment2*>& vChildConstraintSegments) const;

	/**
	 * \brief Retrieves the two direct children and the split point.
	 *
	 * This method retrieves the two child segments that result from
	 * splitting this ConstraintSegment2 as well as the point where
	 * the split occurred.
	 *
	 * \param pCSeg0 A pointer to the first child ConstraintSegment2.
	 * \param pCSeg1 A pointer to the second child ConstraintSegment2.
	 * \param pSplitPoint A pointer to the vertex at which the split occurred.
	 */
	CLASS_DECLSPEC
	void getChildrenAndSplitPoint(ConstraintSegment2*& pCSeg0,ConstraintSegment2*& pCSeg1,Point2*& pSplitPoint);


	CLASS_DECLSPEC
	friend std::ostream &operator<<(std::ostream &stream, const ConstraintSegment2& cSeg);


	/// @private (Internal use)
	CLASS_DECLSPEC
	size_t getNumberOfOwners() const;

	// Not thought for public use. If the current constraint segment is a border segment, then
	// the area of the thought, non-existing outside triangle can manually be deposited here.
	// This value is used by the extended meshing method when a grow factor is given or ignored
	// if not set.
	/// @private (internal use)
	CLASS_DECLSPEC
	void setAdjacentArea(double adjacentArea_);

	/// @private (internal use)
	CLASS_DECLSPEC
	int getLabel() const;

	/// @private (internal use)
	CLASS_DECLSPEC
	std::string getLabelString() const;

	// Not thought for public use: Returns a previously via setAdjacentArea(double adjacentArea_) deposited value. If not explicitly set, DBL_MAX is returned.
	/// @private (internal use)
	CLASS_DECLSPEC
	double getAdjacentArea() const;

	static int runningLabel;
	int label;
protected:
	/// @private
	std::set<ConstraintGraph2*> sOwners;
	Point2 *p0,*p1;
	ConstraintInsertionStrategy cis;
	bool bAlive;
	std::vector<ConstraintSegment2*> vChildren;
	std::vector<ConstraintSegment2*> vParents;
	/// @private
	double adjacentArea;
};

} // NAMESPACE
