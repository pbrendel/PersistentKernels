/* 
 * File:   LocalKernelsPersistence.hpp
 * Author: Piotr Brendel
 */

#ifndef LOCALKERNELSPERSISTENCE_HPP
#define	LOCALKERNELSPERSISTENCE_HPP

#include "Point.hpp"
#include "Domain.hpp"
#include "MapSupplier.hpp"
#include "KernelsPersistence.hpp"
#include "HomologyHelpers.hpp"
#include "QualityFunction.hpp"

template <typename HomologyTraits>
class PersistenceDataT
{
public:

    typedef typename KernelsPersistenceT<HomologyTraits>::PersistenceDgmType PersistenceDgmType;

private:

    Point argument;
    Point value;
    double quality;
    bool discontinuity;
    CRef<PersistenceDgmType> persistenceDgm;

public:


    PersistenceDataT(const Point &arg, CRef<PersistenceDgmType> persistenceDgm, bool discontinuity)
    {
        argument.assign(arg.begin(), arg.end());
        this->persistenceDgm = persistenceDgm;
        this->discontinuity = discontinuity;
    }

    void ApplyMap(Map *map)
    {
        value.resize(map->GetDimension());
        map->GetValue(argument, value);
    }

    void CalculateQuality(QualityFunction<PersistenceDataT> *qualityFunction)
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
};

template <typename HomologyTraits>
class LocalKernelsPersistenceT
{
    typedef typename KernelsPersistenceT<HomologyTraits>::PersistenceDgmType PersistenceDgmType;

    EuclideanMetric euclideanMetric;
    Metric *domainMetric;
    Metric *graphMetric;

    void SetMetrics(Metric *domainMetric, Metric *graphMetric)
    {
        this->domainMetric = (domainMetric != 0) ? domainMetric : &euclideanMetric;
        this->graphMetric = (graphMetric != 0) ? graphMetric : &euclideanMetric;
    }

    bool VerifyDiscontinuity(CRef<PersistenceDgmType> &dgm, int lastFiltrationIndex)
    {
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
            if (hc.death  == lastFiltrationIndex)
            {
                return true;
            }
        }
        return false;
    }

    void Print(CRef<PersistenceDgmType> &dgm)
    {
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
            std::cout<<hc.column<<": "<<hc.birth<<" "<<hc.death<<" "<<std::endl;
        }
    }

public:

    typedef PersistenceDataT<HomologyTraits> PersistenceData;

    std::vector<PersistenceData *> Compute(Domain *domain, Map *map, Domain *testDomain, const std::vector<double> &epsilons, double restrictionRadius, Metric *domMetric = 0, Metric *grMetric = 0)
    {
        std::vector<PersistenceData *> persistenceData;
        SetMetrics(domMetric, grMetric);
        Point center(domain->GetDimension());
        int count = testDomain->GetCount();

//        Point p1(2), p2(2), v1(2), v2(2), g1(4), g2(4);
//        p1[0] = 0.5;
//        p1[1] = 0.78;
//        p2[0] = 0.5;
//        p2[1] = 0.79;
//        map->GetValue(p1, v1);
//        map->GetValue(p2, v2);
//        g1[0] = p1[0];
//        g1[1] = p1[1];
//        g1[2] = v1[0];
//        g1[3] = v1[1];
//        g2[0] = p2[0];
//        g2[1] = p2[1];
//        g2[2] = v2[0];
//        g2[3] = v2[1];
//        std::cout<<p1[0]<<" "<<p1[1]<<" -> "<<v1[0]<<" "<<v1[1]<<std::endl;
//        std::cout<<p2[0]<<" "<<p2[1]<<" -> "<<v2[0]<<" "<<v2[1]<<std::endl;
//        std::cout<<domainMetric->Distance(p1, p2)<<" "<<domainMetric->Distance(v1, v2)<<" -> "<<graphMetric->Distance(g1, g2)<<std::endl;

        for (int i = 0; i < count; i++)
        {
            testDomain->GetValue(i, center);
            DomainRestriction restriction(domain, domainMetric, center, restrictionRadius);
            if (restriction.GetCount() == 0)
            {
                continue;
            }
            MapSupplierBase<HomologyTraits> *supplier = new MapSupplier<HomologyTraits>(&restriction, map, domainMetric, graphMetric, epsilons);
            CRef<PersistenceDgmType> dgm = KernelsPersistenceT<HomologyTraits>::Compute(supplier);
            std::cout<<std::endl;
            //Print(dgm);
//            for (Point::iterator v = center.begin(); v != center.end(); v++)
//            {
//                std::cout<<*v<<" ";
//            }
//            std::cout<<std::endl;
//            bool discontinuity = false;
//            if (VerifyDiscontinuity(dgm, epsilons.size() - 1))
//            {
//                std::cout<<std::endl<<"discontinuity in the neighbourhood of ";
//                for (Point::iterator v = center.begin(); v != center.end(); v++)
//                {
//                    std::cout<<*v<<" ";
//                }
//                std::cout<<std::endl;
//                discontinuity = true;
//            }
            persistenceData.push_back(new PersistenceData(center, dgm, false));
            delete supplier;
        }
        return persistenceData;
    }
};

#endif	/* LOCALKERNELSPERSISTENCE_HPP */

