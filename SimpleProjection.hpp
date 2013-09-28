/* 
 * File:   SimpleProjection.hpp
 * Author: Piotr Brendel
 */

#ifndef SIMPLEPROJECTION_HPP
#define	SIMPLEPROJECTION_HPP

#include <capd/auxil/CRef.h>

template <typename HomologyTraits>
class SimpleProjection
{
public:

    typedef typename HomologyTraits::ComplexType ComplexType;
    typedef typename HomologyTraits::GeneratorType GeneratorType;
    typedef typename HomologyTraits::ScalarType ScalarType;

    static void SetComplexes(CRef<ComplexType> _domComplex, CRef<ComplexType> _rngComplex)
    {
        domComplex = _domComplex;
        rngComplex = _rngComplex;
    }

    static GeneratorType Map(GeneratorType domGen, ScalarType &orientation)
    {
        typename ComplexType::Simplex* s = domComplex().getSimplexById(domGen.getId());
        std::set<int> rngSimp;
        for (int i = 0; i < s->nrs.size(); ++i)
        {
            rngSimp.insert(s->nrs[i]);
        }
        orientation = ScalarType(1);
        return GeneratorType(rngComplex().getSimplex(rngSimp));
    }

private:

    static CRef<ComplexType> domComplex;
    static CRef<ComplexType> rngComplex;

};

template <typename HomologyTraits>
CRef<typename HomologyTraits::ComplexType> SimpleProjection<HomologyTraits>::domComplex;
template <typename HomologyTraits>
CRef<typename HomologyTraits::ComplexType> SimpleProjection<HomologyTraits>::rngComplex;

#endif	/* SIMPLEPROJECTION_HPP */

