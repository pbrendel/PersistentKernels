// pbrendel (c) 2013-21

#include "cube.h"
#include "point.h"


bool Cube::IsInside( const Point &p ) const
{
    if ( m_intervals.GetSize() != p.GetDimension() )
    {
        return false;
    }
    uint d = 0;
    for ( const Interval &interval : m_intervals )
    {
        if ( !interval.IsInside( p[d++] ) )
        {
            return false;
        }
    }
    return true;
}
