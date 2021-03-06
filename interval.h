// pbrendel (c) 2013-21

#pragma once

#include "Core/assert.h"


struct Interval
{
    double m_min;
    double m_max;

    Interval()
        : m_min( 0 )
        , m_max( 0 )
    {}

    Interval( double min, double max )
        : m_min( min )
        , m_max( max )
    {
        assert( max >= min );
    }

    constexpr double GetLength() const
    {
        return m_max - m_min;
    }

    constexpr double GetCenter() const
    {
        return ( m_min + m_max ) * 0.5;
    }

    constexpr bool IsInside( double p ) const
    {
        return p >= m_min && p <= m_max;
    }
};
