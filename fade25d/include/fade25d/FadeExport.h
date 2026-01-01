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

/// @file FadeExport.h
#pragma once

#include <vector>
#include <algorithm>
#include "common.h"

#if GEOM_PSEUDO3D==GEOM_TRUE
	namespace GEOM_FADE25D {
#elif GEOM_PSEUDO3D==GEOM_FALSE
	namespace GEOM_FADE2D {
#else
	#error GEOM_PSEUDO3D is not defined
#endif

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

/** \brief FadeExport is a simple struct to export triangulation data
*
* This data structure is designed to get data out of Fade easily and
* memory efficiently using only basic data types.
*
* @note The important source code for this struct is implemented in the
* `FadeExport.h` file, allowing users to adapt the code for their own needs.
*
* \see https://www.geom.at/triangulation-export/ This article describes exporting triangulations.
*/
struct CLASS_DECLSPEC FadeExport
{
	/** \brief Default constructor */
	FadeExport():
		numCustomIndices(0),numTriangles(0),numPoints(0),
		aCoords(NULL),aCustomIndices(NULL),aTriangles(NULL)
	{
#if GEOM_PSEUDO3D==GEOM_TRUE
		dim=3;
#else
		dim=2;
#endif
	}
	/** \brief Destructor */
	~FadeExport();
	/** \brief Copy constructor */
	FadeExport(const FadeExport& other)
	{
		copy(other);
	}
	/** \brief Assignment operator */
	FadeExport& operator=(const FadeExport& other)
	{
		copy(other);
		return *this;
	}
	/** \brief A copy function
	 *
	 * Copies the \p other FadeExport struct to the current one.
	 *
	 * @param other Another FadeExport struct to copy from
	 */
	void copy(const FadeExport& other)
	{
		numCustomIndices=other.numCustomIndices;
		numTriangles=other.numTriangles;
		numPoints=other.numPoints;
		aCoords=NULL;
		aCustomIndices=NULL;
		aTriangles=NULL;
		dim=other.dim;
		if(other.aTriangles!=NULL)
		{
			size_t siz(3*numTriangles);
			aTriangles=new int[siz];
			std::copy(other.aTriangles,other.aTriangles+siz,aTriangles);
		}
		if(other.aCoords!=NULL)
		{
			size_t siz(dim*numPoints);
			aCoords=new double[siz];
			std::copy(other.aCoords,other.aCoords+siz,aCoords);
		}
		if(other.aCustomIndices!=NULL)
		{
			aCustomIndices=new int[numCustomIndices];
			std::copy(other.aCustomIndices,other.aCustomIndices+numCustomIndices,aCustomIndices);
		}
	}

	/**
	 * \brief Resets the object
	 *
	 * Deletes all dynamically allocated data and resets the number of points,
	 * triangles, and custom indices to 0.
	 */
	void reset()
	{
		if(aCoords!=NULL) delete [] aCoords;
		if(aCustomIndices!=NULL) delete [] aCustomIndices;
		if(aTriangles!=NULL) delete [] aTriangles;

		aCoords=NULL;
		aCustomIndices=NULL;
		aTriangles=NULL;

		numCustomIndices=0;
		numTriangles=0;
		numPoints=0;
	}

	/**
	 * \brief Lexicographically sorts the points
	 *
	 * This method sorts the points in lexicographical order for better
	 * reproducibility. It is mainly used for consistent output in tests
	 * or exports.
	 */
	void lexiSort();


	/**
	 * \brief Prints the data for demonstration purposes
	 *
	 * Prints the points, triangles, and their adjacency relationships for inspection.
	 */
	void print() const;


	/**
	 * \brief Writes the triangulation data to a `.obj` file
	 *
	 * This method exports the triangulation to the popular `.obj` file format,
	 * which is supported by most 3D viewers.
	 *
	 * \param filename The name of the file to write to.
	 * \return `true` if the file was successfully written, `false` otherwise.
	 */
	bool writeObj(const char* filename) const;


	/**
	 * \brief Extracts triangle adjacencies
	 *
	 * This method extracts the adjacency relationships between triangles by
	 * identifying which triangles share an edge. It populates the provided
	 * vector with pairs of adjacent triangle indices.
	 *
	 * \param vNeigs A vector that will be populated with pairs of adjacent triangle indices.
	 */
	void extractTriangleNeighborships(std::vector<std::pair<int,int> >& vNeigs) const;

	/** \brief Get the corner indices of a certain triangle
	 *
	 * \param triIdx [in] triangle index
	 * \param vtxIdx0,vtxIdx1,vtxIdx2 [out] corner indices
	 */
	void getCornerIndices(int triIdx,int& vtxIdx0,int& vtxIdx1,int& vtxIdx2) const;

	/** \brief Get the custom vertex index
	 *
	 * @return the (optional) custom index of the vertex with the index \p vtxIdx
	 * or -1 if the vertex indices have not been exported.
	 *
	 */
	int getCustomIndex(int vtxIdx) const;

#if GEOM_PSEUDO3D==GEOM_TRUE
	/**
	 * \brief Gets the normal vector of a triangle
	 *
	 * This method retrieves the normal vector of the triangle.
	 *
	 * \param triIdx [in] The index of the triangle.
	 * \param[out] x,y,z The components of the normal vector.
	 */
	void getNormal(int triIdx,double& x,double& y,double& z) const;
#endif

#if GEOM_PSEUDO3D==GEOM_TRUE
	/** \brief Get the coorinates for a certain vertex index
	 *
	 * \param vtxIdx [in] vertex index
	 * \param x,y,z [out] coordinates
	 */
	void getCoordinates(int vtxIdx,double& x,double& y,double& z) const;
#else
	/** \brief Get the coorinates for a certain vertex index
	 *
	 * \param vtxIdx [in] vertex index
	 * \param x,y [out] coordinates
	 */
	void getCoordinates(int vtxIdx,double& x,double& y) const;
#endif
	bool operator==(const FadeExport& other) const;
	// DATA
	int numCustomIndices; ///<number of custom indices (same as numPoints when exported, otherwise 0)
	int numTriangles;///< number of triangles
	int numPoints; ///< number of points
	double* aCoords; ///< Cartesian coordinates (dim*numPoints)
	int* aCustomIndices; ///< Custom indices of the points (only when exported)
	int* aTriangles; ///< 3 counterclockwise oriented vertex-indices per triangle (3*numTriangles)
	int dim; ///< Dimension
};

inline FadeExport::~FadeExport()
{
	if(aCoords!=NULL) delete [] aCoords;
	if(aCustomIndices!=NULL) delete [] aCustomIndices;
	if(aTriangles!=NULL) delete [] aTriangles;
	numCustomIndices=0;
	numTriangles=0;
	numPoints=0;
}

inline bool FadeExport::operator==(const FadeExport& other) const
{
	if(numTriangles != other.numTriangles) return false;
	if(numPoints != other.numPoints) return false;

	for(int vtxIdx=0;vtxIdx<numPoints;++vtxIdx)
	{
		for(int component=0;component<dim;++component)
		{
			size_t addr(dim*vtxIdx+component);
			if(aCoords[addr]!=other.aCoords[addr]) return false;
		}
	}

	for(int triIdx=0;triIdx<numTriangles;++triIdx)
	{
		int v0,v1,v2;
		getCornerIndices(int(triIdx),v0,v1,v2);
		int va,vb,vc;
		other.getCornerIndices(int(triIdx),va,vb,vc);
		if(v0!=va || v1!=vb || v2!=vc) return false;
	}
	return true;
}


// For a triangle return the vertex indices
inline void FadeExport::getCornerIndices(int triIdx,int& vtxIdx0,int& vtxIdx1,int& vtxIdx2) const
{
	int base(3*triIdx);
	vtxIdx0=aTriangles[base];
	vtxIdx1=aTriangles[base+1];
	vtxIdx2=aTriangles[base+2];
}

// Print, just for demo purposes
inline void FadeExport::print() const
{
	for(int vtxIdx=0;vtxIdx<numPoints;++vtxIdx)
	{
		int customIndex(-1); // Optional custom index
		if(numCustomIndices>0) customIndex=aCustomIndices[vtxIdx];
		GCOUT<<"\nVertex "<<vtxIdx<<" (customIndex="<<customIndex<<"):";
		for(int component=0;component<dim;++component) GCOUT<<" "<<aCoords[dim*vtxIdx+component];
	}

	for(int triIdx=0;triIdx<numTriangles;++triIdx)
	{
		int v0,v1,v2;
		getCornerIndices(int(triIdx),v0,v1,v2);
		GCOUT<<"\nTriangle "<<triIdx<<": "<<v0<<" "<<v1<<" "<<v2;
	}

	std::vector<std::pair<int,int> > vNeighbors;
	this->extractTriangleNeighborships(vNeighbors);
	for(size_t i=0;i<vNeighbors.size();++i)
	{
		GCOUT<<"\nTriangle "<<vNeighbors[i].first<<" <-> Triangle "<<vNeighbors[i].second;
	}
	GCOUT<<std::endl;
}

// Write an *.obj file
inline bool FadeExport::writeObj(const char* filename) const
{
	std::ofstream outFile(filename);
	if(!outFile.is_open())
	{
		GCOUT<<"Can't write "<<filename<<std::endl;
		return false;
	}
	GCOUT<<"writing "<<filename<<std::endl;

	outFile<<"# Written by Fade2D";
	for(int vtxIdx=0;vtxIdx<numPoints;++vtxIdx)
	{
		outFile<<"\nv";
		for(int component=0;component<dim;++component) outFile<<" "<<aCoords[dim*vtxIdx+component];
		if(dim==2) outFile<<" 0"; // *.obj needs always 3 components, so add z=0
	}
	for(int triIdx=0;triIdx<numTriangles;++triIdx)
	{
		outFile<<"\nf";
		for(int corner=0;corner<3;++corner)
		{
			outFile<<" "<<aTriangles[3*triIdx+corner]+1; // +1 because in *.obj format indices start at 1, not 0.
		}
	}
	outFile<<std::endl;
	outFile.close();
	return true;
}


inline void FadeExport::extractTriangleNeighborships(std::vector<std::pair<int,int> >& vNeigs) const
{
	vNeigs.reserve(numTriangles*3/2);
	std::vector<std::pair<std::pair<int,int>,int> > vVtxPair2Tri;
	vVtxPair2Tri.reserve(numTriangles*3);

	for(int tri=0;tri<numTriangles;++tri)
	{
		size_t vtxIdx(3*tri);
		int vtx0(aTriangles[vtxIdx]);
		int vtx1(aTriangles[vtxIdx+1]);
		int vtx2(aTriangles[vtxIdx+2]);
		if(vtx0>vtx1) std::swap(vtx0,vtx1);
		if(vtx1>vtx2)
		{
			std::swap(vtx1,vtx2);
			if(vtx0>vtx1) std::swap(vtx0,vtx1);
		}
		vVtxPair2Tri.push_back(std::make_pair(std::make_pair(vtx0,vtx1),tri));
		vVtxPair2Tri.push_back(std::make_pair(std::make_pair(vtx1,vtx2),tri));
		vVtxPair2Tri.push_back(std::make_pair(std::make_pair(vtx0,vtx2),tri));
	}
	std::sort(vVtxPair2Tri.begin(),vVtxPair2Tri.end());
	for(size_t i=0;i<vVtxPair2Tri.size();++i)
	{
		int vtx0(vVtxPair2Tri[i].first.first);
		int vtx1(vVtxPair2Tri[i].first.second);
		int tri0(vVtxPair2Tri[i].second);

		if(	++i<vVtxPair2Tri.size() &&
			vVtxPair2Tri[i].first.first==vtx0 &&
			vVtxPair2Tri[i].first.second==vtx1)
		{
			int tri1(vVtxPair2Tri[i].second);
			vNeigs.push_back(std::pair<int,int>(tri0,tri1));
		}
		--i;
	}
}


#if GEOM_PSEUDO3D==GEOM_TRUE
inline void FadeExport::getCoordinates(int vtxIdx,double& x,double& y,double& z) const
{
	int base(dim*vtxIdx);
	x=aCoords[base];
	y=aCoords[base+1];
	z=aCoords[base+2];
}
#else
inline void FadeExport::getCoordinates(int vtxIdx,double& x,double& y) const
{
	int base(dim*vtxIdx);
	x=aCoords[base];
	y=aCoords[base+1];
}
#endif

inline int FadeExport::getCustomIndex(int vtxIdx) const
{
	if(vtxIdx<numCustomIndices)
	{
		return aCustomIndices[vtxIdx];
	}
	return -1;
}
#ifdef _WIN32
#pragma warning(pop)
#endif
} // (namespace)





