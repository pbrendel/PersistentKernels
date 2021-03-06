// pbrendel (c) 2021

#pragma once

#include <ostream>

class ConstSimplex;
class Domain;
class HorseshoeExitSetQuotientMetrics;
class Map;
class Noise;
class PersistenceDiagram;
class Point;
class PointPersistenceData;
class RipsComplex;


struct MapWriter
{
    MapWriter( const Domain &domain, const Map &map )
        : m_domain( domain )
        , m_map( map )
    {}

    const Domain &m_domain;
    const Map &m_map;
};


struct MapValuesWriter
{
    MapValuesWriter( const Domain &domain, const Map &map )
        : m_domain( domain )
        , m_map( map )
    {}

    const Domain &m_domain;
    const Map &m_map;
};


std::ostream &operator<<( std::ostream &str, const ConstSimplex &simplex );
std::ostream &operator<<( std::ostream &str, const Domain &domain );
std::ostream &operator<<( std::ostream &str, const HorseshoeExitSetQuotientMetrics &metrics );
std::ostream &operator<<( std::ostream &str, const MapWriter &wr );
std::ostream &operator<<( std::ostream &str, const MapValuesWriter &wr );
std::ostream &operator<<( std::ostream &str, const Noise &noise );
std::ostream &operator<<( std::ostream &str, const PersistenceDiagram &persistenceDiagram );
std::ostream &operator<<( std::ostream &str, const Point &point );
std::ostream &operator<<( std::ostream &str, const PointPersistenceData &persistenceData );
std::ostream &operator<<( std::ostream &str, const RipsComplex &rips );
