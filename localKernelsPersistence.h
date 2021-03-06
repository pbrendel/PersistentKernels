// pbrendel (c) 2013-21

#pragma once

#include "persistenceData.h"
#include "Core/ptr.h"

class Domain;
class Map;
class Metrics;


class LocalKernelsPersistence
{
public:

    static void Compute_Alg1( const Domain &domain, const Map &map, const Domain &testDomain, const o::DynArray<double> &epsilons, double restrictionRadius,
                              const Metrics &domainMetrics, const Metrics &graphMetrics, PersistenceData &outPersistenceData );
    static void Compute_Alg2( const Domain &domain, const Map &map, double alpha, double beta, const Metrics &domainMetrics, PersistenceData &outPersistenceData );



private:

    struct PointsProxy
    {
        PointsList m_domainPoints;
        PointsList m_rangePoints;
        PointsList m_graphPoints;
    };

    enum PointsCreateFlags : uint
    {
        PCF_Domain  = ( 1 << 0 ),
        PCF_Range   = ( 1 << 1 ),
        PCF_Graph   = ( 1 << 2 ),
        PCF_All     = PCF_Domain | PCF_Range | PCF_Graph,
    };

    enum : uint
    {
        MAX_RIPS_DIMENSION = 1,
    };

    static void CreatePoints( const Domain &domain, const Map &map, uint pcfFlags, PointsProxy &outPoints );
    static void FindEpsilons( const PointsList &graphPoints, const Metrics &metrics, double &prevEpsilon, double &epsilon, double alpha );
};