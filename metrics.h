// pbrendel (c) 2013-21

#pragma once

#include "point.h"
#include "Core/types.h"
#include "Core/ptr.h"


class IndexMetrics;


class Metrics
{   
public:

    enum : uint
    {
        NO_INDEX = static_cast<uint>( -1 ),
    };

    virtual ~Metrics()
    {}
    
    virtual double GetDistance( const Point &x, const Point &y, uint i, uint j ) const = 0;

    virtual bool IsIndexMetrics() const { return false; }
    virtual bool HasIndexMetrics() const { return false; }
    virtual o::Ptr<IndexMetrics> CreateIndexMetrics( const PointsList &points ) const { return nullptr; }
    virtual void ResetIndexMetrics( const PointsList &points ) { assertex( false, "Not implemented" ); }
};

////////////////////////////////////////////////////////////////////////////////

class IndexMetrics : public Metrics
{
public:

    virtual bool IsIndexMetrics() const override { return true; }
};

////////////////////////////////////////////////////////////////////////////////

class EuclideanMetrics : public Metrics
{
public:

    virtual double GetDistance( const Point &x, const Point &y, uint, uint ) const override;

    static const EuclideanMetrics &Get();
};

////////////////////////////////////////////////////////////////////////////////

class MaxMetrics : public Metrics
{
public:

    virtual double GetDistance( const Point &x, const Point &y, uint, uint ) const override;

    static const MaxMetrics &Get();
};

////////////////////////////////////////////////////////////////////////////////

class TaxiMetrics : public Metrics
{
public:

    virtual double GetDistance( const Point &x, const Point &y, uint, uint ) const override;

    static const TaxiMetrics &Get();
};

////////////////////////////////////////////////////////////////////////////////

class MaxDomainRangeMetrics : public Metrics
{
public:

    MaxDomainRangeMetrics( const Metrics &domainMetrics, const Metrics &rangeMetrics, uint domainDim, uint rangeDim )
        : m_domainMetrics( domainMetrics )
        , m_rangeMetrics( rangeMetrics )
        , m_domainDim( domainDim )
        , m_rangeDim( rangeDim )
    {}

    virtual double GetDistance( const Point &x, const Point &y, uint i, uint j ) const override;

private:

    const Metrics &m_domainMetrics;
    const Metrics &m_rangeMetrics;
    uint m_domainDim;
    uint m_rangeDim;
};
