/*
 * File:   Domain.hpp
 * Author: Piotr Brendel
 */

#ifndef METRIC_HPP
#define	METRIC_HPP

#include <vector>
#include <cmath>
#include <cassert>
#include "Point.hpp"

class Metric
{   
public:

    virtual ~Metric()
    {
    }
    
    virtual double Distance(const Point &x, const Point &y) = 0;
};

class EuclideanMetric : public Metric
{
public:

    double Distance(const Point &x, const Point &y)
    {
        assert(x.size() == y.size());
        double d = 0;
        double tmp;
        for (Point::const_iterator i = x.begin(), j = y.begin(); i != x.end(); i++, j++)
        {
            tmp = *i - *j;
            d += tmp * tmp;
        }
        return sqrt(d);
    }
};

class MaxMetric : public Metric
{
public:

    double Distance(const Point &x, const Point &y)
    {
        assert(x.size() == y.size());
        double d = 0;
        for (Point::const_iterator i = x.begin(), j = y.begin(); i != x.end(); i++, j++)
        {
            d = std::max(d, *i - *j);
        }
        return d;
    }
};

class TaxiMetric : public Metric
{
public:

    double Distance(const Point &x, const Point &y)
    {
        assert(x.size() == y.size());
        double d = 0;
        for (Point::const_iterator i = x.begin(), j = y.begin(); i != x.end(); i++, j++)
        {
            d += abs(*i - *j);
        }
        return d;
    }
};

class MaxDomainRangeMetric : public Metric
{
    Metric *domainMetric;
    int domainDim;
    Metric *rangeMetric;
    int rangeDim;

public:

    MaxDomainRangeMetric(Metric *domainMetric, int domainDim, Metric *rangeMetric, int rangeDim)
    {
        this->domainMetric = domainMetric;
        this->domainDim = domainDim;
        this->rangeMetric = rangeMetric;
        this->rangeDim = rangeDim;
    }

    double Distance(const Point &x, const Point &y)
    {
        assert(x.size() == y.size());
        assert(x.size() == (domainDim + rangeDim));

        Point vx(domainDim);
        Point vy(domainDim);
        int index = 0;
        while (index < domainDim)
        {
            vx[index] = x[index];
            vy[index] = y[index];
            index++;
        }
        double distDomain = domainMetric->Distance(vx, vy);
        vx.resize(rangeDim);
        vy.resize(rangeDim);
        index = 0;
        while (index < rangeDim)
        {
            vx[index] = x[index + domainDim];
            vy[index] = y[index + domainDim];
            index++;
        }
        return std::max(distDomain, rangeMetric->Distance(vx, vy));
    }
};

#endif	/* METRIC_HPP */

