/* 
 * File:   Map.hpp
 * Author: Piotr Brendel
 */

#ifndef MAP_HPP
#define	MAP_HPP

#include "Point.hpp"
#include "Domain.hpp"
#include "Noise.hpp"

////////////////////////////////////////////////////////////////////////////////

class Map
{
protected:

    int dim;
    Noise *noise;

public:

    Map(int dim, Noise *noise = 0)
    {
        this->dim = dim;
        this->noise = noise;
    }

    virtual ~Map()
    {
    }

    int GetDimension()
    {
        return dim;
    }

    virtual void GetValue(const Point &d, Point &r) = 0;
    
};

////////////////////////////////////////////////////////////////////////////////

class WriteValues
{
    Domain &domain;
    Map &map;

public:

    WriteValues(Domain &d, Map &m) : domain(d), map(m)
    {
    }

    friend std::ostream &operator<<(std::ostream &str, const WriteValues &wm);
};

std::ostream &operator<<(std::ostream &str, const WriteValues &wv)
{
    Point d(wv.domain.GetDimension());
    Point r(wv.map.GetDimension());
    int count = wv.domain.GetCount();
    for (int i = 0; i < count; i++)
    {
        wv.domain.GetValue(i, d);
        wv.map.GetValue(d, r);
        for (Point::iterator v = r.begin(); v != r.end(); v++)
        {
            str<<*v<<" ";
        }
        str<<std::endl;
    }
    return str;
}

class WriteMap
{
    Domain &domain;
    Map &map;

public:

    WriteMap(Domain &d, Map &m) : domain(d), map(m)
    {
    }

    friend std::ostream &operator<<(std::ostream &str, const WriteMap &wm);
};

std::ostream &operator<<(std::ostream &str, const WriteMap &wm)
{
    Point d(wm.domain.GetDimension());
    Point r(wm.map.GetDimension());
    int count = wm.domain.GetCount();
    for (int i = 0; i < count; i++)
    {
        wm.domain.GetValue(i, d);
        for (Point::iterator v = d.begin(); v != d.end(); v++)
        {
            str<<*v<<" ";
        }
        wm.map.GetValue(d, r);
        for (Point::iterator v = r.begin(); v != r.end(); v++)
        {
            str<<*v<<" ";
        }
        str<<std::endl;
    }
    return str;
}

////////////////////////////////////////////////////////////////////////////////

class LinearMap : public Map
{
    std::vector<double> factors;

public:

    LinearMap(int dim, const std::vector<double> &factors, Noise *noise = 0) : Map(dim, noise)
    {
        assert(factors.size() == dim);
        this->factors.assign(factors.begin(), factors.end());
    }

    void GetValue(const Point &d, Point &r)
    {
        assert(d.size() == dim);
        assert(r.size() == dim);
        for (int i = 0; i < dim; i++)
        {
            r[i] = d[i] * factors[i];
        }
        if (this->noise != 0)
        {
            this->noise->AddNoise(r);
        }
    }
};

class LinearDiscMap : public Map
{
    std::vector<double> factors;
    std::vector<Interval> intervals;

public:

    LinearDiscMap(int dim, const std::vector<double> &factors, Domain *domain, Noise *noise = 0) : Map(dim, noise)
    {
        assert(factors.size() == dim);
        this->factors.assign(factors.begin(), factors.end());
        for (int i = 0; i < dim; i++)
        {
            Interval interval = domain->GetInterval(i);
            interval.min = interval.GetCenter();
            intervals.push_back(interval);
        }
    }

    void GetValue(const Point &d, Point &r)
    {
        assert(d.size() == dim);
        assert(r.size() == dim);
        for (int i = 0; i < dim; i++)
        {
            if (d[i] < intervals[i].min)
            {
                r[i] = d[i] * factors[i];
            }
            else
            {
                r[i] = (intervals[i].max - (d[i] - intervals[i].min)) * factors[i];
            }
        }
        if (this->noise != 0)
        {
            this->noise->AddNoise(r);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

class TestDisc2To1 : public Map
{
    double centerX;
    double centerY;
    double factor;

public:

    TestDisc2To1(Domain *domain, double factor = 1.0, Noise *noise = 0) : Map(1, noise)
    {
        assert(domain->GetDimension() == 2);
        Cube cube = domain->GetCube();
        centerX = cube[0].GetCenter();
        centerY = cube[1].GetCenter();
        this->factor = factor;
    }

    void GetValue(const Point &d, Point &r)
    {
        assert(d.size() == 2);
        assert(r.size() == 1);
        if (d[0] < centerX)
        {
            r[0] = 0;
        }
        else
        {
            r[0] = (d[1] > centerY) ? factor * d[0] : -factor * d[0];
        }
        if (this->noise != 0)
        {
            this->noise->AddNoise(r);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

class Translation : public Map
{
    Point offset;

public:

    Translation(int dim, Point offset, Noise *noise = 0) : Map(dim, noise)
    {
        assert(offset.size() == dim);
        this->offset.assign(offset.begin(), offset.end());
    }

    void GetValue(const Point &d, Point &r)
    {
        assert(d.size() == dim);
        assert(r.size() == dim);
        for (int i = 0; i < dim; i++)
        {
            r[i] = d[i] + offset[i];
        }
        if (this->noise != 0)
        {
            this->noise->AddNoise(r);
        }
    }
};

#endif	/* MAP_HPP */

