/* 
 * File:   HomologyHelpers.hpp
 * Author: Piotr Brendel
 */

#ifndef HOMOLOGYHELPERS_HPP
#define	HOMOLOGYHELPERS_HPP

#include <vector>

template <typename HomologyTraits>
class HomologyHelpers
{

public:
    
    typedef typename HomologyTraits::ComplexType ComplexType;
    typedef typename HomologyTraits::FiltrType FiltrType;
    typedef typename HomologyTraits::ScalarType ScalarType;

    typedef typename ShaveAlgorithmFactory<ComplexType>::DefaultAlgorithm ShaveAlgorithm;
    typedef typename CoreductionAlgorithmFactory<ComplexType, ScalarType>::DefaultAlgorithm CoreductionAlgorithm;
    
    static CRef<FiltrType> CreateFiltr(ComplexType &complex, bool useShaving, bool useCoreductions, bool restore)
    {
        typename FiltrType::ReducedCells reducedCellsS, reducedCellsC;
        typename FiltrType::ExtractedCells extractedCells;

        if (useShaving)
        {
            boost::shared_ptr<ShaveAlgorithm> salgorithm = ShaveAlgorithmFactory<ComplexType>::createDefault(complex);
            salgorithm->setStoreReducedCells(true);
            (*salgorithm)();
            reducedCellsS = salgorithm->getReducedCells();
        }

        if (useCoreductions)
        {
            boost::shared_ptr<CoreductionAlgorithm> algorithm = CoreductionAlgorithmFactory<ComplexType, ScalarType>::createDefault(complex);
            algorithm->setStoreReducedCells(true);
            (*algorithm)();
            extractedCells = algorithm->getExtractedCells();
            reducedCellsC = algorithm->getReducedCells();
        }

        CRef<FiltrType> homFiltr(new FiltrType(complex, extractedCells, reducedCellsS, reducedCellsC));
        homFiltr()(true, restore);
        return homFiltr;
    }

    template <typename RipsComplexType>
    static CRef<FiltrType> CreateFiltr(RipsComplexType &ripsComplex, bool useShaving, bool useCoreductions, bool restore)
    {
        ComplexType complex;
        ripsComplex.GetComplex(complex);
        return CreateFiltr(complex, useShaving, useCoreductions, restore);
    }

    static std::vector<int> GetBettiNumbers(CRef<FiltrType> filtr)
    {
        std::vector<int> bettiNumbers;
        for (int i = 0; i <= filtr().topDim(); i++)
        {
            bettiNumbers.push_back(filtr().baseHomologyChains()[i].size());
        }
        return bettiNumbers;
    }

    static std::vector<int> GetBettiNumbers(ComplexType &complex)
    {
        return GetBettiNumbers(CreateFiltr(complex, true, true, false));
    }

    template <typename RipsComplexType>
    static std::vector<int> GetBettiNumbers(RipsComplexType &ripsComplex)
    {
        ComplexType complex;
        ripsComplex.GetComplex(complex);
        return GetBettiNumbers(complex);
    }
};

#endif	/* HOMOLOGYHELPERS_HPP */

