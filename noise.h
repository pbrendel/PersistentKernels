// pbrendel (c) 2013-21

#pragma once

#include "Core/dynArray.h"

class Point;


class Noise
{
public:

    Noise( const o::DynArray<double> &deltas )
        : m_deltas( deltas )
    {}

    void AddNoise( Point &p ) const;
  
    uint GetDimension() const
    {
        return m_deltas.GetSize();
    }

private:

    o::DynArray<double> m_deltas;
};
