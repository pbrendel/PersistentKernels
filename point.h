// pbrendel (c) 2013-21

#pragma once

#include "Core/dynArray.h"


class Point
{
public:

	typedef o::DynArray<double> Data;
	typedef typename Data::Iterator Iterator;
	typedef typename Data::ConstIterator ConstIterator;

	Point()
	{}

	Point( uint dim )
		: m_data( dim )
	{}

	Point( const Point &other )
		: m_data( other.m_data )
	{}

	Point( Point &&other )
		: m_data( std::move( other.m_data) )
	{}

	Point &operator=( const Point &other )
	{
		m_data = other.m_data;
		return *this;
	}

	Point &operator=( Point &&other )
	{
		m_data = std::move( other.m_data );
		return *this;
	}

	void Clear()
	{
		std::fill( Begin(), End(), 0.0 );
	}

	void Resize( uint n )
	{
		m_data.Resize( n );
	}

	uint GetDimension() const
	{
		return m_data.GetSize();
	}

	double &operator[]( uint index )
	{
		return m_data[index];
	}

	const double &operator[]( uint index ) const
	{
		return m_data[index];
	}

	Iterator Begin()
	{
		return m_data.Begin();
	}

	Iterator End()
	{
		return m_data.End();
	}

	ConstIterator Begin() const
	{
		return m_data.Begin();
	}

	ConstIterator End() const
	{
		return m_data.End();
	}

	bool operator==( const Point &other ) const;

private:

	Data m_data;
};


typedef o::DynArray<Point> PointsList;