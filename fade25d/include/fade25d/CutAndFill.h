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
/// @file CutAndFill.h

#pragma once
#include "common.h"
#include "MsgBase.h"
#include "VtkWriter.h"
#include "CAF_Component.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {

class Zone2; // FWD;
class CutAndFillImpl; // FWD
class Visualizer2; // FWD
class Fade_2D; // FWD
/**
 * \brief Computes the volume differences between two overlapping surfaces.
 *
 * Given two overlapping surfaces with different elevations, the `CutAndFill` algorithm:
 * - Computes the overlapping part.
 * - Partitions the overlapping area into components of type "cut", "fill" and "zero".
 * - Computes the volumes that must be removed (cut) or added (fill) to transform one surface into the other.
 *
 * \image html cut_and_fill_surfaces.png "Cut-and-fill: The original and the desired surface after the earthworks" width=60%
 * \image latex cut_and_fill_surfaces.eps "Cut-and-fill: The original and the desired surface after the earthworks" width=10cm
 *
 * \note The method works also if the surfaces do not match perfectly, as it only requires an overlapping area for the calculation.
 *
 * \sa https://www.geom.at/cut-and-fill/
 */
class CLASS_DECLSPEC CutAndFill
{
public:
	/**
	* \brief Constructor
	*
	* Initializes the `CutAndFill` object with the specified surface data.
	*
	* \param pZoneBefore A pointer to the surface before the earthworks (Zone2 object).
	* \param pZoneAfter A pointer to the surface after the earthworks (Zone2 object).
	* \param ignoreThreshold A threshold value to ignore insignificant
	* height differences (default: 1e-6).
	*/
	CutAndFill(Zone2* pZoneBefore,Zone2* pZoneAfter,double ignoreThreshold=1e-6);

    /** \brief Destructor */
	~CutAndFill();

	/**
	 * \brief Retrieves the difference zone between the two surfaces.
	 *
	 * This method returns a Zone2 of the overlapping area in which the z-values
	 * represent the height differences of the input surfaces ('surface-before' minus 'surface-after').
	 * Additionally, a map is returned that contains the height values for each vertex of
	 * this zone in the two input surfaces.
	 *
	 * \param[out] pDiffZone A Zone2, in which the z-coordinates represent the height
	 * differences ('surface-before' minus 'surface-after').
	 * \param[out] mVtx2BeforeAfter A map that associates each vertex of \p pDiffZone
	 * with the height values from both the before and after surfaces.
	 *
	 * \return `true` if the operation succeeds, or `false` if the two input surfaces
	 * do not share a common area. In this case, the previous call to `CutAndFill::go()`
	 * would have already returned `false`.
	 *
	 * \see https://www.geom.at/cut-and-fill/#difference-zone
	 */
	bool getDiffZone(	Zone2*& pDiffZone,
						std::map<Point2*,std::pair<double,double> >& mVtx2BeforeAfter);

	/**
	* \brief Registers a message receiver object to receive progress updates.
	*
	* A user-defined progress bar or message receiver object can be registered to
	* receive progress information during the computation. This step is optional.
	*
	* \param msgType The message type (for progress updates, the type is always `MSG_PROGRESS`).
	* \param pMsg A pointer to the user-defined message receiver object, which must derive from `MsgBase`.
	*
	* \see https://www.geom.at/cut-and-fill/
	*/
	void subscribe(MsgType msgType,MsgBase* pMsg);

	/**
	* \brief Unregisters a message receiver object.
	*
	* Removes a previously registered message receiver object for progress updates.
	*
	* \param msgType The message type (for progress updates, the type is always `MSG_PROGRESS`).
	* \param pMsg A pointer to the user-defined message receiver object, which must derive from `MsgBase`.
	*
	* \see https://www.geom.at/cut-and-fill/
	*/
	void unsubscribe(MsgType msgType,MsgBase* pMsg);

	/** @brief Get the number of components
	 *
	 * This method returns the number of CAF_Component objects.
	 *
	 * @return the number of components.
	 *
	 *
	 */
	size_t getNumberOfComponents() const;

	/**
	 * \brief Retrieves the component at the specified index.
	 *
	 * A CAF_Component object represents a connected part of the surface
	 * such that
	 * - the first surface is below the second one (Type CT_FILL)
	 * - the first surface is above the second one (Type CT_CUT)
	 * - the first surface corresponds to the second one (Type CT_NULL)
	 *
	 * \param[in] idx The index of the component.
	 *
	 * \return A pointer to the `CAF_Component` at the specified index.
	 */
	CAF_Component* getComponent(size_t idx) const;

	/**
	* \brief Starts the computation of the cut and fill volumes.
	*
	* This method starts the computation of the volume difference between the two
	* surfaces.
	*
	* \param[in] bWithMessages If `true`, messages and warnings will be displayed during
	* the computation. If `false`, warnings will be suppressed.
	*
	* \return `true` if the computation succeeds, `false` otherwise (e.g., if there is no overlapping area).
	*/
	bool go(bool bWithMessages=true);

	/**
	* \brief Draws a postscript or PDF visualization of the result.
	*
	* This method generates a postscript or PDF image showing the cut and fill areas, providing
	* a quick visual overview of the computation results.
	*
	* \image html cut_and_fill_result.png "Cut-and-fill result: YELLOW area = CUT, CYAN area = FILL" width=40%
	* \image latex cut_and_fill_result.png "Cut-and-fill result: YELLOW area = CUT, CYAN area = FILL" width=10cm
	*
	* \param pVis A pointer to the `Visualizer2` object that handles the visualization.
	*
	* \note You must call `pVis->writeFile()` to finalize the visualization.
	*/
	void show(Visualizer2* pVis) const;
	/**
	* \brief Visualizes the result as a .VTK file
	*
	* This method visualizes the result using a VtkWriter, which can generate
	* VTK files for visualization in external programs such as ParaView.
	*
	* \param pVtkWriter A pointer to an existing `VtkWriter` that will handle the visualization.
	* \param cutColor The color to represent the cut area.
	* \param fillColor The color to represent the fill area.
	* \param nullColor The color to represent the null area (neither cut nor fill).
	*
	* \note You must call `pVtkWriter->writeFile()` to write the output file.
	*/
	void showVtk(VtkWriter* pVtkWriter,VtkColor cutColor,VtkColor fillColor,VtkColor nullColor) const;
protected:
/// @private
	CutAndFillImpl* pCAFI;
private:
	CutAndFill(const CutAndFill& ):pCAFI(NULL)
	{
		// Never reached
	}
	CutAndFill& operator=(const CutAndFill& )
	{
		pCAFI=NULL;
		return *this;
	}
};

} // Namespace Fade25D only

#elif GEOM_PSEUDO3D==GEOM_FALSE
#else
	#error GEOM_PSEUDO3D is not defined
#endif


