/* 
 * File:   Noise.hpp
 * Author: Piotr Brendel
 */

#ifndef NOISE_HPP
#define	NOISE_HPP

#include <cstdlib>
#include <vector>
#include <cassert>

class Noise
{
    int dim;
    std::vector<double> deltas;

public:

    Noise(int dim, const std::vector<double> &deltas)
    {
        assert(deltas.size() == dim);
        this->dim = dim;
        this->deltas.assign(deltas.begin(), deltas.end());
    }

    int GetDimension()
    {
        return dim;
    }

    void AddNoise(Point &p)
    {
        assert(p.size() == dim);
        for (int i = 0; i < dim; i++)
        {
            p[i] += (((double)rand() / RAND_MAX) - 0.5) * 2.0 * deltas[i];
        }
    }

    void Print(int count)
    {
        for (int i = 0; i < count; i++)
        {
            Point p(dim);
            AddNoise(p);
            for (Point::iterator v = p.begin(); v != p.end(); v++)
            {
                std::cout<<*v<<" ";
            }
            std::cout<<std::endl;
        }
    }
};

#endif	/* NOISE_HPP */

