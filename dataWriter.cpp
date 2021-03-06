// pbrendel (c) 2021

#include "dataWriter.h"
#include "domain.h"
#include "horseshoeMap.h"
#include "map.h"
#include "noise.h"
#include "persistenceData.h"
#include "ripsComplex.h"
#include "simplexSet.h"
#include "Core/defs.h"

using o::DynArray;


class DataWriter
{
public:

    static void Write( std::ostream &str, const HorseshoeExitSetQuotientMetrics &metrics )
    {
        const uint thresholdsCount = metrics.m_thresholds.GetSize();
        str << "tresholds:" << std::endl;
        for ( uint i = 0; i < thresholdsCount; ++i )
        {
            str << metrics.m_thresholds[i] << std::endl;
        }
        str << "exit set:" << std::endl;
        for ( uint i = 0; i < metrics.m_exitSetIndices.GetSize(); ++i )
        {
            str << "[" << metrics.m_thresholds[metrics.m_exitSetIndices[i]] << ", " << metrics.m_thresholds[metrics.m_exitSetIndices[i] + 1] << "]" << std::endl;
        }
        str << "prev and next exit sets:" << std::endl;
        o::Map<uint, uint>::ConstIterator itPrev = metrics.m_prevExitSet.Begin();
        o::Map<uint, uint>::ConstIterator itNext = metrics.m_nextExitSet.Begin();
        while ( itPrev != metrics.m_prevExitSet.End() )
        {
            str << itPrev->first << " " << itPrev->second << " " << itNext->second << std::endl;
            itPrev++;
            itNext++;
        }
        if ( metrics.m_xExitSetIndex != O_INVALID_INDEX )
        {
            str << "x exit set index: " << metrics.m_xExitSetIndex << " value " << metrics.m_xExitSetValue << " invert: " << metrics.m_xExitSetInvert << std::endl;
        }
    }

    static void Write( std::ostream &str, const PersistenceDiagram &persistenceDiagram )
    {
        for ( DynArray<PersistenceDiagram::HomologyClass>::ConstIterator it = persistenceDiagram.m_homologyClasses.Begin();
              it != persistenceDiagram.m_homologyClasses.End(); ++it )
        {
            str << it->m_column << ": " << it->m_birth << " " << it->m_death << " " << std::endl;
        }
    }

    static void Write( std::ostream &str, const PointPersistenceData& persistenceData )
    {
        str << persistenceData.m_argument << persistenceData.m_value;
        str << persistenceData.m_quality<< " " << persistenceData.m_discontinuity << std::endl;
    }

    static void Write( std::ostream &str, const RipsComplex &rips )
    {
        const RipsComplex::Vertex *verts = rips.m_verts.Get();
        for ( uint i = 0; i < rips.m_vertsCount; ++i )
        {
            str << verts[i].m_label << " = " << *verts[i].m_point << std::endl;
        }
        const uint edgesCount = rips.m_edges.GetSize();
        for ( uint i = 0; i < edgesCount; ++i )
        {
            ConstSimplex e = rips.m_edges[i];
            str << "[" << e << "]" << std::endl;
        }
        str << "connected components representatives:" << std::endl;
        for ( DynArray<uint>::ConstIterator it = rips.m_ccRepresentative.Begin(); it != rips.m_ccRepresentative.End(); ++it )
        {
            str << *it << std::endl;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<( std::ostream &str, const ConstSimplex &simplex )
{
    const uint dim = simplex.GetDimension();
    for ( uint i = 0; i < dim; ++i )
    {
        str << simplex[i] << " ";
    }
    return str;
}


std::ostream &operator<<( std::ostream &str, const Domain &domain )
{
    Point point( domain.GetDimension() );
    const uint count = domain.GetCount();
    for ( uint i = 0; i < count; ++i )
    {
        domain.GetValue( i, point );
        str<< point << std::endl;
    }
    return str;
}


std::ostream &operator<<( std::ostream &str, const HorseshoeExitSetQuotientMetrics &metrics )
{
    DataWriter::Write( str, metrics );
    return str;
}


std::ostream &operator<<( std::ostream &str, const MapWriter &wr )
{
    Point d( wr.m_domain.GetDimension() );
    Point r( wr.m_map.GetDimension() );
    const uint count = wr.m_domain.GetCount();
    for ( uint i = 0; i < count; ++i )
    {
        wr.m_domain.GetValue( i, d );
        for ( Point::Iterator v = d.Begin(); v != d.End(); ++v )
        {
            str << *v << " ";
        }
        wr.m_map.GetValue( d, r );
        for ( Point::Iterator v = r.Begin(); v != r.End(); ++v )
        {
            str << *v << " ";
        }
        str << std::endl;
    }
    return str;
}


std::ostream &operator<<( std::ostream &str, const MapValuesWriter &wr )
{
    Point d( wr.m_domain.GetDimension() );
    Point r( wr.m_map.GetDimension() );
    const uint count = wr.m_domain.GetCount();
    for ( uint i = 0; i < count; ++i )
    {
        wr.m_domain.GetValue( i, d );
        wr.m_map.GetValue( d, r );
        for ( Point::Iterator v = r.Begin(); v != r.End(); ++v )
        {
            str << *v << " ";
        }
        str << std::endl;
    }
    return str;
}


std::ostream &operator<<( std::ostream &str, const Noise &noise )
{
    constexpr uint count = 10;
    for ( uint i = 0; i < count; ++i )
    {
        Point point( noise.GetDimension() );
        point.Clear();
        noise.AddNoise( point );
        str << point << std::endl;
    }
    return str;
}

std::ostream &operator<<( std::ostream &str, const PersistenceDiagram &persistenceDiagram )
{
    DataWriter::Write( str, persistenceDiagram );
    return str;
}

std::ostream &operator<<( std::ostream &str, const Point &point )
{
    for ( Point::ConstIterator v = point.Begin(); v != point.End(); ++v )
    {
        str << *v << " ";
    }
    return str;
}


std::ostream &operator<<( std::ostream &str, const PointPersistenceData &persistenceData )
{
    DataWriter::Write( str, persistenceData );
    return str;
}


std::ostream &operator<<( std::ostream &str, const RipsComplex &rips )
{
    DataWriter::Write( str, rips );
    return str;
}
