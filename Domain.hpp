/*
 * File:   Domain.hpp
 * Author: Piotr Brendel
 */

#ifndef DOMAIN_HPP
#define	DOMAIN_HPP

#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>

#include "Interval.hpp"
#include "Cube.hpp"
#include "Noise.hpp"
#include "Metric.hpp"

class Domain
{
protected:

    int dim;
    Cube cube;
    Noise *noise;
    int count;

public:

    Domain(int dim, const Cube &cube, Noise *noise = 0)
    {
        assert(cube.size() == dim);
        this->dim = dim;
        this->cube.assign(cube.begin(), cube.end());
        this->noise = noise;
        this->count = 0;
    }

    Domain(Domain *domain)
    {
        this->dim = domain->dim;
        this->cube.assign(domain->cube.begin(), domain->cube.end());
        this->count = domain->count;
        this->noise = domain->noise;
    }

    virtual ~Domain()
    {
    }

    virtual int GetCount()
    {
        return count;
    }

    int GetDimension()
    {
        return dim;
    }

    Cube GetCube()
    {
        return cube;
    }

    Interval GetInterval(int dim)
    {
        assert(dim >= 0 && dim < this->dim);
        return cube[dim];
    }

    virtual void GetValue(int index, Point &p) = 0;

    virtual bool IsInDomain(const Point &p)
    {
        if (p.size() != dim)
        {
            return false;
        }
        for (int i = 0; i < dim; i++)
        {
            if (p[i] < cube[i].min || p[i] > cube[i].max)
            {
                return false;
            }
        }
        return true;
    }

    friend std::ostream &operator<<(std::ostream &str, Domain &d);
};

std::ostream &operator<<(std::ostream &str, Domain &d)
{
    Point p(d.dim);
    int count = d.GetCount();
    for (int i = 0; i < count; i++)
    {
        d.GetValue(i, p);
        for (Point::iterator v = p.begin(); v != p.end(); v++)
        {
            str<<*v<<" ";
        }
        str<<std::endl;
    }
    return str;
}

class UniformCube : public Domain
{
protected:

    std::vector<int> resolution;

public:

    UniformCube(int dim, const Cube &cube, const std::vector<int> &resolution, Noise *noise = 0) : Domain(dim, cube, noise)
    {
        assert (resolution.size() == dim);
        this->resolution.assign(resolution.begin(), resolution.end());
        this->count = resolution[0];
        for (int i = 1; i < resolution.size(); i++)
        {
            this->count *= resolution[i];
        }
    }

    int GetCount()
    {
        return this->count;
    }

    void GetValue(int index, Point &p)
    {
        assert(p.size() == dim);
        int d = 0;
        while (d < dim)
        {
            p[d] = cube[d].min + cube[d].GetLength() * (double)(index % this->resolution[d]) / std::max(1, this->resolution[d] - 1);
            index = index / this->resolution[d];
            d++;
        }
        if (this->noise != 0)
        {
            this->noise->AddNoise(p);
        }
    }
};

class UniformCubeWithHole : public UniformCube
{
    Cube hole;

public:

    UniformCubeWithHole(int dim, const Cube &cube, const Cube &hole, const std::vector<int> &resolution, Noise *noise = 0) : UniformCube(dim, cube, resolution, noise)
    {
        this->hole.assign(hole.begin(), hole.end());
    }

    void GetValue(int index, Point &p)
    {
        assert(p.size() == dim);
        const int maxSteps = 100;
        int step = 0;
        do
        {
            int d = 0;
            while (d < dim)
            {
                p[d] = cube[d].min + cube[d].GetLength() * (double)(index % this->resolution[d]) / std::max(1, this->resolution[d] - 1);
                index = index / this->resolution[d];
                d++;
            }
        } while (hole.IsInside(p) && step++ < maxSteps);
        if (this->noise != 0)
        {
            this->noise->AddNoise(p);
        }
    }

    virtual bool IsInDomain(const Point &p)
    {
        if (!Domain::IsInDomain(p))
        {
            return false;
        }
        return !hole.IsInside(p);
    }
};

class RandomCube : public Domain
{
protected:

    double *randoms;

public:

    RandomCube(int dim, const Cube &cube, int count, Noise *noise = 0) : Domain(dim, cube, noise)
    {
        this->count = count;
        randoms = new double[this->count * dim];
        for (int i = 0; i < this->count * dim; i++)
        {
            randoms[i] = (double)(rand() % RAND_MAX) / (RAND_MAX - 1);
        }
    }

    ~RandomCube()
    {
        delete[] randoms;
    }

    void GetValue(int index, Point &p)
    {
        assert(p.size() == dim);
        int d = 0;
        while (d < dim)
        {
            p[d] = cube[d].min + cube[d].GetLength() * randoms[index * dim + d];
            d++;
        }
        if (this->noise != 0)
        {
            this->noise->AddNoise(p);
        }
    }
};

class RandomCubeWithHole : public RandomCube
{
    Cube hole;

public:

    RandomCubeWithHole(int dim, const Cube &cube, const Cube &hole, int count, Noise *noise = 0) : RandomCube(dim, cube, count, noise)
    {
        this->hole.assign(hole.begin(), hole.end());
    }

    void GetValue(int index, Point &p)
    {
        assert(p.size() == dim);
        const int maxSteps = 100;
        int step = 0;
        do
        {
            int d = 0;
            while (d < dim)
            {
                p[d] = cube[d].min + cube[d].GetLength() * randoms[index * dim + d];
                d++;
            }
        } while (hole.IsInside(p) && step++ < maxSteps);
        if (this->noise != 0)
        {
            this->noise->AddNoise(p);
        }
    }

    virtual bool IsInDomain(const Point &p)
    {
        if (!Domain::IsInDomain(p))
        {
            return false;
        }
        return !hole.IsInside(p);
    }
};

class DomainRestriction : public Domain
{
    std::vector<Point> points;

public:

    DomainRestriction(Domain *domain) : Domain(domain)
    {
        this->count = 0;
    }

    DomainRestriction(Domain *domain, Metric *metric, Point &center, double radius) : Domain(domain)
    {
        Create(domain, metric, center, radius);
    }

    DomainRestriction(Domain *domain, IndexMetric *metric, Point &center, double radius) : Domain(domain)
    {
        Create(domain, metric, center, radius);
    }

    void Create(Domain *domain, Metric *metric, Point &center, double radius)
    {
        assert(center.size() == dim);
        points.clear();
        Point p(dim);
        int count = domain->GetCount();
        for (int i = 0; i < count; i++)
        {
            domain->GetValue(i, p);
            if (metric->Distance(p, center) <= radius)
            {
                points.push_back(p);
            }
        }
        this->count = points.size();
    }

   void Create(Domain *domain, IndexMetric *metric, Point &center, double radius)
    {
        assert(center.size() == dim);
        points.clear();
        Point p(dim);
        int count = domain->GetCount();
        for (int i = 0; i < count; i++)
        {
            domain->GetValue(i, p);
            if (metric->Distance(i, metric->GetSize() - 1, p, center) <= radius)
            {
                points.push_back(p);
            }
        }
        this->count = points.size();
    }

    void GetValue(int index, Point &p)
    {
        assert(p.size() == dim);
        assert(index < this->points.size());
        for (int i = 0; i < dim; i++)
        {
            p[i] = this->points[index][i];
        }
        if (this->noise != 0)
        {
            this->noise->AddNoise(p);
        }
    }
};

#endif	/* DOMAIN_HPP */

