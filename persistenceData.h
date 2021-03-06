// pbrendel (c) 2013-21

#pragma once

#include "point.h"
#include "Core/dynArray.h"
#include "Core/map.h"
#include "Core/ptr.h"

class Map;
class QualityFunction;

typedef o::Map<uint, uint> Projection;
typedef o::DynArray<Projection> ProjectionsList;


class PersistenceDiagram
{
public:

    struct HomologyClass
    {
        uint m_column;
        uint m_birth;
        uint m_death;

        HomologyClass( uint column, uint birth, uint death )
            : m_column( column )
            , m_birth( birth )
            , m_death( death )
        {}
    };

    typedef o::DynArray<HomologyClass>::ConstIterator Iterator;

    PersistenceDiagram( PersistenceDiagram &&other );
    PersistenceDiagram( const ProjectionsList &projections );

    Iterator Begin() const { return m_homologyClasses.Begin(); }
    Iterator End() const { return m_homologyClasses.End(); }

private:

    o::DynArray<HomologyClass> m_homologyClasses;

    friend class DataWriter;
};

////////////////////////////////////////////////////////////////////////////////

class PointPersistenceData
{
public:

    PointPersistenceData( PointPersistenceData &&other );
    PointPersistenceData( const Point &argument, const ProjectionsList &projections, bool discontinuity );

    const PersistenceDiagram &GetPersistenceDiagram() const
    {
        return *m_persistenceDiagram;
    }

    void ApplyMap( const Map &map );
    void CalculateQuality( const QualityFunction &qualityFunction );

private:

    Point m_argument;
    Point m_value;
    o::Ptr<PersistenceDiagram> m_persistenceDiagram;
    double m_quality;
    bool m_discontinuity;

    friend class DataWriter;
};

////////////////////////////////////////////////////////////////////////////////

typedef o::DynArray<PointPersistenceData> PersistenceData;
