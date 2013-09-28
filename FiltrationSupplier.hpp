/*
 * File:   FiltrationSupplier.hpp
 * Author: admin
 *
 * Created on August 9, 2012, 2:28 PM
 */

#ifndef FILTRATIONSUPPLIER_HPP
#define	FILTRATIONSUPPLIER_HPP

#include "Point.hpp"
#include "Rips.hpp"
#include "Metric.hpp"

#include "HomologyHelpers.hpp"

template <typename HomologyTraits>
class FiltrationSupplier
{
public:

    virtual ~FiltrationSupplier() { }

    typedef typename HomologyTraits::ComplexType ComplexType;
    typedef typename HomologyTraits::FiltrType FiltrType;

    virtual void GetComplex(CRef<ComplexType>& complexCR) = 0;
    virtual void GetNextComplex(CRef<ComplexType>& nextComplexCR) = 0;

    virtual bool IsFirst() = 0;
    virtual bool IsLast() = 0;
    virtual bool Next() = 0;
    virtual int FiltrationsCount() = 0;
};

////////////////////////////////////////////////////////////////////////////////

template <typename HomologyTraits>
class PointsCloudSupplier : public FiltrationSupplier<HomologyTraits>
{

protected:

    //typedef rips::simplicialComplex<Point> RipsComplex;
    typedef RipsT<Point> RipsComplex;

    typedef FiltrationSupplier<HomologyTraits> BaseType;
    typedef typename BaseType::ComplexType ComplexType;

    vector<Point> points;
    vector<double> epsilons;

    Metric *metric;

    CRef<ComplexType> complexCR;
    CRef<ComplexType> nextComplexCR;

    int current;

    static const int maxRipsDim = 1;

    PointsCloudSupplier(Metric *metric, const std::vector<double> &epsilons)
    {
        this->metric = metric;
        this->epsilons.assign(epsilons.begin(), epsilons.end());
        current = -1;
    }

public:

    void GetComplex(CRef<ComplexType>& complexCR)
    {
        complexCR = this->complexCR;
    }

    void GetNextComplex(CRef<ComplexType>& nextComplexCR)
    {
        nextComplexCR = this->nextComplexCR;
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
            std::cout<<"Processing data for epsilon["<<current<<"]="<<epsilons[current]<<std::endl;
            RipsComplex ripsComplex(points, metric, epsilons[current], maxRipsDim);
            complexCR = CRef<ComplexType>(new ComplexType());
            ripsComplex.GetComplex(complexCR());
            //std::cout<<ripsComplex<<std::endl;
            std::vector<int> bettiNumbers = HomologyHelpers<HomologyTraits>::GetBettiNumbers(ripsComplex);
            std::cout<<"betti numbers: "<<bettiNumbers<<std::endl;
        }
        else
        {
            complexCR = nextComplexCR;
        }

        if (!IsLast())
        {
            std::cout<<"Processing data for epsilon["<<(current + 1)<<"]="<<epsilons[current + 1]<<std::endl;
            RipsComplex ripsComplex(points, metric, epsilons[current + 1], maxRipsDim);
            nextComplexCR = CRef<ComplexType>(new ComplexType());
            ripsComplex.GetComplex(nextComplexCR());
            //std::cout<<ripsComplex<<std::endl;
            std::vector<int> bettiNumbers = HomologyHelpers<HomologyTraits>::GetBettiNumbers(ripsComplex);
            std::cout<<"betti numbers: "<<bettiNumbers<<std::endl;
        }

        return true;
    }

    int FiltrationsCount()
    {
        return epsilons.size();
    }
};

////////////////////////////////////////////////////////////////////////////////

template <typename HomologyTraits, typename Domain>
class DomainSupplier : public PointsCloudSupplier<HomologyTraits>
{
    typedef PointsCloudSupplier<HomologyTraits> BaseType;
    typedef typename BaseType::Point Point;
    typedef typename BaseType::RipsComplex RipsComplex;
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

public:

    DomainSupplier(Domain *domain, Metric *metric, const std::vector<double> &epsilons) : PointsCloudSupplier<HomologyTraits>(metric, epsilons)
    {
        Point d(domain->GetDimension());
        int count = domain->GetCount();
        for (int i = 0; i < count; i++)
        {
            domain->GetValue(i, d);
            this->points.push_back(d);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

template <typename HomologyTraits, typename Domain, typename Map>
class MapRangeSupplier : public PointsCloudSupplier<HomologyTraits>
{
    typedef PointsCloudSupplier<HomologyTraits> BaseType;
    typedef typename BaseType::Point Point;
    typedef typename BaseType::RipsComplex RipsComplex;
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

public:

    MapRangeSupplier(Domain *domain, Map *map, Metric *metric, const std::vector<double> &epsilons) : PointsCloudSupplier<HomologyTraits>(metric, epsilons)
    {
        Point d(domain->GetDimension());
        Point r(map->GetDimension());
        int count = domain->GetCount();
        for (int i = 0; i < count; i++)
        {
            domain->GetValue(i, d);
            map->GetValue(d, r);
            this->points.push_back(r);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////


#endif	/* FILTRATIONSUPPLIER_HPP */

