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


#pragma once
#include <map>
#include "common.h"
#ifndef FADE2D_EXPORT
	#include "License.h"
#endif
#include "Segment2.h"
#include "MsgBase.h"
#include "Bbox2.h"


/// \file SegmentChecker.h


/**
 * \brief Enumerates the possible intersection types between two line segments.
 *
 * The possible values describe if and how two segments intersect.
 */
enum SegmentIntersectionType
{
	SIT_UNINITIALIZED, /**< Invalid value */
	SIT_NONE, /**< No intersection */
	SIT_SEGMENT,  /**< Collinear intersection (intersection is a non-degenerate segment). */
	SIT_POINT, /**< Intersection at a single point (not at the endpoints). */
	SIT_ENDPOINT /**< Intersection occurs at a common endpoint of the two segments. */
};


/**
 * \brief Describes the result of clipping a segment with a bounding box.
 *
 * These values indicate the result of clipping an input segment against a bounding box.
 */
enum ClipResult
{
	CR_INVALID, /**< Can't compute a result, call setLimit() with a valid Bbox2 before! */
	CR_EMPTY, /**< The result is empty (input completely outside the box) */
	CR_CLIPPED_DEGENERATE, /**< The result has been clipped and is degenerate */
	CR_CLIPPED_NONDEGENERATE, /**< The result has been clipped and is non-degenerate */
	CR_COMPLETE_DEGENERATE,   /**< The result is unclipped and degenerate (because the segment was already degenerate) */
	CR_COMPLETE_NONDEGENERATE /**< The result is unclipped and non-degenerate */
};

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

class SegmentCheckerData; // FWD


/**
 * \class SegmentChecker
 * \brief Identifies intersections between a collection of line segments.
 *
 * \image html identify_intersecting_segments.png "Polylines: Intersecting segments are automatically found" width=40%
 * \image latex identify_intersecting_segments.eps "Polylines: Intersecting segments are automatically found" width=0.5\textwidth
 *
 * This class is used to check a collection of line segments for intersections.
 * \if DOC25D It supports both 2D and 2.5D intersection detection. \endif The class provides efficient
 * spatial search algorithms, making it suitable for very large inputs.
 *
 * \sa [C++ Example: Segment Checker](http://www.geom.at/segment-checker/)
 */
class CLASS_DECLSPEC SegmentChecker
{
public:

	/**
	 * \brief Constructor to initialize the SegmentChecker with a collection of segments.
	 *
	 * This constructor processes the input segments and prepares an internal data structure that
	 * allows for efficient spatial searches.
	 *
	 * @param vSegments_ The collection of segments to be checked for intersections.
	 */
	explicit SegmentChecker(const std::vector<Segment2*>& vSegments_);
	/** Default constructor for SegmentChecker. */
	SegmentChecker();
	/** Destructor */
	~SegmentChecker();

	/**
	 * \brief Clips a segment to the intersection with a bounding box.
	 *
	 * This method clips a segment to its intersection with a predefined bounding box.
	 *
	 * @param [in,out] seg The segment to be clipped.
	 *
	 * @return A ClipResult indicating the status of the clipping operation. The segment can
	 * be clipped into the same segment, a subsegment, become degenerate, or be completely
	 * outside the box.
	 *
	 * @note Ensure that setLimit() has been called with a valid bounding box before using
	 * this method, otherwise the method returns CR_INVALID.
	 */
	ClipResult clipSegment(Segment2& seg);


	/**
	 * \brief Sets the bounding box for segment clipping.
	 *
	 * This method defines the bounding box that will be used by clipSegment().
	 *
	 * @param bbx The bounding box to be set.
	 */
	void setLimit(const Bbox2& bbx);


	/**
	 * \brief Retrieves the current bounding box used for clipping.
	 *
	 * Retrieves the current bounding box used for clipSegment().
	 *
	 * @return The currently set bounding box.
	 */
	Bbox2 getLimit() const;


	/**
	 * \brief Gets the segment at the specified index.
	 *
	 * This method returns the Segment2 at index \p i from the internal collection of segments.
	 *
	 * @param i The index of the segment to be returned.
	 * @return The segment at the specified index.
	 */
	Segment2* getSegment(size_t i) const;
	/**
	 * \brief Returns the total number of segments in this SegmentChecker object.
	 *
	 * @return The number of segments in the current object.
	 */
	size_t getNumberOfSegments() const;

	/**
	 * \brief Retrieves the index of a given segment.
	 *
	 * This method returns the index of the provided segment in the internal collection.
	 *
	 * @param pSeg The segment whose index is to be returned.
	 * @return The index of the segment, or -1 if not found.
	 */
	int getIndex(Segment2* pSeg) const;
	/**
	 * \brief Subscribes a message handler to receive progress updates.
	 *
	 * This method allows a custom progress bar or message handler to be registered to receive progress updates
	 * during the intersection check. This is optional and is primarily used for larger datasets.
	 *
	 * @param msgType The type of message (e.g., MSG_PROGRESS).
	 * @param pMsg A user-defined progress bar or message handler derived from MsgBase.
	 */
	void subscribe(MsgType msgType,MsgBase* pMsg);
	/**
	 * \brief Unsubscribes a message handler from receiving updates.
	 *
	 * This method removes a previously registered message handler.
	 *
	 * @param msgType The type of message to unsubscribe from.
	 * @param pMsg The message handler to unsubscribe.
	 */
	void unsubscribe(MsgType msgType,MsgBase* pMsg);

	/**
	 * \brief Visualizes all segments, both intersecting and non-intersecting
	 *
	* \image html show_line_segments.png "Line segments written to a postscript file" width=35%
	* \image latex show_line_segments.eps "Line segments written to a postscript file"  width=.5\textwidth
	 *
	 * This method generates a PDF or PostScript file showing all the segments.
	 *
	 * @param name The filename, use .pdf or .ps as file extension.
	 */
	void showSegments(const char* name) const;


	/**
	 * \brief Visualizes only the intersecting segments
	 *
	 * \image html show_illegal_segments_count.png "Visualization of polyline intersections" width=35%
	* \image latex show_illegal_segments_count.eps "Visualization of polyline intersections"  width=.5\textwidth
	 *
	 * This method highlights the intersecting segments in a PDF or postscript file.
	 *
	 * @param bAlsoEndPointIntersections Flag to include or exclude endpoint intersections as illegal.
	 * @param name he filename, use .pdf or .ps as file extension.
	 */

	void showIllegalSegments(bool bAlsoEndPointIntersections,const char* name) const;

	/** Get illegal segments
	*
	* This method returns the segments involved in intersections. Intersections
	* at endpoints are reported only when \p bAlsoEndPointIntersections is set to true.
	* The result is provided in the output vector \p vIllegalSegmentsOut.
	*
	* \image html segment_intersection_types.png "Types of segment intersections: (1) Non-collinear, (2) collinear, (3) duplicate segments, (4) endpoint intersection"
	*
	* @param bAlsoEndPointIntersections If true, intersections at endpoints will also be detected.
	* @param [out] vIllegalSegmentsOut Output vector to hold the segments involved in intersections.
	*
	*/
	void getIllegalSegments(bool bAlsoEndPointIntersections,std::vector<Segment2*>& vIllegalSegmentsOut) const;


	/**
	 * \brief Determines the intersection type of two segments.
	 *
	 * This method computes the type of intersection between two segments, such as no intersection,
	 * endpoint intersection, or collinear intersection.
	 *
	 * @param pSeg1, pSeg2 The segments to be checked for intersection.
	 * @return The intersection type (e.g., SIT_NONE, SIT_SEGMENT, SIT_POINT, or SIT_ENDPOINT).
	 * @note \p pSeg1 and \p pSeg2 are pointers to arbitrary Segment2 objects. They do not need
	 * to be included in the set that has been used to initialize the SegmentChecker.
	*/
	SegmentIntersectionType getIntersectionType(const Segment2* pSeg1,const Segment2* pSeg2) const;


	/**
	 * \brief Gets all segments intersecting a specified segment.
	 *
	 * This method returns all segments that intersect a given segment along with the type of their intersection.
	 *
	 * @param pTestSegment The segment to check for intersections.
	 * @param bAlsoEndPointIntersections Flag to include endpoint intersections.
	 * @param [out] vIntersectorsOut The output vector holding the intersecting segments and their intersection types.
	 */
	void getIntersectors(	Segment2* pTestSegment,
							bool bAlsoEndPointIntersections,
							std::vector<std::pair<Segment2*,SegmentIntersectionType> >& vIntersectorsOut) const;

	// Intersections
#if GEOM_PSEUDO3D==GEOM_TRUE

	/** Compute the intersection point(s) of two segments.
	*
	* This method computes the intersection point(s) of two segments. Use
	* getIntersectionType() to determine the intersection type \p sit
	* before calling this function. This function should only be used
	* when the intersection type is SIT_POINT or SIT_ENDPOINT.
	*
	* @param sit The intersection type (SIT_POINT or SIT_ENDPOINT).
	* @param seg0, seg1 The two intersecting segments.
	* @param [out] ispOut0 The intersection point on segment \p seg0.
	* @param [out] ispOut1 The intersection point on segment \p seg1.
	*
	* **Details:**
	* - Intersections are computed in 2D, meaning the segments are considered intersecting
	*   if their projections onto the XY plane intersect. However, in 2.5D, their intersection
	*   points may differ in height (z-coordinate). Therefore, two separate intersection points
	*   are returned with identical (x, y) coordinates but possibly different z-values.
	* - \p seg0 and \p seg1 can be any arbitrary Segment2 objects. They do not need to be
	*   part of the segment set initialized in the SegmentChecker.
	*/
	void getIntersectionPoint(	SegmentIntersectionType sit,
								const Segment2& seg0,
								const Segment2& seg1,
								Point2& ispOut0,
								Point2& ispOut1
								) const;
/** Compute the intersection segment(s) of two collinear intersecting segments.
 *
	* This method computes the intersection segment(s) of two segments. Use
	* getIntersectionType() to determine the intersection type \p sit
	* before calling this function. This function should only be used
	* when the intersection type is SIT_SEGMENT.
 *
 * @param seg0, seg1 The intersecting segments, where their SegmentIntersectionType is SIT_SEGMENT.
 * @param [out] issOut0 The intersection segment on segment \p seg0.
 * @param [out] issOut1 The intersection segment on segment \p seg1.
 *
 * **Details**
 * - Intersections are computed in 2D, meaning the segments are considered intersecting
 *   if their projections onto the XY plane intersect. The output intersection segments
 *   will have identical (x, y) coordinates, but possibly different heights (z-values).
 * - \p seg0 and \p seg1 can be any arbitrary Segment2 objects. They do not need
 *   to be part of the segment set initialized in the SegmentChecker.
 */

	void getIntersectionSegment(const Segment2& seg0,
								const Segment2& seg1,
								Segment2& issOut0,
								Segment2& issOut1
								) const;
#else

/** Compute the intersection point of two segments
 *
 * This method computes the intersection point of two segments. Use getIntersectionType() to
 * determine the segment intersection type \p sit before calling this function. Call this
 * function only if \p sit is `SIT_POINT` or `SIT_ENDPOINT`.
 *
 * @param typ The intersection type (`SIT_POINT` or `SIT_ENDPOINT`).
 * @param seg0, seg1 The intersecting segments.
 * @param [out] ispOut The output intersection point.
 *
 * **Details**
 * - \p seg0 and \p seg1 can be any arbitrary Segment2 objects. They do not need
 *   to be part of the segment set used to initialize the SegmentChecker.
 */
	void getIntersectionPoint(	SegmentIntersectionType typ,
								const Segment2& seg0,
								const Segment2& seg1,
								Point2& ispOut
								) const;

/** Computes the intersection segment of two collinear intersecting segments
 *
 * This method computes the intersection segment of two collinear segments
 * that intersect. This function should only be used when the intersection type
 * is SIT_SEGMENT.
 *
 * @param seg0, seg1 The intersecting segments, where their SegmentIntersectionType
 *        is SIT_SEGMENT.
 * @param [out] issOut The computed intersection segment of \p seg0 and \p seg1.
 *
 * **Details**
 * - \p seg0 and \p seg1 can be any arbitrary Segment2 objects. They do not need
 *   to be part of the segment set initialized in the SegmentChecker.
 */
	void getIntersectionSegment(const Segment2& seg0,
								const Segment2& seg1,
								Segment2& issOut
								) const;
#endif

	/** Return the intersection type as a human-readable string.
	*
	* This method converts the given intersection type \p sit to a human-readable string.
	*
	* @param sit The intersection type to be converted to a string.
	*
	* @return A string representation of the intersection type.
	*/
	static const char* getIntersectionTypeString(SegmentIntersectionType sit);



private:
	SegmentChecker(const SegmentChecker& );
	SegmentChecker& operator=(const SegmentChecker&);

	///< private
	SegmentIntersectionType getIntersectionType_degeneratePart(
																const Segment2*& pSeg1,
																const Segment2*& pSeg2,
																Point2& p1,
																Point2& p2,
																Point2& p3,
																Point2& p4
																) const;
	///< private
	void addSegments(const std::vector<Segment2*>& vSegments_);

	// Data
	///< private
	SegmentCheckerData* pSCD;



};

} // Namespace

