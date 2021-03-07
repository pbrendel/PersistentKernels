// pbrendel (c) 2013-21

#pragma once

#include "metrics.h"

class Domain;
class Map;


class ExitSetQuotientMetrics : public Metrics
{
public:

    ExitSetQuotientMetrics( const Domain &domain, const Map &map, const Metrics& innerMetrics );

    virtual double GetDistance( const Point &x, const Point &y, uint, uint ) const override;
    virtual bool HasIndexMetrics() const override { return true; }
    virtual o::Ptr<IndexMetrics> CreateIndexMetrics( const PointsList &points ) const override;

    double GetDistanceToExitSet( const Point &p ) const;

    const Metrics &GetInnerMetrics() const
    {
        return m_innerMetrics;
    }

private:

    struct MyPoint
    {
        Point m_point;
        double m_distance;

        MyPoint( const Point &point );

        bool Equals( const Point &point ) const;
    };

    typedef o::DynArray<MyPoint> MyPointsList;

    double AddPoint( const Point &p, const Map &map );
    bool IsInExitSet( const Point &p ) const;

    MyPointsList m_points;
    PointsList m_exitSetPoints;
    const Domain &m_domain;
    const Metrics &m_innerMetrics;
};

////////////////////////////////////////////////////////////////////////////////

class ExitSetQuotientIndexMetrics : public IndexMetrics
{
public:

    ExitSetQuotientIndexMetrics( const PointsList &points, const ExitSetQuotientMetrics &metrics );

    virtual double GetDistance( const Point &x, const Point &y, uint i, uint j ) const override;
    virtual void ResetIndexMetrics( const PointsList &points ) override;

    void AddPoint( double distance )
    {
        m_distanceToExitSet.PushBack( distance );
    }

    uint GetSize() const
    {
        return m_distanceToExitSet.GetSize();
    }

private:

    o::DynArray<double> m_distanceToExitSet;
    const ExitSetQuotientMetrics &m_metrics;
};
