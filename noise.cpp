// pbrendel (c) 2013-21

#include "noise.h"
#include "point.h"
#include "Core/assert.h"


void Noise::AddNoise( Point &p ) const
{
    const uint dim = GetDimension();
    assert( p.GetDimension() == dim );
    for ( uint i = 0; i < dim; ++i )
    {
        p[i] += ( ( (double)rand() / RAND_MAX ) - 0.5 ) * 2.0 * m_deltas[i];
    }
}
