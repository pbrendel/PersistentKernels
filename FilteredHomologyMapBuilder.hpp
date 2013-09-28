/* 
 * File:   FilteredHomologyMapBuilder.hpp
 * Author: Piotr Brendel
 */

#ifndef FILTEREDHOMOLOGYMAPBUILDER_HPP
#define	FILTEREDHOMOLOGYMAPBUILDER_HPP

#include <capd/homAlgebra/ProjChainMap.hpp>
#include "SComplexChainMapCrHom.hpp"

template<typename HomologyTraits>
class FilteredHomologyMapBuilder
{
public:

    typedef typename HomologyTraits::ComplexType ComplexType;
    typedef typename HomologyTraits::GeneratorType GeneratorType;
    typedef typename HomologyTraits::FiltrType FiltrType;
    typedef typename HomologyTraits::ScalarType ScalarType;
    typedef typename HomologyTraits::MatrixType MatrixType;
    typedef typename HomologyTraits::ChainType ChainType;

    typedef ProjChainMap<ChainType, ChainType> ProjChainMapType;
    typedef GeneratorType(*OrientedProjMapType)(GeneratorType, ScalarType&);
    typedef SComplexChainMapCrHom<ComplexType, ComplexType, ScalarType, ProjChainMapType> ChainMapCrHom;

    FilteredHomologyMapBuilder(const OrientedProjMapType& projMap, int _maxDim = 99)
        : maxDim(_maxDim), chainMap(ProjChainMapType(projMap)), cmh(ChainMapCrHom(chainMap))
    {
    }

    void AddFiltration(const FiltrType &domainFiltr, const FiltrType &rangeFiltr)
    {
        CRef<std::vector<MatrixType> > gradedMatrix = cmh.getHomMatrix(domainFiltr, rangeFiltr, maxDim);
        FillGradation(gradedMatrix);
        filteredHom.push_back(gradedMatrix);
    }

    vector<CRef<vector<MatrixType> > > GetMatrices()
    {
        return filteredHom;
    }

private:

    vector<CRef<vector<MatrixType> > > filteredHom;
    ProjChainMapType chainMap;
    ChainMapCrHom cmh;
    int maxDim;

    void FillGradation(CRef<vector<MatrixType> > &gradedMatrix)
    {
        while (gradedMatrix().size() <= maxDim)
        {
            gradedMatrix().push_back(MatrixType(0, 0));
        }
    }
};

#endif	/* FILTEREDHOMOLOGYMAPBUILDER_HPP */

