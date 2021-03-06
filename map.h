// pbrendel (c) 2013-21

#pragma once

#include "interval.h"
#include "point.h"

class Domain;
class Noise;


class Map
{
public:

    Map( uint dim, const Noise *noise )
        : m_dim( dim )
        , m_noise( noise )
    {}

    virtual ~Map()
    {}

    constexpr uint GetDimension() const
    {
        return m_dim;
    }

    virtual void GetValue( const Point &d, Point &r ) const = 0;

protected:

    uint m_dim;
    const Noise *m_noise;
};

////////////////////////////////////////////////////////////////////////////////

class LinearMap : public Map
{
public:

    LinearMap( uint dim, const o::DynArray<double> &factors, const Noise *noise );

    virtual void GetValue( const Point &d, Point &r ) const override;

private:

    o::DynArray<double> m_factors;
};

////////////////////////////////////////////////////////////////////////////////

class LinearDiscMap : public Map
{
public:

    LinearDiscMap( uint dim, const o::DynArray<double> &factors, const Domain &domain, const Noise *noise );

    virtual void GetValue( const Point &d, Point &r ) const override;

private:

    o::DynArray<double> m_factors;
    o::DynArray<Interval> m_intervals;

};

////////////////////////////////////////////////////////////////////////////////

class TestDisc2To1 : public Map
{
public:

    TestDisc2To1( const Domain &domain, double factor, const Noise *noise );

    virtual void GetValue( const Point &d, Point &r ) const override;

private:

    double m_centerX;
    double m_centerY;
    double m_factor;
};

////////////////////////////////////////////////////////////////////////////////

class Translation : public Map
{
public:

    Translation( uint dim, Point offset, const Noise *noise );

    virtual void GetValue( const Point &d, Point &r ) const override;

private:

    Point m_offset;
};
