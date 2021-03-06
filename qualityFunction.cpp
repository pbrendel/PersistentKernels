// pbrendel (c) 2013-21

#include "qualityFunction.h"


void QualityFunction1::Init( const PersistenceData &persistenceData, uint filtrationsCount )
{
    m_min = filtrationsCount;
    m_max = 0;
    for ( PersistenceData::ConstIterator i = persistenceData.Begin(); i != persistenceData.End(); ++i )
    {
        const PersistenceDiagram &diagram = i->GetPersistenceDiagram();
        uint longest = 0;
        for ( PersistenceDiagram::Iterator h = diagram.Begin(); h != diagram.End(); ++h )
        {
            const uint diff = h->m_death - h->m_birth;
            longest = std::max( longest, diff );
        }
        m_min = std::min( m_min, longest );
        m_max = std::max( m_max, longest );
    }
}


double QualityFunction1::Calculate( const PersistenceDiagram &persistenceDiagram ) const
{
    if ( m_min == m_max )
    {
        return 0.0;
    }
   uint value = m_min;
    for ( PersistenceDiagram::Iterator h = persistenceDiagram.Begin(); h != persistenceDiagram.End(); ++h )
    {
        const uint diff = h->m_death - h->m_birth;
        value = std::max( value, diff );
    }
    return static_cast<double>( value - m_min ) / static_cast<double>( m_max - m_min );
}

////////////////////////////////////////////////////////////////////////////////

void QualityFunction2::Init( const PersistenceData &persistenceData, uint filtrationsCount )
{
    m_min = filtrationsCount;
    m_max = 0;
    for ( PersistenceData::ConstIterator i = persistenceData.Begin(); i != persistenceData.End(); ++i )
    {
        const PersistenceDiagram &diagram = i->GetPersistenceDiagram();
        uint longest = 0;
        for ( PersistenceDiagram::Iterator h = diagram.Begin(); h != diagram.End(); ++h )
        {
            const uint diff = h->m_death - h->m_birth;
            longest = std::max( longest, diff );
        }
        m_min = std::min( m_min, longest );
        m_max = std::max( m_max, longest );
    }
}


double QualityFunction2::Calculate( const PersistenceDiagram &persistenceDiagram ) const
{
    if ( m_min == m_max )
    {
        return 0.0;
    }
    uint value = m_min;
    for ( PersistenceDiagram::Iterator h = persistenceDiagram.Begin(); h != persistenceDiagram.End(); ++h )
    {
        const uint diff = h->m_death - h->m_birth;
        value = std::max( value, diff );
    }
    return ( value == m_max ) ? 1.0 : 0.0;
}

////////////////////////////////////////////////////////////////////////////////

void QualityFunction3::Init( const PersistenceData &persistenceData, uint filtrationsCount )
{
    m_max = 0;
    o::Map<uint, uint> histogram;
    for ( PersistenceData::ConstIterator i = persistenceData.Begin(); i != persistenceData.End(); ++i )
    {
        const PersistenceDiagram &diagram = i->GetPersistenceDiagram();
        uint longest = 0;
        for ( PersistenceDiagram::Iterator h = diagram.Begin(); h != diagram.End(); ++h )
        {
            const uint diff = h->m_death - h->m_birth;
            longest = std::max( longest, diff );
        }
        histogram[longest] = histogram[longest] + 1;
        m_max = std::max( m_max, longest );
    }
    uint m = 0;
    for ( o::Map<uint, uint>::ConstIterator it = histogram.Begin(); it != histogram.End(); ++it )
    {
        if ( it->second > m )
        {
            m = it->second;
            m_medium = it->first;
        }
    }
}


double QualityFunction3::Calculate( const PersistenceDiagram &persistenceDiagram ) const
{
    uint value = 0;
    for ( PersistenceDiagram::Iterator h = persistenceDiagram.Begin(); h != persistenceDiagram.End(); ++h )
    {
        const uint diff = h->m_death - h->m_birth;
        value = std::max( value, diff );
    }
    if ( value <= m_medium )
    {
        return 0;
    }
    return static_cast<double>( value - m_medium ) / static_cast<double>( m_max - m_medium );
}

////////////////////////////////////////////////////////////////////////////////

void QualityFunction4::Init( const PersistenceData &persistenceData, uint filtrationsCount )
{
    m_min = filtrationsCount;
    m_max = 0;
    for ( PersistenceData::ConstIterator i = persistenceData.Begin(); i != persistenceData.End(); ++i )
    {
        const PersistenceDiagram &diagram = i->GetPersistenceDiagram();
        for ( PersistenceDiagram::Iterator h = diagram.Begin(); h != diagram.End(); ++h )
        {
            const uint diff = h->m_death - h->m_birth;
            m_min = std::min( m_min, diff );
            m_max = std::max( m_max, diff );
        }
    }
}


double QualityFunction4::Calculate( const PersistenceDiagram &persistenceDiagram ) const
{
    if ( m_min == m_max )
    {
        return 0.0;
    }
    uint value = m_min;
    for ( PersistenceDiagram::Iterator h = persistenceDiagram.Begin(); h != persistenceDiagram.End(); ++h )
    {
        const uint diff = h->m_death - h->m_birth;
        value = std::max( value, diff );
    }
    return ( value == m_max ) ? 1.0 : 0.0;
}

////////////////////////////////////////////////////////////////////////////////

void QualityFunction5::Init( const PersistenceData &persistenceData, uint filtrationsCount )
{
    m_filtrationsCount = filtrationsCount;
}


double QualityFunction5::Calculate( const PersistenceDiagram &persistenceDiagram ) const
{
    if ( m_filtrationsCount <= 0 )
    {
        return 0.0;
    }
    for ( PersistenceDiagram::Iterator h = persistenceDiagram.Begin(); h != persistenceDiagram.End(); ++h )
    {
        if ( h->m_death == m_filtrationsCount )
        {
            return 1.0;
        }
    }
    return 0.0;
}
