/* 
 * File:   IndexMetric.hpp
 * Author: Piotr Brendel
 */

#ifndef INDEXMETRIC_HPP
#define	INDEXMETRIC_HPP

#include "Point.hpp"
#include <cassert>

class IndexMetric
{
public:

    virtual ~IndexMetric()
    {
    }

    virtual double Distance(int i, int j, const Point &x, const Point &y) = 0;
    virtual int GetSize() = 0;
};

class MaxDomainRangeIndexMetric : public IndexMetric
{
    IndexMetric *domainMetric;
    int domainDim;
    IndexMetric *rangeMetric;
    int rangeDim;

public:

    MaxDomainRangeIndexMetric(IndexMetric *domainMetric, int domainDim, IndexMetric *rangeMetric, int rangeDim)
    {
        this->domainMetric = domainMetric;
        this->domainDim = domainDim;
        this->rangeMetric = rangeMetric;
        this->rangeDim = rangeDim;
    }

    double Distance(int i, int j, const Point &x, const Point &y)
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
        double distDomain = domainMetric->Distance(i, j, vx, vy);
        vx.resize(rangeDim);
        vy.resize(rangeDim);
        index = 0;
        while (index < rangeDim)
        {
            vx[index] = x[index + domainDim];
            vy[index] = y[index + domainDim];
            index++;
        }
        return std::max(distDomain, rangeMetric->Distance(i, j, vx, vy));
    }

    int GetSize()
    {
        return domainMetric->GetSize();
    }
};

#endif	/* INDEXMETRIC_HPP */

