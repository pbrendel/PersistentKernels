/* 
 * File:   LocalKernels.hpp
 * Author: Piotr Brendel
 */

#ifndef LOCALKERNELS_HPP
#define	LOCALKERNELS_HPP

#include <vector>

#include "Point.hpp"
#include "Domain.hpp"
#include "Rips.hpp"

#include "Utils.hpp"
#include "HomologyHelpers.hpp"

template <typename HomologyTraits>
class LocalKernels
{
    typedef RipsT<Point> RipsComplex;

    typedef typename HomologyTraits::ComplexType ComplexType;
    typedef typename HomologyTraits::FiltrType FiltrType;

    EuclideanMetric euclideanMetric;
    Metric *domainMetric;
    Metric *graphMetric;

    vector<Point> domainPoints;
    vector<Point> graphPoints;

    void SetMetrics(Metric *domainMetric, Metric *graphMetric)
    {
        this->domainMetric = (domainMetric != 0) ? domainMetric : &euclideanMetric;
        this->graphMetric = (graphMetric != 0) ? graphMetric : &euclideanMetric;
    }
    
    template<typename DomainType, typename MapType>
    void CreateDomainAndGraphPoints(DomainType *domain, MapType *map)
    {
        domainPoints.clear();
        graphPoints.clear();
        Point d(domain->GetDimension());
        Point r(map->GetDimension());
        Point g(domain->GetDimension() + map->GetDimension());
        int count = domain->GetCount();
        for (int i = 0; i < count; i++)
        {
            domain->GetValue(i, d);
            map->GetValue(d, r);
            this->domainPoints.push_back(d);
            for (int i = 0; i < d.size(); i++)
            {
                g[i] = d[i];
            }
            for (int i = 0; i < r.size(); i++)
            {
                g[d.size() + i] = r[i];
            }
            this->graphPoints.push_back(g);
        }
    }

    double FindEpsilon(const std::vector<double> &epsilons)
    {
        for (std::vector<double>::const_iterator e = epsilons.begin(); e != epsilons.end(); e++)
        {
            RipsComplex complex(graphPoints, graphMetric, *e, 1);
            if (Get0thBettiNumber(complex) == 1)
            {
                return *e;
            }
        }
        return 0;
    }

    int Get0thBettiNumber(RipsComplex &ripsComplex)
    {
        std::vector<int> bettiNumbers = HomologyHelpers<HomologyTraits>::GetBettiNumbers(ripsComplex);
        return bettiNumbers[0];
    }

public:

    template<typename DomainType, typename MapType>
    void Compute(DomainType *domain, MapType *map, const std::vector<double> &epsilons, const std::vector<int> &resolution, Metric *domMetric = 0, Metric *grMetric = 0)
    {
        SetMetrics(domMetric, grMetric);
        CreateDomainAndGraphPoints(domain, map);
        double epsilon = FindEpsilon(epsilons);
        std::cout<<"epsilon = "<<epsilon<<std::endl;
        UniformCube net(domain->GetRanges(), resolution);
        // RandomCube<DomainType::dim> net(domain->GetCube(), resolution[0]);
        Point center(domain->GetDimension());
        int count = net.GetCount();
        for (int i = 0; i < count; i++)
        {
            net.GetValue(i, center);
            DomainRestriction restriction(domain, domainMetric, center, epsilon);
            RipsComplex complex(graphPoints, graphMetric, epsilon, 1);
            if (Get0thBettiNumber(complex) != 1)
            {
                std::cout<<"discontinuity in the neighbourhood of ";
                for (Point::iterator v = center.begin(); v != center.end(); v++)
                {
                    std::cout<<*v<<" ";
                }
                std::cout<<std::endl;
            }
        }
    }
};

#endif	/* LOCALKERNELS_HPP */

