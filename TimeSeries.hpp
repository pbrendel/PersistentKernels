/* 
 * File:   TimeSeries.hpp
 * Author: Piotr Brendel
 */

#ifndef TIMESERIES_HPP
#define	TIMESERIES_HPP

#include <list>

template <typename Map>
class TimeSeries
{
    Map *map;
    int dim;

    std::list<double> values;

public:

    TimeSeries(Map *map, int dim)
    {
        this->map = map;
        this->dim = dim * 2;
    }

    std::vector<double> First(double rootX, double rootY)
    {
        double x = rootX;
        double y = rootY;
        while (values.size() < dim)
        {
            values.push_back(x);
            values.push_back(y);
            map->GetValue(x, y);
        }
        std::vector<double> point(values.begin(), values.end());
        return point;
    }

    std::vector<double> Next()
    {
        double x = values.front();
        values.pop_front();
        double y = values.front();
        values.pop_front();
        map->GetValue(x, y);
        values.push_back(x);
        values.push_back(y);
        std::vector<double> point(values.begin(), values.end());
        return point;
    }
};


#endif	/* TIMESERIES_HPP */

