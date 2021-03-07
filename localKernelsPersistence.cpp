// pbrendel (c) 2013-21

#include "localKernelsPersistence.h"
#include "domain.h"
#include "map.h"
#include "metrics.h"
#include "ripsComplex.h"
#include "Core/dynArray.h"

using o::DynArray;


void LocalKernelsPersistence::CreatePoints( const Domain &domain, const Map &map, uint pcfFlags, PointsProxy &outPoints )
{
    PointsList &domainPoints = outPoints.m_domainPoints;
    PointsList &rangePoints = outPoints.m_rangePoints;
    PointsList &graphPoints = outPoints.m_graphPoints;
    domainPoints.Clear();
    rangePoints.Clear();
    graphPoints.Clear();
    const bool createDomain = pcfFlags & PCF_Domain;
    const bool createRange = pcfFlags & PCF_Range;
    const bool createGraph = pcfFlags & PCF_Graph;
    const uint domainDim = domain.GetDimension();
    const uint rangeDim = map.GetDimension();
    Point d( domainDim );
    Point r( rangeDim );
    Point g( domainDim + rangeDim );
    const uint count = domain.GetCount();
    for ( uint i = 0; i < count; ++i )
    {
        domain.GetValue( i, d );
        map.GetValue( d, r );
        if ( createDomain )
        {
            domainPoints.PushBack( d );
        }
        if ( createRange )
        {
            rangePoints.PushBack( r );
        }
        if ( createGraph )
        {
            for ( uint j = 0; j < domainDim; ++j )
            {
                g[j] = d[j];
            }
            for ( uint j = 0; j < rangeDim; ++j )
            {
                g[domainDim + j] = r[j];
            }
            graphPoints.PushBack( g );
        }
    }
}


void LocalKernelsPersistence::FindEpsilons( const PointsList &graphPoints, const Metrics &metrics, double &prevEpsilon, double &epsilon, double alpha )
{
    RipsComplex ripsGraph( graphPoints, metrics, epsilon, 1 );
    ripsGraph.CreateConnectedComponents();
    while ( ripsGraph.GetConnectedComponentsNumber() > 1 )
    {
        prevEpsilon = epsilon;
        epsilon *= 2;
        ripsGraph.Create( graphPoints, metrics, epsilon, 1 );
        ripsGraph.CreateConnectedComponents();
    }
    ripsGraph.Create( graphPoints, metrics, prevEpsilon, 1 );
    ripsGraph.CreateConnectedComponents();
    while ( ripsGraph.GetConnectedComponentsNumber() == 1 )
    {
        prevEpsilon *= 0.5;
        ripsGraph.Create( graphPoints, metrics, prevEpsilon, 1 );
        ripsGraph.CreateConnectedComponents();
    }
    while ( ( epsilon - prevEpsilon ) > alpha )
    {
        const double newEpsilon = ( epsilon + prevEpsilon ) * 0.5;
        ripsGraph.Create( graphPoints, metrics, newEpsilon, 1 );
        ripsGraph.CreateConnectedComponents();
        if ( ripsGraph.GetConnectedComponentsNumber() == 1 )
        {
            epsilon = newEpsilon;
        }
        else
        {
            prevEpsilon = newEpsilon;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void LocalKernelsPersistence::Compute_Alg1( const Domain &domain, const Map &map, const Domain &testDomain, const DynArray<double> &epsilons, double restrictionRadius,
                                            const Metrics &domainMetrics, const Metrics &graphMetrics, PersistenceData &outPersistenceData )
{
    Point center( domain.GetDimension() );
    const uint count = testDomain.GetCount();
    for ( uint i = 0; i < count; ++i )
    {
        testDomain.GetValue( i, center );
        DomainRestriction restriction( domain, domainMetrics, center, restrictionRadius );
        if ( restriction.GetCount() == 0 )
        {
            continue;
        }
        PointsProxy points;
        CreatePoints( domain, map, PCF_Domain | PCF_Graph, points );
        PointsList &domainPoints = points.m_domainPoints;
        PointsList &graphPoints = points.m_graphPoints;
        ProjectionsList projections;
        const uint epsilonsCount = epsilons.GetSize();
        for ( uint j = 0; j < epsilonsCount; ++j )
        {
            RipsComplex ripsComplexDomain( domainPoints, domainMetrics, epsilons[j], true );
            ripsComplexDomain.CreateConnectedComponents();
            const uint domainConnectedComponents = ripsComplexDomain.GetConnectedComponentsNumber();

            RipsComplex ripsComplexGraph( graphPoints, graphMetrics, epsilons[j], true );
            ripsComplexGraph.CreateConnectedComponents();
            const uint graphConnectedComponents = ripsComplexGraph.GetConnectedComponentsNumber();

            assert( graphConnectedComponents >= domainConnectedComponents );

            Projection projection;
            ripsComplexGraph.GetProjectionMap( ripsComplexDomain, projection );
            projections.PushBack( projection );
            if ( graphConnectedComponents == 1 )
            {
                break;
            }
        }
        outPersistenceData.PushBack( PointPersistenceData( center, projections, false ) );
    }
}

////////////////////////////////////////////////////////////////////////////////

class MetricsProxy
{
public:

    MetricsProxy( const Metrics &domainMetrics, uint domainDim, uint rangeDim, const LocalKernelsPersistence::PointsProxy &points )
        : m_domainMetrics( domainMetrics )
    {
        if ( m_domainMetrics.HasIndexMetrics() )
        {
            m_domainIndexMetrics = m_domainMetrics.CreateIndexMetrics( points.m_domainPoints );
            m_rangeIndexMetrics = m_domainMetrics.CreateIndexMetrics( points.m_rangePoints );
            m_graphMetrics = new MaxDomainRangeMetrics( *m_domainIndexMetrics, *m_rangeIndexMetrics, domainDim, rangeDim );
        }
        else
        {
            m_graphMetrics = new MaxDomainRangeMetrics( domainMetrics, domainMetrics, domainDim, rangeDim );
        }
    }

    const Metrics &GetDomainMetrics() const { return m_domainIndexMetrics ? *m_domainIndexMetrics : m_domainMetrics; }
    const Metrics &GetGraphMetrics() const { return *m_graphMetrics; }

private:

    const Metrics &m_domainMetrics;
    o::Ptr<Metrics> m_graphMetrics;
    o::Ptr<IndexMetrics> m_domainIndexMetrics;
    o::Ptr<IndexMetrics> m_rangeIndexMetrics;
};


void LocalKernelsPersistence::Compute_Alg2( const Domain &domain, const Map &map, double alpha, double beta, const Metrics &domainMetrics, PersistenceData &outPersistenceData )
{
    if ( domain.GetCount() < 2 )
    {
        return;
    }
    const uint domainDim = domain.GetDimension();
    const uint rangeDim = map.GetDimension();

    double epsilon = 0.1;
    double prevEpsilon = epsilon * 0.5;
    PointsProxy points;
    CreatePoints( domain, map, PCF_All, points );
    MetricsProxy mainMetrics( domainMetrics, domainDim, rangeDim, points );
    FindEpsilons( points.m_graphPoints, mainMetrics.GetGraphMetrics(), prevEpsilon, epsilon, alpha );
    epsilon = ( 1.0 + beta ) * epsilon;

    Point center( domain.GetDimension() );
    const uint count = domain.GetCount();
    for ( uint i = 0; i < count; i++ )
    {
        domain.GetValue( i, center );
        DomainRestriction restriction( domain, mainMetrics.GetDomainMetrics(), center, epsilon );
        if ( restriction.GetCount() == 0 )
        {
            continue;
        }
        
        PointsProxy points;
        CreatePoints( restriction, map, PCF_All, points );
        MetricsProxy localMetrics( domainMetrics, domainDim, rangeDim, points );
        
        RipsComplex ripsComplexDomain( points.m_domainPoints, localMetrics.GetDomainMetrics(), epsilon, false );
        ripsComplexDomain.CreateConnectedComponents();        
        const uint domainConnectedComponents = ripsComplexDomain.GetConnectedComponentsNumber();
        
        RipsComplex ripsComplexGraph( points.m_graphPoints, localMetrics.GetGraphMetrics(), epsilon, false );
        ripsComplexGraph.CreateConnectedComponents();
        const uint graphConnectedComponents = ripsComplexGraph.GetConnectedComponentsNumber();
        assert( graphConnectedComponents >= domainConnectedComponents );
        
        Projection projection;
        ripsComplexGraph.GetProjectionMap( ripsComplexDomain, projection );
        ProjectionsList projections;
        projections.PushBack( projection );
        outPersistenceData.PushBack( PointPersistenceData( center, projections, false ) );
    }
}
