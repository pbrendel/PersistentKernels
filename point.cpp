// pbrendel (c) 2013-21

#include "point.h"


bool Point::operator==( const Point &other ) const
{
	if ( GetDimension() != other.GetDimension() )
	{
		return false;
	}
	return std::equal( Begin(), End(), other.Begin() );
}
