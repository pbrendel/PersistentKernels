/* 
 * File:   HomologyTraits.hpp
 * Author: Piotr Brendel
 */

#ifndef HOMOLOGYTRAITS_HPP
#define	HOMOLOGYTRAITS_HPP

#include <capd/vectalg/Matrix.hpp>
#include <capd/vectalg/Vector.hpp>

#include <capd/homAlgebra/FreeModule.h>
#include <capd/homAlgebra/FreeChainComplex.h>
#include <capd/homAlgebra/QuotientGradedModule.h>
#include <capd/redAlg/ReducibleFreeChainComplex.h>

#include <redHom/algorithm/Algorithms.hpp>
//#include <capd/matrixAlgorithms/intMatrixAlgorithms.hpp>

#include "capd/homAlgebra/ChainContainer.h"
#include <capd/homAlgebra/ChainT.h>

// SComplexFiltrT needs this
#include <boost/foreach.hpp>

#include <redHom/complex/simplicial/SimplexSComplex.hpp>
#include <capd/RedHom/SComplexFiltrT.hpp>

//int Zp::p = 1009;
//int Zp::p = 2;

class HomologyTraits
{
public:

    typedef SimplexSComplex ComplexType;
    typedef SimplexSComplex::Cell GeneratorType;
    typedef double ScalarType;
    typedef SComplexFiltrT<ComplexType, ScalarType> FiltrType;

    typedef capd::vectalg::Matrix<ScalarType, 0, 0> MatrixType;
    typedef capd::vectalg::Vector<ScalarType, 0> VectorType;
    typedef ChainT<ChainContainer<GeneratorType, ScalarType> > ChainType;
};

#endif	/* HOMOLOGYTRAITS_HPP */

