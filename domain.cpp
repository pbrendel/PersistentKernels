// pbrendel (c) 2013-21

#include "domain.h"
#include "metrics.h"
#include "noise.h"
#include "Core/assert.h"

#include <algorithm>

using o::DynArray;



////////////////////////////////////////////////////////////////////////////////

UniformCube::UniformCube( const Cube &cube, const DynArray<uint> &resolution, Noise *noise )
    : Domain( cube, noise )
    , m_resolution( resolution )
{
    m_count = 0;
    for ( uint i = 1; i < resolution.GetSize(); ++i )
    {
        m_count *= resolution[i];
    }
}


void UniformCube::GetValue( uint index, Point &p ) const
{
    const uint dim = GetDimension();
    assert( p.GetDimension() == dim );
    uint d = 0;
    while ( d < dim )
    {
        p[d] = m_cube[d].m_min + m_cube[d].GetLength() * (double)( index % m_resolution[d] ) / std::max( 1u, m_resolution[d] - 1 );
        index = index / m_resolution[d];
        d++;
    }
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( p );
    }
}

////////////////////////////////////////////////////////////////////////////////

bool UniformCubeWithHole::IsInDomain( const Point &p ) const
{
    if ( !Domain::IsInDomain( p ) )
    {
        return false;
    }
    return !m_hole.IsInside( p );
}


void UniformCubeWithHole::GetValue( uint index, Point &p ) const
{
    const uint dim = GetDimension();
    assert( p.GetDimension() == dim );
    const uint maxSteps = 100;
    uint step = 0;
    do
    {
        uint d = 0;
        while ( d < dim )
        {
            p[d] = m_cube[d].m_min + m_cube[d].GetLength() * (double)( index % m_resolution[d] ) / std::max( 1u, m_resolution[d] - 1 );
            index = index / m_resolution[d];
            d++;
        }
    } while ( m_hole.IsInside( p ) && step++ < maxSteps );
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( p );
    }
}

////////////////////////////////////////////////////////////////////////////////

RandomCube::RandomCube( const Cube &cube, uint count, Noise *noise )
    : Domain( cube, noise )
    , m_randoms( cube.GetDimension() *count )
{
    const uint size = m_randoms.GetSize();
    for ( uint i = 0; i < size; ++i )
    {
        m_randoms[i] = static_cast<double>( rand() % RAND_MAX ) / ( RAND_MAX - 1 );
    }
}


void RandomCube::GetValue( uint index, Point &p ) const
{
    const uint dim = GetDimension();
    assert( p.GetDimension() == dim );
    uint d = 0;
    while ( d < dim )
    {
        p[d] = m_cube[d].m_min + m_cube[d].GetLength() * m_randoms[index * dim + d];
        d++;
    }
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( p );
    }
}

////////////////////////////////////////////////////////////////////////////////

bool RandomCubeWithHole::IsInDomain( const Point &p ) const
{
    if ( !Domain::IsInDomain( p ) )
    {
        return false;
    }
    return !m_hole.IsInside( p );
}


void RandomCubeWithHole::GetValue( uint index, Point &p ) const
{
    const uint dim = GetDimension();
    assert( p.GetDimension() == dim );
    const uint maxSteps = 100;
    uint step = 0;
    do
    {
        uint d = 0;
        while ( d < dim )
        {
            p[d] = m_cube[d].m_min + m_cube[d].GetLength() * m_randoms[index * dim + d];
            d++;
        }
    } while ( m_hole.IsInside( p ) && step++ < maxSteps );
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( p );
    }
}

////////////////////////////////////////////////////////////////////////////////

void DomainRestriction::GetValue( uint index, Point &p ) const
{
    const uint dim = GetDimension();
    assert( p.GetDimension() == dim );
    assert( index < m_points.GetSize() );
    for ( uint i = 0; i < dim; ++i )
    {
        p[i] = m_points[index][i];
    }
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( p );
    }
}


void DomainRestriction::Create( const Domain &other, const Metrics &metrics, const Point &center, double radius )
{
    const uint dim = GetDimension();
    assert( center.GetDimension() == dim );
    m_points.Clear();
    Point p( dim );
    const uint count = other.GetCount();
    for ( uint i = 0; i < count; ++i )
    {
        other.GetValue( i, p );
        if ( metrics.GetDistance( p, center, i, Metrics::NO_INDEX ) <= radius )
        {
            m_points.PushBack( p );
        }
    }
    m_count = m_points.GetSize();
}
