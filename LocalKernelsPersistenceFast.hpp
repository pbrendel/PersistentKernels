/* 
 * File:   LocalKernelsPersistenceFast.hpp
 * Author: Piotr Brendel
 */

#ifndef LOCALKERNELSPERSISTENCEFAST_HPP
#define	LOCALKERNELSPERSISTENCEFAST_HPP

#include "Point.hpp"
#include "Domain.hpp"
#include "MapSupplier.hpp"
#include "KernelsPersistence.hpp"
#include "HomologyHelpers.hpp"
#include "QualityFunction.hpp"
#include "Rips.hpp"

#include <deque>

class PersistenceDgmFast
{
public:

    struct HomologyClass
    {
        int column;
        int birth;
        int death;

        HomologyClass(int c, int b, int d) : column(c), birth(b), death(d)
        {
        }
    };

    std::vector<HomologyClass> homologyClasses;

    PersistenceDgmFast(std::vector<std::map<int, int> > &projections)
    {
        bool show = false;
        std::vector<std::vector<int> > kernels;
        for (std::vector<std::map<int, int> >::iterator projection = projections.begin(); projection != projections.end(); projection++)
        {
            std::set<int> range;
            std::vector<int> kernel;
            for (std::map<int, int>::iterator p = projection->begin(); p != projection->end(); p++)
            {
                // if was projected for a given generator then element is in kernel
                if (std::find(range.begin(), range.end(), p->second) != range.end())
                {
                    kernel.push_back(p->first);
                }
                else
                {
                    range.insert(p->second);
                }
            }
            kernels.push_back(kernel);
        }
        std::map<int, int> duration;
        std::map<int, int> start;
        for (int i = 0; i < kernels.size(); i++)
        {
            std::vector<int> &kernel = kernels[i];
            for (std::vector<int>::iterator k = kernel.begin(); k != kernel.end(); k++)
            {
                if (duration[*k] == 0)
                {
                    duration[*k] = 1;
                    start[*k] = i;
                }
                else
                {
                    duration[*k] = duration[*k] + 1;
                }
            }
        }
        int column = 0;
        for (std::map<int, int>::iterator it = start.begin(); it != start.end(); it++)
        {
            int birth = it->second;
            int death = birth + duration[it->first];
            homologyClasses.push_back(HomologyClass(column++, birth, death));
        }
    }
};

template <typename HomologyTraits>
class PersistenceDataFastT
{
public:

    typedef PersistenceDgmFast PersistenceDgmType;

private:

    Point argument;
    Point value;
    CRef<PersistenceDgmType> persistenceDgm;
    double quality;
    bool discontinuity;

public:


    PersistenceDataFastT(const Point &arg, std::vector<std::map<int, int> > &projections, bool discontinuity)
    {
        argument.assign(arg.begin(), arg.end());
        persistenceDgm = CRef<PersistenceDgmType>(new PersistenceDgmType(projections));
        this->discontinuity = discontinuity;
    }

    void ApplyMap(Map *map)
    {
        value.resize(map->GetDimension());
        map->GetValue(argument, value);
    }

    void CalculateQuality(QualityFunction<PersistenceDataFastT> *qualityFunction)
    {
        quality = qualityFunction->Calculate(persistenceDgm);
    }

    CRef<PersistenceDgmType> GetPersistenceDgm()
    {
        return persistenceDgm;
    }

    void Print(std::ostream &str)
    {
        for (Point::iterator i = argument.begin(); i != argument.end(); i++)
        {
            str<<*i<<" ";
        }
        for (Point::iterator i = value.begin(); i != value.end(); i++)
        {
            str<<*i<<" ";
        }
        str<<quality<<" "<<discontinuity<<std::endl;
    }
    
    void PrintExitSet(std::ostream &str, HorseshoeExitSetQuotientMetric *exitSetMetric)
    {
        for (Point::iterator i = argument.begin(); i != argument.end(); i++)
        {
            str<<*i<<" ";
        }
        for (Point::iterator i = value.begin(); i != value.end(); i++)
        {
            str<<*i<<" ";
        }
        str<<exitSetMetric->DistanceToExitSet(argument)<<" "<<discontinuity<<std::endl;
    }    
};

template <typename HomologyTraits>
class LocalKernelsPersistenceFastT
{
public:

    typedef PersistenceDataFastT<HomologyTraits> PersistenceData;

private:

    typedef typename PersistenceData::PersistenceDgmType PersistenceDgmType;
    typedef RipsT<Point> RipsComplex;

    std::vector<Point> domainPoints;
    std::vector<Point> graphPoints;

    EuclideanMetric euclideanMetric;
    Metric *domainMetric;
    Metric *graphMetric;

    std::vector<int> kernelSize;

    static const int maxRipsDim = 1;

    void SetMetrics(Metric *domainMetric, Metric *graphMetric)
    {
        this->domainMetric = (domainMetric != 0) ? domainMetric : &euclideanMetric;
        this->graphMetric = (graphMetric != 0) ? graphMetric : &euclideanMetric;
    }

    void CreateDomainGraphPoints(Domain *domain, Map *map)
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

    void Print(const CRef<PersistenceDgmType> &dgm)
    {
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
            std::cout<<hc.column<<": "<<hc.birth<<" "<<hc.death<<" "<<std::endl;
        }
    }

public:

    std::vector<PersistenceData *> Compute(Domain *domain, Map *map, Domain *testDomain, const std::vector<double> &epsilons, double restrictionRadius, Metric *domMetric = 0, Metric *grMetric = 0)
    {
        std::vector<PersistenceData *> persistenceData;
        SetMetrics(domMetric, grMetric);
        Point center(domain->GetDimension());
        int count = testDomain->GetCount();
        for (int i = 0; i < count; i++)
        {
            testDomain->GetValue(i, center);
            DomainRestriction restriction(domain, domainMetric, center, restrictionRadius);
            if (restriction.GetCount() == 0)
            {
                continue;
            }
            CreateDomainGraphPoints(&restriction, map);
            std::vector<std::map<int, int> > projections;
            for (int j = 0; j < epsilons.size(); j++)
            {
                std::cout<<"-";
                RipsComplex ripsComplexDomain(domainPoints, domainMetric, epsilons[j], maxRipsDim);
                int domainConnectedComponents = ripsComplexDomain.GetConnectedComponentsNumber();
                RipsComplex ripsComplexGraph(graphPoints, graphMetric, epsilons[j], maxRipsDim);
                int graphConnectedComponents = ripsComplexGraph.GetConnectedComponentsNumber();

                assert(graphConnectedComponents >= domainConnectedComponents);
                std::map<int, int> projection;
                ripsComplexGraph.GetProjectionMap(ripsComplexDomain, projection);
                projections.push_back(projection);
                if (graphConnectedComponents == 1)
                {
                    break;
                }
            }
            std::cout<<std::endl;
            persistenceData.push_back(new PersistenceData(center, projections, false));
            //Print(persistenceData[persistenceData.size() - 1]->GetPersistenceDgm());
        }
        return persistenceData;
    }
};

#endif	/* LOCALKERNELSPERSISTENCEFAST_HPP */

