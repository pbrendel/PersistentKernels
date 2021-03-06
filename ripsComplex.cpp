// pbrendel (c) 2013-21

#include "ripsComplex.h"
#include "metrics.h"
#include "Core/deque.h"

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
    CreateNeighbours( metrics, epsilon );
    CreateEdges();
}


void RipsComplex::CreateVerts( const PointsList &points )
{
    m_vertsCount = points.GetSize();
    m_verts = new Vertex[m_vertsCount];;
    Vertex *verts = m_verts.Get();
    for ( uint i = 0; i < m_vertsCount; ++i )
    {
        verts[i].m_point = &points[i];
    }
}


void RipsComplex::GluePoints( const Metrics &metrics )
{
    assert( !metrics.IsIndexMetrics() );
    Vertex *verts = m_verts.Get();
    constexpr Label LABEL_REMOVED = O_INVALID_INDEX - 1;
    struct LabelRemove
    {
        bool operator()( const RipsComplex::Vertex &vertex ) { return vertex.m_label == LABEL_REMOVED; }
    };
    for ( uint i = 0; i < m_vertsCount; ++i )
    {
        for ( uint j = i + 1; j < m_vertsCount; ++j )
        {
            if ( metrics.GetDistance( *verts[i].m_point, *verts[j].m_point, i, j ) == 0 )
            {
                verts[j].m_label = LABEL_REMOVED;
            }
        }
    }
    const Vertex *vertsEnd = std::remove_if( verts, verts + m_vertsCount, LabelRemove() );
    m_vertsCount = static_cast<uint>( vertsEnd - verts );
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


void RipsComplex::CreateNeighbours( const Metrics &metrics, double epsilon )
{
    const Vertex *verts = m_verts.Get();
    m_neighbours.Init( MAX_VERTEX_DEGREE, m_vertsCount );
    for ( uint i = 0; i < m_vertsCount; ++i )
    {
        Simplex n = m_neighbours.PushBack();
        uint d = 0;
        for ( uint j = i + 1; j < m_vertsCount; ++j )
        {
            if ( metrics.GetDistance( *verts[i].m_point, *verts[j].m_point, i, j ) <= epsilon )
            {
                n[d++] = j;
            }
        }
        if ( d < MAX_VERTEX_DEGREE )
        {
            n[d] = O_INVALID_INDEX;
        }
    }
}


void RipsComplex::CreateEdges()
{
    m_edges.Init( 1, m_vertsCount * 4 );
    for ( uint i = 0; i < m_vertsCount; ++i )
    {
        Simplex n = m_neighbours[i];
        uint d = 0;
        while ( d <= MAX_VERTEX_DEGREE &&  n[d] != SimplexSet::INVALID_LABEL )
        {
            Simplex edge = m_edges.PushBack();
            edge[0] = i;
            edge[1] = n[d++];
        }
    }
}


void RipsComplex::CreateConnectedComponents()
{
    // Create "reverse neighbours" map
    SimplexSet reverseNeighbours( MAX_VERTEX_DEGREE, m_vertsCount );
    DynArray<uint> neighboursCount( m_vertsCount );
    neighboursCount.Clear();
    for ( uint i = 0; i < m_vertsCount; ++i )
    {
        ConstSimplex n = m_neighbours[i];
        uint d = 0;
        while ( d <= MAX_VERTEX_DEGREE && n[d] != SimplexSet::INVALID_LABEL )
        {
            const uint j = n[d++];
            const uint c = neighboursCount[j]++;
            reverseNeighbours[j][c] = i;
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
            Simplex n = m_neighbours[q.Front()];
            uint d = 0;
            while ( d <= MAX_VERTEX_DEGREE && n[d] != SimplexSet::INVALID_LABEL )
            {
                const uint v = static_cast<uint>( n[d++] );
                if ( verts[v].m_ccIndex == O_INVALID_INDEX )
                {
                    verts[v].m_ccIndex = newCcIndex;
                    q.PushBack( v );
                }
            }
            n = reverseNeighbours[q.Front()];
            d = 0;
            while ( d <= MAX_VERTEX_DEGREE && n[d] != SimplexSet::INVALID_LABEL )
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
