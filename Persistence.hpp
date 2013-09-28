/* 
 * File:   Persistence.hpp
 * Author: admin
 *
 * Created on August 9, 2012, 2:16 PM
 */

#ifndef PERSISTENCE_HPP
#define	PERSISTENCE_HPP

#include "FilteredHomologyMapBuilder.hpp"
#include "SimpleProjection.hpp"
#include "FiltrationSupplier.hpp"
#include "HomologyHelpers.hpp"
#include <PersistenceDgm.hpp>
#include "Utils.hpp"

template <typename HomologyTraits>
class PersistenceT
{
public:

    typedef typename HomologyTraits::ComplexType ComplexType;
    typedef typename HomologyTraits::FiltrType FiltrType;
    typedef typename HomologyTraits::MatrixType MatrixType;
    typedef typename HomologyTraits::VectorType VectorType;
    typedef PersistenceDgm<MatrixType, MatrixType, IntVector> PersistenceDgmType;

    static std::vector<CRef<PersistenceDgmType> > Compute(FiltrationSupplier<HomologyTraits> *supplier)
    {
        bool useCoreductions = true;
        bool useShaving = true;
        bool restore = true;

        FilteredHomologyMapBuilder<HomologyTraits> inclHomologyBuilder(SimpleProjection<HomologyTraits>::Map, 1);

        CRef<ComplexType> complex, complexNext;
        CRef<FiltrType> homFiltrCR, homFiltrNextCR;

        while (supplier->Next())
        {
            supplier->GetComplex(complex);

            if (supplier->IsFirst())
            {
                homFiltrCR = HomologyHelpers<HomologyTraits>::CreateFiltr(complex(), useShaving, useCoreductions, restore);
            }
            else
            {
                homFiltrCR = homFiltrNextCR;
            }

            if (!supplier->IsLast())
            {
                supplier->GetNextComplex(complexNext);
                homFiltrNextCR = HomologyHelpers<HomologyTraits>::CreateFiltr(complexNext(), useShaving, useCoreductions, restore);
                SimpleProjection<HomologyTraits>::SetComplexes(complex, complexNext);
                inclHomologyBuilder.AddFiltration(homFiltrCR(), homFiltrNextCR());
            }
        }

        std::vector<CRef<PersistenceDgmType> > persistenceDiagrams;
        /*
        std::vector<CRef<std::vector<MatrixType> > > inclusions = inclHomologyBuilder.GetMatrices();
        for (int i = 0; i < inclusions.size(); i++)
        {
            std::vector<MatrixType> A;
            //A.assign(inclusions[i]().begin(), inclusions[i]().end());
            for (typename std::vector<MatrixType>::iterator m = inclusions[i]().begin(); m != inclusions[i]().end(); m++)
            {
          //      if (i == 0)
            //    std::cout<<*m<<std::endl;
                A.push_back(*m);
            }
        //    persistenceDiagrams.push_back(CRef<PersistenceDgmType>(new PersistenceDgmType(A)));
            CRef<PersistenceDgmType> dgm(new PersistenceDgmType(A));
            Print(dgm);
        }
         */
        return persistenceDiagrams;
    }

    template <typename Domain, typename Metric>
    static std::vector<CRef<PersistenceDgmType> > ComputeForDomain(Domain *domain, Metric *metric, const std::vector<double> &epsilons)
    {
        DomainSupplier<HomologyTraits, Domain> supplier(domain, metric, epsilons);
        return PersistenceT::Compute(&supplier);
    }

    template <typename Domain, typename Map, typename Metric>
    static std::vector<CRef<PersistenceDgmType> > ComputeForRange(Domain *domain, Map *map, Metric *metric, const std::vector<double> &epsilons)
    {
        MapRangeSupplier<HomologyTraits, Domain, Map> supplier(domain, map, metric, epsilons);
        return PersistenceT::Compute(&supplier);
    }

    static void Print(CRef<PersistenceDgmType> &dgm)
    {
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
            if (hc.death - hc.birth > 1)
            {
                std::cout << "born: " << hc.birth << ": " << endl;
                std::cout << "die: " << hc.death << ", ::[" << hc.birth << "," << hc.death << ")" << std::endl;
            }
        }
    }

    static void Print(std::vector<CRef<PersistenceDgmType> > &persistenceDiagrams)
    {
        for (int i = 0; i < persistenceDiagrams.size(); i++)
        {
            BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, persistenceDiagrams[i]().homologyClasses)
            {
                if (hc.death - hc.birth > 1)
                {
                    std::cout << "born: " << hc.birth << ": " << endl;
                    std::cout << "die: " << hc.death << ", ::[" << hc.birth << "," << hc.death << ")" << std::endl;
                }
            }
        }
    }
};

#endif	/* PERSISTENCE_HPP */

