// pbrendel (c) 2013-21

#pragma once

#include "simplexSet.h"
#include "point.h"
#include "Core/defs.h"
#include "Core/dynArray.h"
#include "Core/dynBuffer.h"
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

     struct VertexRefDist
     {
         uint m_index;
         double m_distance;
     };

     void Create( const PointsList &points, const Metrics &metrics, double epsilon, bool gluePoints );
     void CreateVerts( const PointsList &points );
     o::DynBuffer<VertexRefDist> CalculateVertexReferenceDistance( const Metrics &metrics );
     void GluePoints( const Metrics &metrics );
     void AssignLabels();
     void CreateEdges( const Metrics &metrics, double epsilon );

     o::DynBuffer<Vertex> m_verts;
     uint m_vertsCount;
     SimplexSet m_edges;
     o::DynArray<uint> m_ccRepresentative;
     uint m_vertexDegree;

     friend class DataWriter;
};
