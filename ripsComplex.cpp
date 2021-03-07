// pbrendel (c) 2013-21

#include "ripsComplex.h"
#include "metrics.h"
#include "Core/deque.h"
#include "Core/dynBuffer.h"

#include <algorithm>

using o::DynArray;


RipsComplex::RipsComplex( const PointsList &points, const Metrics &metrics, double epsilon, bool gluePoints )
{
    Create( points, metrics, epsilon, gluePoints );
}


void RipsComplex::Create( const PointsList &points, const Metrics &metrics, double epsilon, bool gluePoints )
{
    CreateVerts( points );
    if ( gluePoints )
    {
        GluePoints( metrics );
    }
    AssignLabels();
    CreateEdges( metrics, epsilon );
}


void RipsComplex::CreateVerts( const PointsList &points )
{
    m_vertsCount = points.GetSize();
    m_verts.Reset( m_vertsCount );
    for ( uint i = 0; i < m_vertsCount; ++i )
    {
        m_verts[i].m_point = &points[i];
    }
}


o::DynBuffer<RipsComplex::VertexRefDist> RipsComplex::CalculateVertexReferenceDistance( const Metrics &metrics )
{
    struct VertexRefDistComparer
    {
        bool operator()( const VertexRefDist &a, const VertexRefDist &b ) { return a.m_distance < b.m_distance; }
    };

    o::DynBuffer<VertexRefDist> vertRefDist( m_vertsCount );
    const Vertex *verts = m_verts.Get();
    const Point *center = verts[0].m_point;
    vertRefDist[0].m_index = 0;
    vertRefDist[0].m_distance = 0.0;
    for ( uint i = 1; i < m_vertsCount; ++i )
    {
        const double d = metrics.GetDistance( *center, *verts[i].m_point, 0, i );
        vertRefDist[i].m_index = i;
        vertRefDist[i].m_distance = d;
    }
    std::sort( vertRefDist.Get(), vertRefDist.Get() + m_vertsCount, VertexRefDistComparer() );
    return vertRefDist;
}


void RipsComplex::GluePoints( const Metrics &metrics )
{
    assert( !metrics.IsIndexMetrics() );
    
    o::DynBuffer<VertexRefDist> vertsRefDist = CalculateVertexReferenceDistance( metrics );

    constexpr Label LABEL_REMOVED = O_INVALID_INDEX - 1;
    struct LabelRemove
    {
        bool operator()( const RipsComplex::Vertex &vertex ) { return vertex.m_label == LABEL_REMOVED; }
    };

    // For each group of the same distance, check distance between the points and mark vertices to be removed.
    uint start = 0;
    while ( start < m_vertsCount )
    {
        const double d = vertsRefDist[start].m_distance;
        uint end = start + 1;
        while ( end < m_vertsCount && vertsRefDist[end].m_distance == d )
        {
            end++;
        }
        if ( end > start + 1 )
        {
            for ( uint i = start; i < end; ++i )
            {
                const uint indexI = vertsRefDist[i].m_index;
                const Vertex &v = m_verts[indexI];
                if ( v.m_label == LABEL_REMOVED )
                {
                    continue;
                }
                for ( uint j = i + 1; j < end; ++j )
                {
                    const uint indexJ = vertsRefDist[j].m_index;
                    if ( metrics.GetDistance( *v.m_point, *m_verts[indexJ].m_point, indexI, indexJ ) == 0 )
                    {
                        m_verts[indexJ].m_label = LABEL_REMOVED;
                    }
                }
            }
        }
        start = end;
    }

    const Vertex *vertsEnd = std::remove_if( m_verts.Get(), m_verts.Get() + m_vertsCount, LabelRemove() );
    m_vertsCount = static_cast<uint>( vertsEnd - m_verts.Get() );
}


void RipsComplex::AssignLabels()
{
    Vertex *verts = m_verts.Get();
    Label label = 0;
    for ( uint i = 0; i < m_vertsCount; ++i )
    {
        verts[i].m_label = label++;
    }
}


void RipsComplex::CreateEdges( const Metrics &metrics, double epsilon )
{
    o::DynBuffer<VertexRefDist> vertsRefDist = CalculateVertexReferenceDistance( metrics );
    
    m_vertexDegree = 0;
    m_edges.Init( 1, m_vertsCount * 4 );
    o::DynBuffer<uint> vertDegree( m_vertsCount );
    vertDegree.Clear();

    // Chech for connectibity within each group of distance no greater than 'epsilon'
    uint start = 0;
    while ( start < m_vertsCount )
    {
        const double d = vertsRefDist[start].m_distance;
        uint end = start + 1;
        while ( end < m_vertsCount && vertsRefDist[end].m_distance == d )
        {
            end++;
        }
        const uint endGroup = end;
        while ( end < m_vertsCount && vertsRefDist[end].m_distance <= d + epsilon )
        {
            end++;
        }
        if ( end > start + 1 )
        {
            for ( uint i = start; i < end; ++i )
            {
                const uint indexI = vertsRefDist[i].m_index;
                const Vertex &v = m_verts[indexI];
                for ( uint j = i + 1; j < end; ++j )
                {
                    const uint indexJ = vertsRefDist[j].m_index;
                    if ( metrics.GetDistance( *v.m_point, *m_verts[indexJ].m_point, indexI, indexJ ) <= epsilon )
                    {
                        Simplex edge = m_edges.PushBack();
                        edge[0] = indexI;
                        edge[1] = indexJ;
                        m_vertexDegree = (std::max)( m_vertexDegree, ++vertDegree[indexI] );
                        m_vertexDegree = (std::max)( m_vertexDegree, ++vertDegree[indexJ] );
                    }
                }
            }
        }
        start = endGroup;
    }
}


void RipsComplex::CreateConnectedComponents()
{
    // Create "reverse neighbours" map
    SimplexSet neighbours( m_vertexDegree, m_vertsCount );
    neighbours.Resize( m_vertsCount );
    o::DynBuffer<uint> neighboursCount( m_vertsCount );
    neighboursCount.Clear();
    const uint edgesCount = m_edges.GetSize();
    for ( uint i = 0; i < edgesCount; ++i )
    {
        ConstSimplex e = m_edges[i];
        {
            const uint v = e[0];
            const uint n = neighboursCount[v]++;
            neighbours[v][n] = e[1];
        }
        {
            const uint v = e[1];
            const uint n = neighboursCount[v]++;
            neighbours[v][n] = e[0];
        }
    }

    // Do a simple BFS to mark connected components.
    Vertex *verts = m_verts.Get();
    for ( uint i = 0; i < m_vertsCount; ++i )
    {
        if ( m_verts[i].m_ccIndex != O_INVALID_INDEX )
        {
            continue;
        }
        const uint newCcIndex = m_ccRepresentative.GetSize();
        m_ccRepresentative.PushBack( i );
        o::Deque<uint> q;
        q.PushBack( i );
        verts[i].m_ccIndex = newCcIndex;
        while ( !q.IsEmpty() )
        {
            Simplex n = neighbours[q.Front()];
            const uint count = neighboursCount[q.Front()];
            for ( uint d = 0; d < count; ++d )
            {
                const uint v = static_cast<uint>( n[d++] );
                if ( verts[v].m_ccIndex == O_INVALID_INDEX )
                {
                    verts[v].m_ccIndex = newCcIndex;
                    q.PushBack( v );
                }
            }
            q.PopFront();
        }
    }
}


uint RipsComplex::GetConnectedComponentsNumber() const
{
    assert( !m_ccRepresentative.IsEmpty() );
    return m_ccRepresentative.GetSize();
}


void RipsComplex::GetProjectionMap( const RipsComplex &rangeComplex, o::Map<uint, uint> &outProjection ) const
{
    for ( DynArray<uint>::ConstIterator v = m_ccRepresentative.Begin(); v != m_ccRepresentative.End(); ++v )
    {
        outProjection[*v] = rangeComplex.m_ccRepresentative[rangeComplex.m_verts[*v].m_ccIndex];
    }
}
