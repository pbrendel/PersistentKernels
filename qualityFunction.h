// pbrendel (c) 2013-21

#pragma once

#include "Core/dynArray.h"
#include "persistenceData.h"


class QualityFunction
{
public:

    virtual ~QualityFunction()
    {}

    virtual void Init( const PersistenceData &persistenceData, uint filtrationsCount ) = 0;
    virtual double Calculate( const PersistenceDiagram &persistenceDiagram ) const = 0;
};


class QualityFunction1 : public QualityFunction
{
public:

    virtual void Init( const PersistenceData &persistenceData, uint filtrationsCount ) override;
    virtual double Calculate( const PersistenceDiagram &persistenceDiagram ) const override;

private:

    uint m_min;
    uint m_max;
};


class QualityFunction2 : public QualityFunction
{
public:

    virtual void Init( const PersistenceData &persistenceData, uint filtrationsCount ) override;
    virtual double Calculate( const PersistenceDiagram &persistenceDiagram ) const override;

private:

    uint m_min;
    uint m_max;
};


class QualityFunction3 : public QualityFunction
{
public:

    virtual void Init( const PersistenceData &persistenceData, uint filtrationsCount ) override;
    virtual double Calculate( const PersistenceDiagram &persistenceDiagram ) const override;

private:

    uint m_medium;
    uint m_max;

};

class QualityFunction4 : public QualityFunction
{
public:

    virtual void Init( const PersistenceData &persistenceData, uint filtrationsCount ) override;
    virtual double Calculate( const PersistenceDiagram &persistenceDiagram ) const override;

private:

    uint m_min;
    uint m_max;
};


class QualityFunction5 : public QualityFunction
{
public:

    virtual void Init( const PersistenceData &persistenceData, uint filtrationsCount ) override;
    virtual double Calculate( const PersistenceDiagram &persistenceDiagram ) const override;

private:

    uint m_filtrationsCount;
};
