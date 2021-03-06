// pbrendel (c) 2013-21

#pragma once

#include "interval.h"
#include "Core/dynArray.h"

class Point;


class Cube
{
public:

	void Clear()
	{
		m_intervals.Clear();
	}

    const uint GetDimension() const
    {
        return m_intervals.GetSize();
    }

	void SetDimension( uint dim )
	{
		m_intervals.Resize( dim );
	}

	void AddDimension( const Interval &interval )
	{
		m_intervals.PushBack( interval );
	}

	Interval &operator[]( uint index )
	{
		return m_intervals[index];
	}

	const Interval &operator[]( uint index ) const
	{
		return m_intervals[index];
	}

    bool IsInside( const Point &p ) const;

private:

    o::DynArray<Interval> m_intervals;
};
