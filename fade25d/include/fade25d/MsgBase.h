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

/// @file MsgBase.h
#pragma once
#include "common.h"


#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif


/**
 * \brief Message types for the message system
 *
 * Enum that defines different types of messages that can be sent
 * by the system. This is used to specify the type of message being
 * passed to the subscriber.
 */
enum MsgType
{
	MSG_PROGRESS,  /**< Message indicating progress */
	MSG_WARNING /**< Message indicating a warning */
};

/** \brief MsgBase, a base class for message subscriber classes
 *
 * MsgBase serves as a base class from which message subscriber
 * classes (e.g., widgets, progress bars, etc.) can be derived. These
 * subscriber classes receive messages such as progress updates or warnings
 * from Fade.
 *
 * \sa [Progress bar example](http://www.geom.at/progress-bar/)
 */
class CLASS_DECLSPEC MsgBase
{
public:
	/** \brief Default constructor */
	MsgBase(){};
	/** \brief Destructor */
	virtual ~MsgBase(){}
	/** \brief update
	 *
	 * This method must be implemented in derived classes. It is automatically
	 * called whenever the Fade system has a message of type \p msgType to deliver.
	 *
	 * \param msgType The type of message (e.g., progress, warning).
	 * \param s A string message.
	 * \param d A numeric value associated with the message (e.g., progress value).
	 */
	virtual void update(MsgType msgType,const char* s,double d)=0;
};


} // Namespace

