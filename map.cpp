// pbrendel (c) 2013-21

#include "map.h"
#include "cube.h"
#include "domain.h"
#include "noise.h"

using o::DynArray;


LinearMap::LinearMap( uint dim, const DynArray<double> &factors, const Noise *noise )
    : Map( dim, noise )
    , m_factors( factors )
{
    assert( factors.GetSize() == dim );
}


void LinearMap::GetValue( const Point &d, Point &r ) const
{
    assert( d.GetDimension() == m_dim );
    assert( r.GetDimension() == m_dim );
    for ( uint i = 0; i < m_dim; ++i )
    {
        r[i] = d[i] * m_factors[i];
    }
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( r );
    }
}

////////////////////////////////////////////////////////////////////////////////

LinearDiscMap::LinearDiscMap( uint dim, const DynArray<double> &factors, const Domain &domain, const Noise *noise )
    : Map( dim, noise )
    , m_factors( factors )
{
    assert( factors.GetSize() == dim );
    for ( uint i = 0; i < dim; ++i )
    {
        Interval interval = domain.GetInterval( i );
        interval.m_min = interval.GetCenter();
        m_intervals.PushBack( interval );
    }
}


void LinearDiscMap::GetValue( const Point &d, Point &r ) const
{
    assert( d.GetDimension() == m_dim );
    assert( r.GetDimension() == m_dim );
    for ( uint i = 0; i < m_dim; ++i )
    {
        if ( d[i] < m_intervals[i].m_min )
        {
            r[i] = d[i] * m_factors[i];
        }
        else
        {
            r[i] = ( m_intervals[i].m_max - ( d[i] - m_intervals[i].m_min ) ) * m_factors[i];
        }
    }
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( r );
    }
}

////////////////////////////////////////////////////////////////////////////////

TestDisc2To1::TestDisc2To1( const Domain &domain, double factor, const Noise *noise )
    : Map( 1, noise )
    , m_factor( factor )
{
    assert( domain.GetDimension() == 2 );
    Cube cube = domain.GetCube();
    m_centerX = cube[0].GetCenter();
    m_centerY = cube[1].GetCenter();
}


void TestDisc2To1::GetValue( const Point &d, Point &r ) const
{
    assert( d.GetDimension() == 2 );
    assert( r.GetDimension() == 1 );
    if ( d[0] < m_centerX )
    {
        r[0] = 0;
    }
    else
    {
        r[0] = ( d[1] > m_centerY ) ? m_factor * d[0] : -m_factor * d[0];
    }
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( r );
    }
}

////////////////////////////////////////////////////////////////////////////////

Translation::Translation( uint dim, Point offset, const Noise *noise )
    : Map( dim, noise )
    , m_offset( offset )
{
    assert( offset.GetDimension() == dim );
}


void Translation::GetValue( const Point &d, Point &r ) const
{
    assert( d.GetDimension() == m_dim );
    assert( r.GetDimension() == m_dim );
    for ( uint i = 0; i < m_dim; ++i )
    {
        r[i] = d[i] + m_offset[i];
    }
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( r );
    }
}
