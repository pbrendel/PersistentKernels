// pbrendel (c) 2013-21

#pragma once

#include "cube.h"
#include "map.h"
#include "metrics.h"
#include "Core/map.h"
#include "Core/ptr.h"

class Domain;
struct Interval;


class HorseshoeExitSetQuotientMetrics : public Metrics
{
public:

    HorseshoeExitSetQuotientMetrics( const Interval &yInterval, double exitMargin, uint piecesCount );

    virtual double GetDistance( const Point &x, const Point &y, uint, uint ) const override;
 
    void AddPieceToExitSet( uint pieceIndex );
    void RemovePieceFromExitSet( uint pieceIndex );
    void SetXExitSet( uint index, double progress, bool isInvert, const Interval &xInterval );
    bool IsInExitSet( const Point &p ) const;
    double GetDistanceToExitSet( const Point &p ) const;

    void SetIsTrivial( bool trivial )
    {
        m_isTrivial = trivial;
    }
        
private:

    void UpdatePrevNextExitSet();
    uint GetPieceIndex( double y ) const;
    bool IsExitSetIndex( uint pieceIndex ) const;
    bool IsInExitSet( const Point &p, uint pieceIndex ) const;
    double GetDistanceToExitSet( const Point &p, uint pieceIndex ) const;

    o::DynArray<double> m_thresholds;
    o::DynArray<uint> m_exitSetIndices;
    o::Map<uint, uint> m_prevExitSet;
    o::Map<uint, uint> m_nextExitSet;

    uint m_xExitSetIndex;
    double m_xExitSetValue;
    bool m_xExitSetInvert;

    bool m_isTrivial;

    friend class DataWriter;
};

////////////////////////////////////////////////////////////////////////////////

class HorseshoeMap : public Map
{
public:

    HorseshoeMap( const Noise *noise )
        : Map( 2, noise )
    {}

    const Metrics &GetExitSetQuotientMetrics() const
    {
        return *m_exitSetQuotientMetrics;
    }

protected:

    o::Ptr<HorseshoeExitSetQuotientMetrics> m_exitSetQuotientMetrics;
};

////////////////////////////////////////////////////////////////////////////////

class HorseshoeU : public HorseshoeMap
{
public:

    HorseshoeU( double exitMargin, double enterMargin, const Domain &domain, double offset, const Noise *noise );
    
    virtual void GetValue( const Point &d, Point &r ) const override;

private:

    void SetupOffset( double enterMargin, double sizeX );
    void Shrink( double dx, double dy, double &rx, double &ry ) const;
    void Stretch( double dx, double dy, double &rx, double &ry ) const;
    void Fold( double dx, double dy, double &rx, double &ry ) const;

    Cube m_cube;

    double m_shrinkCenter;
    double m_shrinkFactor;
    double m_shrinkOffset;

    double m_stretchCenter;
    double m_stretchFactor;
    double m_stretchOffset;

    double m_piece2Start;
    double m_piece3Start;

    double m_foldFactor;
    double m_foldCenterX;
    double m_foldCenterY;

    double m_offset;
};

////////////////////////////////////////////////////////////////////////////////

class HorseshoeS : public HorseshoeMap
{
public:

    HorseshoeS( double exitMargin, double enterMargin, const Domain &domain, double offset, const Noise *noise );

    virtual void GetValue( const Point &d, Point &r ) const override;

private:

    void SetupOffset( double enterMargin, double sizeX );
    void Shrink( double dx, double dy, double &rx, double &ry ) const;
    void Stretch( double dx, double dy, double &rx, double &ry ) const;
    void Fold( double dx, double dy, double &rx, double &ry ) const;

    Cube m_cube;

    double m_shrinkCenter;
    double m_shrinkFactor;
    double m_shrinkOffset;

    double m_stretchCenter;
    double m_stretchFactor;
    double m_stretchOffset;

    double m_piece2Start;
    double m_piece3Start;
    double m_piece4Start;
    double m_piece5Start;

    double m_piece1Center;
    double m_piece3Center;
    double m_piece5Center;

    double m_foldFactor;
    double m_foldUpCenterX;
    double m_foldUpCenterY;
    double m_foldDownCenterX;
    double m_foldDownCenterY;

    double m_offset;
};

////////////////////////////////////////////////////////////////////////////////

class HorseshoeG : public HorseshoeMap
{
public:

    HorseshoeG( double exitMargin, double enterMargin, const Domain &domain, double offset, const Noise *noise );

    virtual void GetValue( const Point &d, Point &r ) const override;

private:

    void SetupOffset( double enterMargin, double sizeX );
    void Shrink( double dx, double dy, double &rx, double &ry ) const;
    void Stretch( double dx, double dy, double &rx, double &ry ) const;
    void Fold( double dx, double dy, double &rx, double &ry ) const;

    Cube m_cube;

    double m_shrinkCenter;
    double m_shrinkFactor;
    double m_shrinkOffset;

    double m_stretchCenter;
    double m_stretchFactor;
    double m_stretchOffset;

    double m_piece2Start;
    double m_piece3Start;
    double m_piece4Start;
    double m_piece5Start;

    double m_piece1Center;
    double m_piece3Center;
    double m_piece5Center;

    double m_foldFactor;
    double m_foldUpCenterX;
    double m_foldUpCenterY;
    double m_foldDownCenterX;
    double m_foldDownCenterY;

    double m_offset;
};
