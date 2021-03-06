// pbrendel (c) 2013-21

#include "persistenceData.h"
#include "map.h"
#include "qualityFunction.h"
#include "Core/set.h"

using o::DynArray;


PersistenceDiagram::PersistenceDiagram( PersistenceDiagram &&other )
    : m_homologyClasses( std::move( other.m_homologyClasses ) )
{
}


PersistenceDiagram::PersistenceDiagram( const ProjectionsList &projections )
{
    typedef o::Set<uint> Range;
    typedef DynArray<uint> Kernel;
    DynArray<Kernel> kernels;

    for ( ProjectionsList::ConstIterator projection = projections.Begin(); projection != projections.End(); ++projection )
    {
        Range range;
        Kernel kernel;
        for ( Projection::ConstIterator p = projection->Begin(); p != projection->End(); ++p )
        {
            // if more than one graph generator was projected to the same domain generator then the generator is in kernel
            if ( range.Contains( p->second ) )
            {
                kernel.PushBack( p->first );
            }
            else
            {
                range.Insert( p->second );
            }
        }
        kernels.PushBack( kernel );
    }
    typedef o::Map<uint, uint> GeneratorsMap;
    GeneratorsMap generatorBirth;
    GeneratorsMap generatorDuration;
    const uint kernelsCount = kernels.GetSize();
    for ( uint i = 0; i < kernelsCount; ++i )
    {
        const Kernel &kernel = kernels[i];
        for ( Kernel::ConstIterator k = kernel.Begin(); k != kernel.End(); ++k )
        {
            if ( generatorDuration[*k] == 0 )
            {
                generatorDuration[*k] = 1;
                generatorBirth[*k] = i;
            }
            else
            {
                generatorDuration[*k]++;
            }
        }
    }
    uint column = 0;
    for ( GeneratorsMap::ConstIterator it = generatorBirth.Begin(); it != generatorBirth.End(); ++it )
    {
        const uint birth = it->second;
        const uint death = birth + generatorDuration[it->first];
        m_homologyClasses.PushBack( HomologyClass( column++, birth, death ) );
    }
}

////////////////////////////////////////////////////////////////////////////////

PointPersistenceData::PointPersistenceData( PointPersistenceData &&other )
    : m_argument( std::move( other.m_argument ) )
    , m_value( std::move( other.m_value ) )
    , m_persistenceDiagram( std::move( other.m_persistenceDiagram ) )
    , m_quality( other.m_quality )
    , m_discontinuity( other.m_discontinuity )
{
}


PointPersistenceData::PointPersistenceData( const Point &argument, const ProjectionsList &projections, bool discontinuity )
    : m_argument( argument )
    , m_discontinuity( discontinuity )
{
    m_persistenceDiagram = new PersistenceDiagram( projections );
}


void PointPersistenceData::ApplyMap( const Map &map )
{
    m_value.Resize( map.GetDimension() );
    map.GetValue( m_argument, m_value );
}


void PointPersistenceData::CalculateQuality( const QualityFunction &qualityFunction )
{
    m_quality = qualityFunction.Calculate( *m_persistenceDiagram );
}
