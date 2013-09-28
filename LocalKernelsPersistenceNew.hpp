/* 
 * File:   LocalKernelsPersistenceNew.hpp
 * Author: Piotr Brendel
 */

#ifndef LOCALKERNELSPERSISTENCENEW_HPP
#define	LOCALKERNELSPERSISTENCENEW_HPP

#include "Point.hpp"
#include "Domain.hpp"
#include "MapSupplier.hpp"
#include "KernelsPersistence.hpp"
#include "HomologyHelpers.hpp"
#include "QualityFunction.hpp"
#include "Rips.hpp"
#include "HorseshoeMap.hpp"
#include "ExitSetQuotientMetric.hpp"

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
    std::vector<Point> rangePoints;
    std::vector<Point> graphPoints;

    int domainDim;
    int rangeDim;

    EuclideanMetric euclideanMetric;
    Metric *domainMetric;
    Metric *graphMetric;
    IndexMetric *domainIndexMetric;
    IndexMetric *rangeIndexMetric;
    IndexMetric *graphIndexMetric;
    IndexMetric *localDomainIndexMetric;
    IndexMetric *localRangeIndexMetric;
    IndexMetric *localGraphIndexMetric;

    std::vector<int> kernelSize;

    static const int maxRipsDim = 1;

    void SetDomainMetric(Metric *metric)
    {
        domainMetric = (metric != 0) ? metric : &euclideanMetric;
        graphMetric = new MaxDomainRangeMetric(domainMetric, domainDim, domainMetric, rangeDim);
    }

    void UpdateMetrics()
    {
        ExitSetQuotientMetric *exitSetQuotientMetric = dynamic_cast<ExitSetQuotientMetric*>(domainMetric);
        if (exitSetQuotientMetric != 0)
        {
            delete domainIndexMetric;
            delete rangeIndexMetric;
            delete graphIndexMetric;
            domainIndexMetric = new ExitSetQuotientIndexMetric(domainPoints, exitSetQuotientMetric);
            rangeIndexMetric = new ExitSetQuotientIndexMetric(rangePoints, exitSetQuotientMetric);
            graphIndexMetric = new MaxDomainRangeIndexMetric(domainIndexMetric, domainDim, rangeIndexMetric, rangeDim);
        }
    }

    void UpdateLocalMetrics()
    {
        ExitSetQuotientMetric *exitSetQuotientMetric = dynamic_cast<ExitSetQuotientMetric*>(domainMetric);
        if (exitSetQuotientMetric != 0)
        {
            delete localDomainIndexMetric;
            delete localRangeIndexMetric;
            delete localGraphIndexMetric;
            localDomainIndexMetric = new ExitSetQuotientIndexMetric(domainPoints, exitSetQuotientMetric);
            localRangeIndexMetric = new ExitSetQuotientIndexMetric(rangePoints, exitSetQuotientMetric);
            localGraphIndexMetric = new MaxDomainRangeIndexMetric(localDomainIndexMetric, domainDim, localRangeIndexMetric, rangeDim);
        }
    }

    void UpdateDomainMetricsWithPoint(const Point &p, Map *map)
    {
        ExitSetQuotientMetric *exitSetQuotientMetric = dynamic_cast<ExitSetQuotientMetric*>(domainMetric);
        if (exitSetQuotientMetric != 0)
        {
            double distance = exitSetQuotientMetric->AddPoint(p, map);
            ExitSetQuotientIndexMetric *exitSetQuotientIndexMetric = dynamic_cast<ExitSetQuotientIndexMetric*>(domainIndexMetric);
            if (exitSetQuotientIndexMetric != 0)
            {
                exitSetQuotientIndexMetric->AddPoint(distance);
            }
        }
    }

    void CreateDomainRangeGraphPoints(Domain *domain, Map *map)
    {
        domainPoints.clear();
        rangePoints.clear();
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
            this->rangePoints.push_back(r);
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

    template <typename MetricType>
    void FindEpsilons(MetricType *metric, double &prevEpsilon, double &epsilon, double alpha)
    {
        std::cout<<"finding epsilon"<<std::endl;
        int steps = 1;
        RipsComplex ripsGraph(graphPoints, metric, epsilon, 1);
        while (ripsGraph.GetConnectedComponentsNumber() > 1)
        {
            std::cout<<ripsGraph.GetConnectedComponentsNumber()<<" "<<prevEpsilon<<std::endl;
            steps++;
            prevEpsilon = epsilon;
            epsilon *= 2;
            ripsGraph = RipsComplex(graphPoints, metric, epsilon, 1);
        }
        ripsGraph = RipsComplex(graphPoints, metric, prevEpsilon, 1);
        while (ripsGraph.GetConnectedComponentsNumber() == 1)
        {
            std::cout<<ripsGraph.GetConnectedComponentsNumber()<<" "<<prevEpsilon<<std::endl;
            steps++;
            prevEpsilon *= 0.5;
            ripsGraph = RipsComplex(graphPoints, metric, prevEpsilon, 1);
        }
        while ((epsilon - prevEpsilon) > alpha)
        {
            steps++;
            double newEpsilon = (epsilon + prevEpsilon) * 0.5;
            ripsGraph = RipsComplex(graphPoints, metric, newEpsilon, 1);
            std::cout<<"step "<<steps<<" newEpsilon "<<newEpsilon<<" cc "<<ripsGraph.GetConnectedComponentsNumber()<<std::endl;
            if (ripsGraph.GetConnectedComponentsNumber() == 1)
            {
                epsilon = newEpsilon;
            }
            else
            {
                prevEpsilon = newEpsilon;
            }
        }
        std::cout<<RipsComplex(graphPoints, metric, prevEpsilon, 1).GetConnectedComponentsNumber()<<std::endl;
        std::cout<<RipsComplex(graphPoints, metric, epsilon, 1).GetConnectedComponentsNumber()<<std::endl;
        std::cout<<"epsilons found "<<prevEpsilon<<" "<<epsilon<<" in "<<steps<<" steps"<<std::endl;        
    }

public:

    LocalKernelsPersistenceFastT()
    {
        domainMetric = 0;
        graphMetric = 0;
        domainIndexMetric = 0;
        rangeIndexMetric = 0;
        graphIndexMetric = 0;
        localDomainIndexMetric = 0;
        localRangeIndexMetric = 0;
        localGraphIndexMetric = 0;
    }

    ~LocalKernelsPersistenceFastT()
    {
        delete graphMetric;
        delete domainIndexMetric;
        delete rangeIndexMetric;
        delete graphIndexMetric;
        delete localDomainIndexMetric;
        delete localRangeIndexMetric;
        delete localGraphIndexMetric;
    }

    std::vector<PersistenceData *> Compute(Domain *domain, Map *map, double alpha, double beta, Metric *domMetric = 0)
    {
        std::vector<PersistenceData *> persistenceData;
        if (domain->GetCount() < 2)
        {
            return persistenceData;
        }
        domainDim = domain->GetDimension();
        rangeDim = map->GetDimension();
        CreateDomainRangeGraphPoints(domain, map);
        SetDomainMetric(domMetric);
        UpdateMetrics();
        double epsilon = 0.1;
        double prevEpsilon = epsilon * 0.5;
        if (graphIndexMetric != 0)
        {
            FindEpsilons(graphIndexMetric, prevEpsilon, epsilon, alpha);
        }
        else
        {
            FindEpsilons(graphMetric, prevEpsilon, epsilon, alpha);
        }
        // HorseshoeExitSetQuotientMetric *tmpMetric = dynamic_cast<HorseshoeExitSetQuotientMetric *>(domainMetric);
        std::cout<<"checking continuity"<<std::endl;
        std::vector<double> newEpsilons;
        newEpsilons.push_back((1.0 + beta) * epsilon);
        Point center(domain->GetDimension());
        int count = domain->GetCount();
        for (int i = 0; i < count; i++)
        {            
            domain->GetValue(i, center);
            for (Point::iterator it = center.begin(); it != center.end(); it++)
            {
                std::cout<<*it<<" ";
            }
            std::cout<<std::endl;
            UpdateDomainMetricsWithPoint(center, map);
            DomainRestriction restriction(domain);
            if (domainIndexMetric != 0)
            {
                restriction.Create(domain, domainIndexMetric, center, epsilon);
            }
            else
            {
                restriction.Create(domain, domainMetric, center, epsilon);
            }
//            DomainRestriction restriction(domain, domainMetric, center, epsilon);
            if (restriction.GetCount() == 0)
            {
                continue;
            }
            CreateDomainRangeGraphPoints(&restriction, map);
            UpdateLocalMetrics();
            std::vector<std::map<int, int> > projections;
            for (int j = 0; j < newEpsilons.size(); j++)
            {
                RipsComplex ripsComplexDomain;
                if (localDomainIndexMetric != 0)
                {
                    ripsComplexDomain.Create(domainPoints, localDomainIndexMetric, newEpsilons[j], maxRipsDim);
                }
                else
                {
                    ripsComplexDomain.Create(domainPoints, domainMetric, newEpsilons[j], maxRipsDim);
                }
//                RipsComplex ripsComplexDomain(domainPoints, domainMetric, newEpsilons[j], maxRipsDim);
                int domainConnectedComponents = ripsComplexDomain.GetConnectedComponentsNumber();
                RipsComplex ripsComplexGraph;
                if (localGraphIndexMetric != 0)
                {
                    ripsComplexGraph.Create(graphPoints, localGraphIndexMetric, newEpsilons[j], maxRipsDim);
                }
                else
                {
                    ripsComplexGraph.Create(graphPoints, graphMetric, newEpsilons[j], maxRipsDim);
                }
//                RipsComplex ripsComplexGraph(graphPoints, graphMetric, newEpsilons[j], maxRipsDim);
                int graphConnectedComponents = ripsComplexGraph.GetConnectedComponentsNumber();
//                std::cout<<graphConnectedComponents<<"->"<<domainConnectedComponents<<std::endl;
                assert(graphConnectedComponents >= domainConnectedComponents);
                std::map<int, int> projection;
                ripsComplexGraph.GetProjectionMap(ripsComplexDomain, projection);
                projections.push_back(projection);
//                if (graphConnectedComponents == 1)
//                {
//                    break;
//                }
            }
            persistenceData.push_back(new PersistenceData(center, projections, false));
        }
        std::cout<<"done"<<std::endl;
        return persistenceData;
    }
};

#endif	/* LOCALKERNELSPERSISTENCENEW_HPP */

