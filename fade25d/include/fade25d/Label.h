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

/// @file Label.h
#pragma once


#include "Point2.h"
#include "common.h"
#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

struct LDat; // FWD

/**
 * \brief Represents a text label for visualization.
 *
 * The `Label` class defines a text label that can be displayed at a specified point
 * in a 2D space. Labels are used in visualizations via the `Visualizer2` class.
 *
 * \see Visualizer2 where `Label` objects are used.
 */

class CLASS_DECLSPEC Label
{

public:
	/**
	* \brief Constructs a text label at a specified point.
	*
	* This constructor initializes a `Label` with a given position, text, and options
	* for displaying the text with or without a mark. It also sets the font size.
	*
	* \param p_ The position where the label appears.
	* \param s_ The text to be displayed in the label.
	* \param bWithMark_ A boolean flag indicating whether to display the text with an cross mark (default: true).
	* \param fontSize_ The font size of the label text (default: 8).
	*/
	Label(const Point2& p_,const char* s_,bool bWithMark_=true,int fontSize_=8);

	/** \brief Copy constructor */
	Label(const Label& other);
	/** \brief Assignment operator */
	Label& operator=(const Label& other);
	/** \brief Destructor */
	~Label();
	/**
     * \brief Gets the text string of the label.
     *
     * This method retrieves the text stored in the label.
     *
     * \return A pointer to the text string of the label.
     */
	const char* getCS() const;
	// Data
	LDat* pDat;
	Point2 p;
	bool bWithMark;
	int fontSize;
};



} // (namespace)
