/* 
 * File:   KernelsPersistence.hpp
 * Author: Piotr Brendel
 */

#ifndef KERNELSPERSISTENCE_HPP
#define	KERNELSPERSISTENCE_HPP

#include "FilteredHomologyMapBuilder.hpp"
#include "SimpleProjection.hpp"
#include "MapSupplier.hpp"
#include "HomologyHelpers.hpp"
#include <PersistenceDgm.hpp>

template <typename HomologyTraits>
class KernelsPersistenceT
{
public:

    typedef typename HomologyTraits::ComplexType ComplexType;
    typedef typename HomologyTraits::FiltrType FiltrType;
    typedef typename HomologyTraits::MatrixType MatrixType;
    typedef typename HomologyTraits::VectorType VectorType;
    typedef PersistenceDgm<MatrixType, MatrixType, IntVector> PersistenceDgmType;

    static CRef<PersistenceDgmType> Compute(MapSupplierBase<HomologyTraits> *supplier)
    {
        bool useCoreductions = true;
        bool useShaving = true;
        bool restore = true;

        FilteredHomologyMapBuilder<HomologyTraits> projHomologyBuilder(SimpleProjection<HomologyTraits>::Map, 0);
        FilteredHomologyMapBuilder<HomologyTraits> graphInclHomologyBuilder(SimpleProjection<HomologyTraits>::Map, 0);
        //FilteredHomologyMapBuilder<HomologyTraits> domainInclHomologyBuilder(SimpleProjection<HomologyTraits>::impl, 1);

        CRef<ComplexType> complexDomain, complexDomainNext, complexGraph, complexGraphNext;
        CRef<FiltrType> homFiltrDomainCR, homFiltrDomainNextCR, homFiltrGraphCR, homFiltrGraphNextCR;

        bool graphIsConnected = false;
        while (supplier->Next() && !graphIsConnected)
        {
            std::cout<<"-";
            
            supplier->GetDomainComplex(complexDomain);
            supplier->GetGraphComplex(complexGraph);

            if (supplier->IsFirst())
            {
                homFiltrGraphCR = HomologyHelpers<HomologyTraits>::CreateFiltr(complexGraph(), useShaving, useCoreductions, restore);
                homFiltrDomainCR = HomologyHelpers<HomologyTraits>::CreateFiltr(complexDomain(), useShaving, useCoreductions, restore);
            }
            else
            {
                homFiltrGraphCR = homFiltrGraphNextCR;
                homFiltrDomainCR = homFiltrDomainNextCR;
            }

            graphIsConnected = (homFiltrGraphCR().topDim() < 0 || homFiltrGraphCR().baseHomologyChains()[0].size() < 2);
            /*
            if (homFiltrGraphCR().topDim() >= 0 && homFiltrDomainCR().topDim() >= 0)
            {
                std::cout<<homFiltrGraphCR().baseHomologyChains()[0].size()<<" -> "<<homFiltrDomainCR().baseHomologyChains()[0].size()<<std::endl;
            }
             */

            if (!supplier->IsLast() && !graphIsConnected)
            {
                supplier->GetNextGraphComplex(complexGraphNext);
                supplier->GetNextDomainComplex(complexDomainNext);
                homFiltrGraphNextCR = HomologyHelpers<HomologyTraits>::CreateFiltr(complexGraphNext(), useShaving, useCoreductions, restore);
                homFiltrDomainNextCR = HomologyHelpers<HomologyTraits>::CreateFiltr(complexDomainNext(), useShaving, useCoreductions, restore);

                //SimpleProjection<HomologyTraits>::SetComplexes(complexDomain, complexDomainNext);
                //domainInclHomologyBuilder.AddFiltration(homFiltrDomainCR(), homFiltrDomainNextCR());
                SimpleProjection<HomologyTraits>::SetComplexes(complexGraph, complexGraphNext);
                graphInclHomologyBuilder.AddFiltration(homFiltrGraphCR(), homFiltrGraphNextCR());
            }

            SimpleProjection<HomologyTraits>::SetComplexes(complexGraph, complexDomain);
            projHomologyBuilder.AddFiltration(homFiltrGraphCR(), homFiltrDomainCR());
        }

        std::vector<MatrixType> A;
        std::vector<CRef<std::vector<MatrixType> > > projections = projHomologyBuilder.GetMatrices();
        std::vector<MatrixType> kernels;
        for (int i = 0; i < projections.size(); i++)
        {
            //std::cout << "projection for epsilon[" << i << "] = " << epsilons[i] << std::endl;
            //std::cout << projections[i]()[0] << std::endl;
            //std::cout<<projections[i]()[0].numberOfColumns()<<" -> "<<projections[i]()[0].numberOfRows()<<std::endl;

            MatrixType kernel, image;
            kernelImage(projections[i]()[0], kernel, image);
            //std::cout << "kernel " << std::endl << kernel << std::endl;
            //std::cout << "image " << std::endl << image << std::endl;
            kernels.push_back(kernel);
        }

        std::vector<CRef<std::vector<MatrixType> > > graphInclusions = graphInclHomologyBuilder.GetMatrices();
        for (int i = 0; i < graphInclusions.size(); i++)
        {
            //std::cout << "graph inclusion : " << epsilons[i] << " -> " << epsilons[i + 1] << std::endl;
            MatrixType kerX = graphInclusions[i]()[0] * kernels[i];
            MatrixType kerY = kernels[i + 1];
            MatrixType t(kerY.numberOfColumns(), kerX.numberOfColumns());
            //std::cout << "solving for " << kerY << std::endl;
            for (int c = 0; c < kerX.numberOfColumns(); c++)
            {
                VectorType b = kerX.column(c);
                //std::cout << "b = " << b << std::endl;
                VectorType x;
                solveLinearEquation(kerY, b, x);
                //std::cout << "x = " << x << std::endl;
                for (int d = 0; d < x.dimension(); d++)
                {
                    t(d + 1, c + 1) = x(d + 1);
                }
            }
            // std::cout << "ker inc " << t << std::endl;
            A.push_back(t);
        }
        return CRef<PersistenceDgmType>(new PersistenceDgmType(A));
    }

    static CRef<PersistenceDgmType> Compute(Domain *domain, Map *map, Metric *domainMetric, Metric *graphMetric, const std::vector<double> &epsilons)
    {
        MapSupplier<HomologyTraits> supplier(domain, map, domainMetric, graphMetric, epsilons);
        return KernelsPersistenceT::Compute(&supplier);
    }
};

#endif	/* KERNELSPERSISTENCE_HPP */

