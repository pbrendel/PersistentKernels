/* 
 * File:   ExitSetQuotientIndexMetric.hpp
 * Author: Piotr Brendel
 */

#ifndef EXITSETQUOTIENTINDEXMETRIC_HPP
#define	EXITSETQUOTIENTINDEXMETRIC_HPP

#include "Point.hpp"
#include "IndexMetric.hpp"
#include "ExitSetQuotientMetric.hpp"

class ExitSetQuotientIndexMetric : public IndexMetric
{
    std::vector<double> distanceToExitSet;
    Metric *innerMetric;

public:

    ExitSetQuotientIndexMetric(const std::vector<Point> &points, ExitSetQuotientMetric *metric)
    {
        for (int i = 0; i < points.size(); i++)
        {
            distanceToExitSet.push_back(metric->DistanceToExitSet(points[i]));
        }
        innerMetric = metric->GetInnerMetric();
    }

    void AddPoint(double distance)
    {
        distanceToExitSet.push_back(distance);
    }

    double Distance(int i, int j, const Point &x, const Point &y)
    {
        assert(i >= 0 && i < distanceToExitSet.size());
        assert(j >= 0 && j < distanceToExitSet.size());
        double d1 = distanceToExitSet[i];
        double d2 = distanceToExitSet[j];
        if (d1 == 0 || d2 == 0)
        {
            return d1 + d2;
        }
        return std::min(d1 + d2, innerMetric->Distance(x, y));
    }

    int GetSize()
    {
        return distanceToExitSet.size();
    }
};


#endif	/* EXITSETQUOTIENTINDEXMETRIC_HPP */

