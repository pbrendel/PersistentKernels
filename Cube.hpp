/* 
 * File:   Cube.hpp
 * Author: Piotr Brendel
 */

#ifndef CUBE_HPP
#define	CUBE_HPP

#include "Point.hpp"
#include "Interval.hpp"

class Cube : public std::vector<Interval>
{
public:

    bool IsInside(const Point &p)
    {
        if (size() != p.size())
        {
            return false;
        }
        for (int i = 0; i < size(); i++)
        {
            if (p[i] < at(i).min || p[i] > at(i).max)
            {
                return false;
            }
        }
        return true;
    }

};

#endif	/* CUBE_HPP */

