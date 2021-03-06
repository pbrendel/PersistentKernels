// pbrendel (c) 2013-21

#include "metrics.h"


double EuclideanMetrics::GetDistance( const Point &x, const Point &y, uint, uint ) const
{
    assert( x.GetDimension() == y.GetDimension() );
    double d = 0;
    double tmp;
    for ( Point::ConstIterator i = x.Begin(), j = y.Begin(); i != x.End(); ++i, ++j )
    {
        tmp = *i - *j;
        d += tmp * tmp;
    }
    return sqrt( d );
}


const EuclideanMetrics &EuclideanMetrics::Get()
{
    static EuclideanMetrics s_instance;
    return s_instance;
}


////////////////////////////////////////////////////////////////////////////////

double MaxMetrics::GetDistance( const Point &x, const Point &y, uint, uint ) const
{
    assert( x.GetDimension() == y.GetDimension() );
    double d = 0;
    for ( Point::ConstIterator i = x.Begin(), j = y.Begin(); i != x.End(); ++i, ++j )
    {
        d = std::max( d, *i - *j );
    }
    return d;
}


const MaxMetrics &MaxMetrics::Get()
{
    static MaxMetrics s_instance;
    return s_instance;
}

////////////////////////////////////////////////////////////////////////////////

double TaxiMetrics::GetDistance( const Point &x, const Point &y, uint, uint ) const
{
    assert( x.GetDimension() == y.GetDimension() );
    double d = 0;
    for ( Point::ConstIterator i = x.Begin(), j = y.Begin(); i != x.End(); ++i, ++j )
    {
        d += abs( *i - *j );
    }
    return d;
}


const TaxiMetrics &TaxiMetrics::Get()
{
    static TaxiMetrics s_instance;
    return s_instance;
}

////////////////////////////////////////////////////////////////////////////////

double MaxDomainRangeMetrics::GetDistance( const Point &x, const Point &y, uint i, uint j ) const
{
    assert( x.GetDimension() == y.GetDimension() );
    assert( x.GetDimension() == ( m_domainDim + m_rangeDim ) );

    Point vx( m_domainDim );
    Point vy( m_domainDim );
    uint index = 0;
    while ( index < m_domainDim )
    {
        vx[index] = x[index];
        vy[index] = y[index];
        index++;
    }
    const double distDomain = m_domainMetrics.GetDistance( vx, vy, i, j );
    vx.Resize( m_rangeDim );
    vy.Resize( m_rangeDim );
    index = 0;
    while ( index < m_rangeDim )
    {
        vx[index] = x[index + m_domainDim];
        vy[index] = y[index + m_domainDim];
        index++;
    }
    return std::max( distDomain, m_rangeMetrics.GetDistance( vx, vy, i, j ) );
}
