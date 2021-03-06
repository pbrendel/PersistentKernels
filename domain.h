// pbrendel (c) 2013-21

#pragma once

#include "cube.h"
#include "point.h"

class Noise;
class Metrics;


class Domain
{
public:

    Domain( const Cube &cube, Noise *noise )
        : m_count( 0 )
        , m_cube( cube )
        , m_noise( noise )
    {}

    Domain( const Domain &other )
        : m_count( other.m_count )
        , m_cube( other.m_cube )
        , m_noise( other.m_noise )
    {}

    virtual ~Domain()
    {}

    constexpr uint GetCount() const
    {
        return m_count;
    }

    uint GetDimension() const
    {
        return m_cube.GetDimension();
    }

    const Cube &GetCube() const
    {
        return m_cube;
    }

    const Interval &GetInterval( uint dim ) const
    {
        return m_cube[dim];
    }

    virtual bool IsInDomain( const Point &p ) const
    {
        return m_cube.IsInside( p );
    }

    virtual void GetValue( uint index, Point &p ) const = 0;

protected:

    uint m_count;
    Cube m_cube;
    Noise *m_noise;
};

////////////////////////////////////////////////////////////////////////////////

class UniformCube : public Domain
{
public:

    UniformCube( const Cube &cube, const o::DynArray<uint> &resolution, Noise *noise );

    virtual void GetValue( uint index, Point &p ) const override;

protected:

    o::DynArray<uint> m_resolution;
};

////////////////////////////////////////////////////////////////////////////////

class UniformCubeWithHole : public UniformCube
{
public:

    UniformCubeWithHole( const Cube &cube, const Cube &hole, const o::DynArray<uint> &resolution, Noise *noise )
        : UniformCube( cube, resolution, noise )
        , m_hole( hole )
    {}

    virtual bool IsInDomain( const Point &p ) const override;
    virtual void GetValue( uint index, Point &p ) const override;

private:

    Cube m_hole;
};

////////////////////////////////////////////////////////////////////////////////

class RandomCube : public Domain
{
public:

    RandomCube( const Cube &cube, uint count, Noise *noise );

    virtual void GetValue( uint index, Point &p ) const override;

protected:

    o::DynArray<double> m_randoms;
};

////////////////////////////////////////////////////////////////////////////////

class RandomCubeWithHole : public RandomCube
{
public:

    RandomCubeWithHole( const Cube &cube, const Cube &hole, uint count, Noise *noise )
        : RandomCube( cube, count, noise )
        , m_hole( hole )
    {}

    virtual bool IsInDomain( const Point &p ) const override;
    virtual void GetValue( uint index, Point &p ) const override;

private:

    Cube m_hole;
};

////////////////////////////////////////////////////////////////////////////////

class DomainRestriction : public Domain
{
public:

    DomainRestriction( const Domain &other, const Metrics &metrics, const Point &center, double radius )
        : Domain( other )
    {
        Create( other, metrics, center, radius );
    }

    virtual void GetValue( uint index, Point &p ) const override;

private:

    void Create( const Domain &other, const Metrics &metrics, const Point &center, double radius );

    PointsList m_points;
};
