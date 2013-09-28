/* 
 * File:   TimeSeriesSupplier.hpp
 * Author: Piotr Brendel
 */

#ifndef TIMESERIESSUPPLIER_HPP
#define	TIMESERIESSUPPLIER_HPP

#include "Supplier.h"
#include "Rips/rips.hpp"

//test
#include <iostream>

template <typename TimeSeries>
class TimeSeriesSupplier : public Supplier
{
    vector<rips::euclideanPoint> pointsA;
    vector<rips::euclideanPoint> pointsGraph;
    vector<double> epsilons;

    int maxDimension;
    CRef<SimplexSComplex> complexCR;
    CRef<SimplexSComplex> nextComplexCR;
    CRef<SimplexSComplex> graphComplexCR;
    CRef<SimplexSComplex> nextGraphComplexCR;

    int current;

public:

    TimeSeriesSupplier(TimeSeries *timeSeries, int pointsCount, std::vector<double> &epsilons, int maxDimension)
    {
        rips::euclideanPoint p(timeSeries->First(0.2, 0.2));
        for (int i = 0; i < pointsCount; i++)
        {
            rips::euclideanPoint q(timeSeries->Next());
            rips::euclideanPoint g(p, q);
            pointsA.push_back(p);
            pointsGraph.push_back(g);
            p = q;
        }
        this->epsilons = epsilons;
        this->maxDimension = maxDimension;
        current = -1;
    }

    inline void getQProj(vector<int>& qProjV)
    {
        std::vector<int> q;
        int last = pointsA.size() - 1;
        for (int i = 0; i < last; i++)
        {
            q.push_back(i + 1);
        }
        q.push_back(last);
        qProjV = q;
    }

    bool getGraphComplex(CRef<SimplexSComplex>& graphComplexCR)
    {
        graphComplexCR = this->graphComplexCR;
    }

    bool getNextGraphComplex(CRef<SimplexSComplex>& nextGraphComplexCR)
    {
        nextGraphComplexCR = this->nextGraphComplexCR;
    }

    void getComplex(CRef<SimplexSComplex>& complexCR)
    {
        complexCR = this->complexCR;
    }

    void getNextComplex(CRef<SimplexSComplex>& nextComplexCR)
    {
        nextComplexCR = this->nextComplexCR;
    }

    bool isFirst()
    {
        return current == 0;
    }

    bool isLast()
    {
        return current == epsilons.size() - 1;
    }

    bool next()
    {
        current++;
        if (current >= epsilons.size())
        {
            return false;
        }

        std::cout<<"Processing data for epsilon["<<current<<"]="<<epsilons[current]<<endl;

        if (isFirst())
        {
            RipsComplex ripsCmplxA(pointsA, epsilons[current], maxDimension);
            complexCR = CRef<SimplexSComplex>(new SimplexSComplex());
            ripsCmplxA.outputMaxSimplices(complexCR());

            RipsComplex ripsCmplxGSub(pointsGraph, epsilons[current], maxDimension);
            graphComplexCR = CRef<SimplexSComplex>(new SimplexSComplex());
            ripsCmplxGSub.outputMaxSimplices(graphComplexCR());
        }
        else
        {
            graphComplexCR = nextGraphComplexCR;
            complexCR = nextComplexCR;
        }

        if (!isLast())
        {
            RipsComplex ripsCmplxGSup(pointsGraph, epsilons[current + 1], maxDimension);
            nextGraphComplexCR = CRef<SimplexSComplex>(new SimplexSComplex());
            ripsCmplxGSup.outputMaxSimplices(nextGraphComplexCR());

            RipsComplex ripsCmplxA(pointsA, epsilons[current + 1], maxDimension);
            nextComplexCR = CRef<SimplexSComplex>(new SimplexSComplex());
            ripsCmplxA.outputMaxSimplices(nextComplexCR());
        }

        return true;
    }
};

#endif	/* TIMESERIESSUPPLIER_HPP */

