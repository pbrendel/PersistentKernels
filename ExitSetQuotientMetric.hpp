/* 
 * File:   ExitSetQuotientMetric.hpp
 * Author: Piotr Brendel
 */

#ifndef EXITSETQUOTIENTMETRIC_HPP
#define	EXITSETQUOTIENTMETRIC_HPP

#include <limits>
#include "Point.hpp"
#include "Domain.hpp"
#include "Map.hpp"
#include "Metric.hpp"

class ExitSetQuotientMetric : public Metric
{
    struct MyPoint
    {
        Point p;
        double distance;

        MyPoint(const Point &p1) : p(p1.begin(), p1.end())
        {
            distance = std::numeric_limits<double>::max();
        }

        bool Equals(const Point &p1)
        {
            if (p.size() != p1.size())
            {
                return false;
            }
            Point::const_iterator it1 = p1.begin();
            for (Point::iterator it = p.begin(); it != p.end(); it++, it1++)
            {
                if (*it != *it1)
                {
                    return false;
                }
            }
            return true;
        }
    };

    std::vector<MyPoint> points;
    std::vector<Point> exitSetPoints;
    Domain *domain;
    Metric *innerMetric;

public:

    ExitSetQuotientMetric(Domain *domain, Map *map)
    {
        this->domain = domain;
        this->innerMetric = new EuclideanMetric();
        int size = domain->GetCount();
        if (size == 0)
        {
            return;
        }
        Point p(domain->GetDimension()), v(domain->GetDimension()), v1(domain->GetDimension());
        for (int i = 0; i < size; i++)
        {
            domain->GetValue(i, p);
            map->GetValue(p, v);
            if (domain->IsInDomain(v))
            {
                points.push_back(MyPoint(p));
                map->GetValue(v, v1);
                if (domain->IsInDomain(v1))
                {
                    points.push_back(MyPoint(v));
                }
                else
                {
                    exitSetPoints.push_back(v);
                }
            }
            else
            {
                exitSetPoints.push_back(p);
            }
        }
        for (std::vector<MyPoint>::iterator it = points.begin(); it != points.end(); it++)
        {
            for (std::vector<Point>::iterator it1 = exitSetPoints.begin(); it1 != exitSetPoints.end(); it1++)
            {
                double distance = innerMetric->Distance(it->p, *it1);
                if (distance < it->distance)
                {
                    it->distance = distance;
                }
            }
        }
    }

    ~ExitSetQuotientMetric()
    {
        delete innerMetric;
    }

    double AddPoint(const Point &p, Map *map)
    {
        if (!domain->IsInDomain(p))
        {
            return 0;
        }
        Point v(domain->GetDimension());
        map->GetValue(p, v);
        if (!domain->IsInDomain(v))
        {
            exitSetPoints.push_back(p);
            return 0;
        }
        MyPoint p1(p);
        for (std::vector<Point>::iterator it = exitSetPoints.begin(); it != exitSetPoints.end(); it++)
        {
            double distance = innerMetric->Distance(p, *it);
            if (distance < p1.distance)
            {
                p1.distance = distance;
            }
        }
        points.push_back(p1);
        return p1.distance;
    }

    double Distance(const Point &x, const Point &y)
    {
        double dx = DistanceToExitSet(x);
        double dy = DistanceToExitSet(y);
        if (dx == 0 || dy == 0)
        {
            return dx + dy;
        }
        return std::min(dx + dy, innerMetric->Distance(x, y));
    }

    bool IsInExitSet(const Point &p)
    {
        if (!domain->IsInDomain(p))
        {
            return true;
        }
        for (std::vector<MyPoint>::iterator it = points.begin(); it != points.end(); it++)
        {
            if (it->Equals(p))
            {
                return false;
            }
        }
        return true;
    }

    double DistanceToExitSet(const Point &p)
    {
        if (!domain->IsInDomain(p))
        {
            return 0;
        }
        for (std::vector<MyPoint>::iterator it = points.begin(); it != points.end(); it++)
        {
            if (it->Equals(p))
            {
                return it->distance;
            }
        }
        return 0;
    }

    Metric *GetInnerMetric()
    {
        return innerMetric;
    }
};

#endif	/* EXITSETQUOTIENTMETRIC_HPP */

