/*
 * File:   MapSupplier.hpp
 * Author: Piotr Brendel
 */

#ifndef MAPSUPPLIER_HPP
#define	MAPSUPPLIER_HPP

#include "Point.hpp"
#include "Rips.hpp"
#include "Domain.hpp"
#include "Map.hpp"
#include "Metric.hpp"

#include "Utils.hpp"
#include "HomologyHelpers.hpp"

////////////////////////////////////////////////////////////////////////////////

template <typename HomologyTraits>
class MapSupplierBase
{
public:

    virtual ~MapSupplierBase() { }

    typedef typename HomologyTraits::ComplexType ComplexType;
    typedef typename HomologyTraits::FiltrType FiltrType;

    virtual void GetDomainComplex(CRef<ComplexType>& complexCR) = 0;
    virtual void GetNextDomainComplex(CRef<ComplexType>& nextComplexCR) = 0;
    virtual void GetGraphComplex(CRef<ComplexType>& graphComplexCR) = 0;
    virtual void GetNextGraphComplex(CRef<ComplexType>& nextGraphComplexCR) = 0;

    virtual bool IsFirst() = 0;
    virtual bool IsLast() = 0;
    virtual bool Next() = 0;
    virtual int FiltrationsCount() = 0;
};

////////////////////////////////////////////////////////////////////////////////

template <typename HomologyTraits>
class MapSupplier : public MapSupplierBase<HomologyTraits>
{
    typedef RipsT<Point> RipsComplex;
    //typedef rips::simplicialComplex<Point> RipsComplex;

    typedef MapSupplierBase<HomologyTraits> BaseType;
    typedef typename BaseType::ComplexType ComplexType;

    vector<Point> domain;
    vector<Point> graph;
    vector<double> epsilons;

    Metric *domainMetric;
    Metric *graphMetric;

    CRef<ComplexType> domainComplexCR;
    CRef<ComplexType> nextDomainComplexCR;
    CRef<ComplexType> graphComplexCR;
    CRef<ComplexType> nextGraphComplexCR;

    int current;

    static const int maxRipsDim = 1;

public:

    MapSupplier(Domain *domain, Map *map, Metric *domainMetric, Metric *graphMetric, const std::vector<double> &epsilons)
    {
        this->domainMetric = domainMetric;
        this->graphMetric = graphMetric;
        Point d(domain->GetDimension());
        Point r(map->GetDimension());
        Point g(domain->GetDimension() + map->GetDimension());
        int count = domain->GetCount();
        for (int i = 0; i < count; i++)
        {
            domain->GetValue(i, d);
            map->GetValue(d, r);
            this->domain.push_back(d);
            for (int i = 0; i < d.size(); i++)
            {
                g[i] = d[i];
            }
            for (int i = 0; i < r.size(); i++)
            {
                g[d.size() + i] = r[i];
            }
            this->graph.push_back(g);
        }
        //for (int i = 0; i < count; i++)
        //{
        //    std::cout<<this->domain[i]<<" -> "<<this->graph[i]<<std::endl;
        //}
        this->epsilons.assign(epsilons.begin(), epsilons.end());
        current = -1;
    }

    void GetDomainComplex(CRef<ComplexType>& domainComplexCR)
    {
        domainComplexCR = this->domainComplexCR;
    }

    void GetNextDomainComplex(CRef<ComplexType>& nextDomainComplexCR)
    {
        nextDomainComplexCR = this->nextDomainComplexCR;
    }

    void GetGraphComplex(CRef<ComplexType>& graphComplexCR)
    {
        graphComplexCR = this->graphComplexCR;
    }

    void GetNextGraphComplex(CRef<ComplexType>& nextGraphComplexCR)
    {
        nextGraphComplexCR = this->nextGraphComplexCR;
    }

    bool IsFirst()
    {
        return current == 0;
    }

    bool IsLast()
    {
        return current == epsilons.size() - 1;
    }

    bool Next()
    {
        current++;
        if (current >= epsilons.size())
        {
            return false;
        }


        if (IsFirst())
        {
        // std::cout<<"Processing data for epsilon["<<current<<"]="<<epsilons[current]<<endl;

            RipsComplex ripsComplexDomain(domain, domainMetric, epsilons[current], maxRipsDim);
            domainComplexCR = CRef<ComplexType>(new ComplexType());
            ripsComplexDomain.GetComplex(domainComplexCR());

            RipsComplex ripsComplexGraph(graph, graphMetric, epsilons[current], maxRipsDim);
            graphComplexCR = CRef<ComplexType>(new ComplexType());
            ripsComplexGraph.GetComplex(graphComplexCR());
        }
        else
        {
            graphComplexCR = nextGraphComplexCR;
            domainComplexCR = nextDomainComplexCR;
        }

        if (!IsLast())
        {
        // std::cout<<"Processing data for epsilon["<<(current + 1)<<"]="<<epsilons[current + 1]<<endl;

            RipsComplex ripsComplexDomain(domain, domainMetric, epsilons[current + 1], maxRipsDim);
            nextDomainComplexCR = CRef<ComplexType>(new ComplexType());
            ripsComplexDomain.GetComplex(nextDomainComplexCR());

            RipsComplex ripsComplexGraph(graph, graphMetric, epsilons[current + 1], maxRipsDim);
            nextGraphComplexCR = CRef<ComplexType>(new ComplexType());
            ripsComplexGraph.GetComplex(nextGraphComplexCR());
        }
        /*
        else
        {
            std::cout<<"last complexes"<<std::endl;

            RipsComplex ripsComplexDomain(domain, domainMetric, epsilons[current], maxRipsDim);


            RipsComplex ripsComplexGraph(graph, graphMetric, epsilons[current], maxRipsDim);

            CRef<ComplexType> comGr = CRef<ComplexType>(new ComplexType());
            ripsComplexGraph.GetComplex(comGr());

            //std::cout<<ripsComplexDomain<<std::endl;
            std::cout<<ripsComplexGraph<<std::endl;

            std::cout<<HomologyHelpers<HomologyTraits>::CreateFiltr(comGr(), true, true, true)();

            ripsComplexGraph.DoTest(3, 0.2);

            assert(false);
        }
         */

        return true;
    }

    int FiltrationsCount()
    {
        return epsilons.size();
    }
};

#endif	/* MAPSUPPLIER_HPP */

