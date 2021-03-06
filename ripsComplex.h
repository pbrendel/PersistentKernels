// pbrendel (c) 2013-21

#pragma once

#include "simplexSet.h"
#include "point.h"
#include "Core/defs.h"
#include "Core/dynArray.h"
#include "Core/map.h"

class Metrics;

// 1-d RipsComplex

class RipsComplex 
{

public:

    RipsComplex( const PointsList &points, const Metrics &metrics, double epsilon, bool gluePoints );

    void CreateConnectedComponents();
    uint GetConnectedComponentsNumber() const;
    void GetProjectionMap( const RipsComplex &rangeComplex, o::Map<uint, uint> &outProjection ) const;

 private:

     enum : uint
     {
         MAX_VERTEX_DEGREE = 5,
     };

     struct Vertex
     {
         const Point *m_point;
         Label m_label;
         uint m_ccIndex;

         Vertex()
             : m_point( nullptr )
             , m_label( O_INVALID_INDEX )
             , m_ccIndex( O_INVALID_INDEX )
         {}
     };

     void Create( const PointsList &points, const Metrics &metrics, double epsilon, bool gluePoints );
     void CreateVerts( const PointsList &points );
     void GluePoints( const Metrics &metrics );
     void AssignLabels();
     void CreateNeighbours( const Metrics &metrics, double epsilon );
     void CreateEdges();

     o::Ptr<Vertex> m_verts;
     uint m_vertsCount;
     SimplexSet m_edges;
     SimplexSet m_neighbours;
     o::DynArray<uint> m_ccRepresentative;

     friend class DataWriter;
};
