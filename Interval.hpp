/* 
 * File:   Interval.hpp
 * Author: Piotr Brendel
 */

#ifndef INTERVAL_HPP
#define	INTERVAL_HPP

#include <cassert>

struct Interval
{
    double min;
    double max;

    Interval() : min(0), max(0)
    {
    }

    Interval(double _min, double _max) : min(_min), max(_max)
    {
        assert(max >= min);
    }

    double GetLength() const
    {
        return max - min;
    }

    double GetCenter() const
    {
        return (min + max) * 0.5;
    }
};

#endif	/* INTERVAL_HPP */

