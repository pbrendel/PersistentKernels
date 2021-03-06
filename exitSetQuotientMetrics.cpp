// pbrendel (c) 2013-21

#include "exitSetQuotientMetrics.h"
#include "domain.h"
#include "map.h"

#include <limits>


ExitSetQuotientMetrics::ExitSetQuotientMetrics( const Domain &domain, const Map &map, const Metrics &innerMetrics )
    : m_domain( domain )
    , m_innerMetrics( innerMetrics )
{
    assert( !innerMetrics.IsIndexMetrics() );
    const uint domainSize = domain.GetCount();
    if ( domainSize == 0 )
    {
        return;
    }
    const uint domainDim = domain.GetDimension();
    Point p( domainDim ), v( domainDim ), v1( domainDim );
    // For all points from domain AND image (not only from domain),
    // we need to check whether their image lies in the domain.
    for ( uint i = 0; i < domainSize; ++i )
    {
        domain.GetValue( i, p );
        map.GetValue( p, v );
        if ( domain.IsInDomain( v ) )
        {
            m_points.PushBack( MyPoint( p ) );
            map.GetValue( v, v1 );
            if ( domain.IsInDomain( v1 ) )
            {
                m_points.PushBack( MyPoint( v ) );
            }
            else
            {
                m_exitSetPoints.PushBack( v );
            }
        }
        else
        {
            m_exitSetPoints.PushBack( p );
        }
    }
    // Finally, for all non-exit set points, find nearest exit set point 
    // and store the distance.
    for ( MyPointsList::Iterator it = m_points.Begin(); it != m_points.End(); ++it )
    {
        for ( PointsList::Iterator it1 = m_exitSetPoints.Begin(); it1 != m_exitSetPoints.End(); ++it1 )
        {
            const double distance = innerMetrics.GetDistance( it->m_point, *it1, Metrics::NO_INDEX, Metrics::NO_INDEX );
            if ( distance < it->m_distance )
            {
                it->m_distance = distance;
            }
        }
    }
}


double ExitSetQuotientMetrics::GetDistance( const Point &x, const Point &y, uint, uint ) const
{
    const double dx = GetDistanceToExitSet( x );
    const double dy = GetDistanceToExitSet( y );
    if ( dx == 0 || dy == 0 )
    {
        return dx + dy;
    }
    return std::min( dx + dy, m_innerMetrics.GetDistance( x, y, Metrics::NO_INDEX, Metrics::NO_INDEX ) );
}


double ExitSetQuotientMetrics::AddPoint( const Point &p, const Map &map )
{
    if ( !m_domain.IsInDomain( p ) )
    {
        return 0.0;
    }
    Point v( m_domain.GetDimension() );
    map.GetValue( p, v );
    if ( !m_domain.IsInDomain( v ) )
    {
        m_exitSetPoints.PushBack( p );
        return 0.0;
    }
    MyPoint p1( p );
    for ( PointsList::Iterator it = m_exitSetPoints.Begin(); it != m_exitSetPoints.End(); ++it )
    {
        const double distance = m_innerMetrics.GetDistance( p, *it, Metrics::NO_INDEX, Metrics::NO_INDEX );
        if ( distance < p1.m_distance )
        {
            p1.m_distance = distance;
        }
    }
    m_points.PushBack( p1 );
    return p1.m_distance;
}


bool ExitSetQuotientMetrics::IsInExitSet( const Point &p ) const
{
    if ( !m_domain.IsInDomain( p ) )
    {
        return true;
    }
    for ( MyPointsList::ConstIterator it = m_points.Begin(); it != m_points.End(); ++it )
    {
        if ( it->Equals( p ) )
        {
            return false;
        }
    }
    return true;
}


double ExitSetQuotientMetrics::GetDistanceToExitSet( const Point &p ) const
{
    if ( !m_domain.IsInDomain( p ) )
    {
        return 0.0;
    }
    for ( MyPointsList::ConstIterator it = m_points.Begin(); it != m_points.End(); ++it )
    {
        if ( it->Equals( p ) )
        {
            return it->m_distance;
        }
    }
    return 0.0;
}

//////////////////////////////////////////////////////////////////////////

ExitSetQuotientMetrics::MyPoint::MyPoint( const Point &point )
    : m_point( point )
    , m_distance( std::numeric_limits<double>::max() )
{
}


bool ExitSetQuotientMetrics::MyPoint::Equals( const Point &point ) const
{
    return m_point == point;
}

////////////////////////////////////////////////////////////////////////////////

ExitSetQuotientIndexMetrics::ExitSetQuotientIndexMetrics( const PointsList &points, const ExitSetQuotientMetrics &metrics )
    : m_metrics( metrics )
{
    ResetIndexMetrics( points );
}


double ExitSetQuotientIndexMetrics::GetDistance( const Point &x, const Point &y, uint i, uint j ) const
{
    assert( i == Metrics::NO_INDEX || i < m_distanceToExitSet.GetSize() );
    assert( j == Metrics::NO_INDEX || j < m_distanceToExitSet.GetSize() );
    double d1 = i != Metrics::NO_INDEX ? m_distanceToExitSet[i] : m_metrics.GetDistanceToExitSet( x );
    double d2 = j != Metrics::NO_INDEX ? m_distanceToExitSet[j] : m_metrics.GetDistanceToExitSet( y );
    if ( d1 == 0 || d2 == 0 )
    {
        return d1 + d2;
    }
    return std::min( d1 + d2, m_metrics.GetInnerMetrics().GetDistance( x, y, i, j ) );
}


void ExitSetQuotientIndexMetrics::ResetIndexMetrics( const PointsList &points )
{
    m_distanceToExitSet.Clear();
    const uint size = points.GetSize();
    for ( uint i = 0; i < size; ++i )
    {
        m_distanceToExitSet.PushBack( m_metrics.GetDistanceToExitSet( points[i] ) );
    }
}
