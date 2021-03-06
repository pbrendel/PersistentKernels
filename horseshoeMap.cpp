// pbrendel (c) 2013-21

#include "horseshoeMap.h"
#include "domain.h"
#include "noise.h"
#include "Core/defs.h"
#include "Core/math.h"

#include <algorithm>


HorseshoeExitSetQuotientMetrics::HorseshoeExitSetQuotientMetrics( const Interval &yInterval, double exitMargin, uint piecesCount )
    : m_xExitSetIndex( O_INVALID_INDEX )
    , m_xExitSetValue( 0 )
    , m_xExitSetInvert( false )
    , m_isTrivial( false )
{
    const uint thresholdsCount = piecesCount + 3;
    m_thresholds.Resize( thresholdsCount );
    double pieceDelta = yInterval.GetLength() / ( piecesCount + 2 * exitMargin );
    double marginDelta = pieceDelta * exitMargin;
    double t = yInterval.m_min;
    m_thresholds[0] = yInterval.m_min;
    t += marginDelta;
    m_thresholds[1] = t;
    for ( uint i = 0; i < piecesCount; ++i )
    {
        t += pieceDelta;
        m_thresholds[2 + i] = t;
    }
    m_thresholds[thresholdsCount - 1] = yInterval.m_max;
    for ( uint i = 0; i < piecesCount + 2; i += 2 )
    {
        m_exitSetIndices.PushBack( i );
    }
    UpdatePrevNextExitSet();
}


double HorseshoeExitSetQuotientMetrics::GetDistance( const Point &x, const Point &y, uint, uint ) const
{
    if ( m_isTrivial )
    {
        return 0.0;
    }
    assert( x.GetDimension() == 2 );
    assert( y.GetDimension() == 2 );
    const uint ix = GetPieceIndex( x[1] );
    const uint iy = GetPieceIndex( y[1] );
    if ( !IsInExitSet( x, ix ) && !IsInExitSet( y, iy ) && m_prevExitSet[ix] == m_prevExitSet[iy] /* && nextExitSet[ix] = nextExitSet[iy]*/ )
    {
        return std::min( EuclideanMetrics::Get().GetDistance( x, y, Metrics::NO_INDEX, Metrics::NO_INDEX ), GetDistanceToExitSet( x, ix ) + GetDistanceToExitSet( y, iy ) );
    }
    else
    {
        return GetDistanceToExitSet( x, ix ) + GetDistanceToExitSet( y, iy );
    }
}


void HorseshoeExitSetQuotientMetrics::AddPieceToExitSet( uint pieceIndex )
{
    m_exitSetIndices.PushBack( pieceIndex );
    std::sort( m_exitSetIndices.Begin(), m_exitSetIndices.End() );
    UpdatePrevNextExitSet();
}


void HorseshoeExitSetQuotientMetrics::RemovePieceFromExitSet( uint pieceIndex )
{
    o::DynArray<uint>::Iterator it = std::find( m_exitSetIndices.Begin(), m_exitSetIndices.End(), pieceIndex );
    if ( it != m_exitSetIndices.End() )
    {
        m_exitSetIndices.Erase( it );
    }
    UpdatePrevNextExitSet();
}


void HorseshoeExitSetQuotientMetrics::SetXExitSet( uint index, double progress, bool isInvert, const Interval &xInterval )
{
    m_xExitSetIndex = index;
    m_xExitSetInvert = isInvert;
    if ( isInvert )
    {
        m_xExitSetValue = progress * xInterval.GetLength() + xInterval.m_min;
    }
    else
    {
        m_xExitSetValue = ( 1.0 - progress ) * xInterval.GetLength() + xInterval.m_min;
    }
}


bool HorseshoeExitSetQuotientMetrics::IsInExitSet( const Point &p ) const
{
    return m_isTrivial ? true : IsInExitSet( p, GetPieceIndex( p[1] ) );
}


double HorseshoeExitSetQuotientMetrics::GetDistanceToExitSet( const Point &p ) const
{
    return m_isTrivial ? 0.0 : GetDistanceToExitSet( p, GetPieceIndex( p[1] ) );
}


void HorseshoeExitSetQuotientMetrics::UpdatePrevNextExitSet()
{
    assert( !m_exitSetIndices.IsEmpty() );
    uint first = m_exitSetIndices.Front();
    uint last = m_exitSetIndices.Back();
    m_prevExitSet.Clear();
    m_nextExitSet.Clear();
    const uint thresholdsCount = m_thresholds.GetSize();
    for ( uint i = 0; i < thresholdsCount - 1; ++i )
    {
        if ( IsExitSetIndex( i ) )
        {
            continue;
        }
        uint index = first;
        for ( uint j = i - 1; j > first; --j )
        {
            if ( IsExitSetIndex( j ) )
            {
                index = j;
                break;
            }
        }
        m_prevExitSet[i] = index;
        index = last;
        for ( uint j = i + 1; j < last; ++j )
        {
            if ( IsExitSetIndex( j ) )
            {
                index = j;
                break;
            }
        }
        m_nextExitSet[i] = index;
    }
}


uint HorseshoeExitSetQuotientMetrics::GetPieceIndex( double y ) const
{
    const uint thresholdsCount = m_thresholds.GetSize();
    for ( uint i = 0; i < thresholdsCount - 1; ++i )
    {
        if ( m_thresholds[i + 1] > y )
        {
            return i;
        }
    }
    return thresholdsCount - 2;
}


bool HorseshoeExitSetQuotientMetrics::IsExitSetIndex( uint pieceIndex ) const
{
    return m_isTrivial ? true : ( std::find( m_exitSetIndices.Begin(), m_exitSetIndices.End(), pieceIndex ) != m_exitSetIndices.End() );
}


bool HorseshoeExitSetQuotientMetrics::IsInExitSet( const Point &p, uint pieceIndex ) const
{
    if ( m_isTrivial )
    {
        return true;
    }
    if ( m_xExitSetIndex != O_INVALID_INDEX && pieceIndex == m_xExitSetIndex )
    {
        return m_xExitSetInvert ? ( p[0] <= m_xExitSetValue ) : ( p[0] >= m_xExitSetValue );
    }
    return IsExitSetIndex( pieceIndex );
}


double HorseshoeExitSetQuotientMetrics::GetDistanceToExitSet( const Point &p, uint pieceIndex ) const
{
    if ( m_isTrivial )
    {
        return 0.0;
    }
    if ( IsInExitSet( p, pieceIndex ) ) // exit set
    {
        return 0.0;
    }
    double y = p[1];
    if ( m_xExitSetIndex != O_INVALID_INDEX && pieceIndex == m_xExitSetIndex )
    {
        double x = p[0];
        const double xDist = m_xExitSetInvert ? ( x - m_xExitSetValue ) : ( m_xExitSetValue - x );
        const double yDist = std::min( abs( y - m_thresholds[m_prevExitSet[pieceIndex] + 1] ), abs( y - m_thresholds[m_nextExitSet[pieceIndex]] ) );
        /*
        double prev = prevExitSet[pieceIndex];
        double prevTreshold = tresholds[prevExitSet[pieceIndex] + 1];
        double next = nextExitSet[pieceIndex];
        double nextTreshold = tresholds[nextExitSet[pieceIndex]];
        double resY = std::min(abs(y - tresholds[prevExitSet[pieceIndex] + 1]), abs(y - tresholds[nextExitSet[pieceIndex]]));
        double res = std::min(xDist, yDist);
        */
        return std::min( xDist, yDist );
    }
    return std::min( abs( y - m_thresholds[m_prevExitSet[pieceIndex] + 1] ), abs( y - m_thresholds[m_nextExitSet[pieceIndex]] ) );
}

////////////////////////////////////////////////////////////////////////////////

HorseshoeU::HorseshoeU( double exitMargin, double enterMargin, const Domain &domain, double offset, const Noise *noise )
    : HorseshoeMap( noise )
    , m_offset( offset )
{
    m_cube = domain.GetCube();

    m_exitSetQuotientMetrics = new HorseshoeExitSetQuotientMetrics( m_cube[1], exitMargin, 3 );

    const double sizeX = m_cube[0].GetLength();
    const double sizeY = m_cube[1].GetLength();

    m_shrinkCenter = m_cube[0].GetCenter();
    m_shrinkFactor = 0.5 - 2 * enterMargin;
    m_shrinkOffset = m_cube[0].m_min + sizeX * 0.25;

    m_stretchCenter = m_cube[1].GetCenter() * 0.5;
    m_stretchFactor = 3 + 2 * exitMargin;
    m_stretchOffset = m_cube[1].m_min + sizeY * 1.5;

    m_piece2Start = m_cube[1].m_max;
    m_piece3Start = m_piece2Start + sizeY;

    m_foldFactor = O_PI / sizeY;
    m_foldCenterX = m_cube[0].GetCenter();
    m_foldCenterY = m_cube[1].m_max;

    if ( m_offset != 0.0 )
    {
        SetupOffset( enterMargin, sizeX );
    }
}


void HorseshoeU::GetValue( const Point &d, Point &r ) const
{
    double x = d[0];
    double y = d[1];
    Shrink( x, y, x, y );
    Stretch( x, y, x, y );
    Fold( x, y, x, y );
    x += m_offset;
    r[0] = x;
    r[1] = y;
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( r );
    }
}


void HorseshoeU::SetupOffset( double enterMargin, double sizeX )
{
    assert( m_offset >= 0.0 );
    double e = enterMargin * sizeX;
    double w = ( 0.5 - 2 * enterMargin ) * sizeX;

    // nothing happens
    if ( m_offset < e )
    {
        return;
    }
    // everything is exit set
    if ( m_offset > 3 * e + 2 * w )
    {
        m_exitSetQuotientMetrics->SetIsTrivial( true );
        return;
    }

    // 3rd piece contains exit set
    if ( m_offset < e + w )
    {
        const double p = ( m_offset - e ) / w;
        m_exitSetQuotientMetrics->SetXExitSet( 3, p, true, m_cube[0] );
        return;
    }
    // 3rd piece is exit set
    m_exitSetQuotientMetrics->AddPieceToExitSet( 3 );
    if ( m_offset < 3 * e + w )
    {
        return;
    }
    // 1st piece contains exit set
    const double p = ( m_offset - 3 * e - w ) / w;
    m_exitSetQuotientMetrics->SetXExitSet( 1, p, false, m_cube[0] );
}


void HorseshoeU::Shrink( double dx, double dy, double &rx, double &ry ) const
{
    rx = ( dx - m_shrinkCenter ) * m_shrinkFactor + m_shrinkOffset;
    ry = dy; // y untouched
}


void HorseshoeU::Stretch( double dx, double dy, double &rx, double &ry ) const
{
    rx = dx; // x untouched
    ry = ( dy - m_stretchCenter ) * m_stretchFactor + m_stretchOffset;
}


void HorseshoeU::Fold( double dx, double dy, double &rx, double &ry ) const
{
    if ( dy > m_piece3Start )
    {
        rx = m_cube[0].m_max - ( dx - m_cube[0].m_min );
        ry = m_cube[1].m_max - ( dy - m_piece3Start );
    }
    else if ( dy > m_piece2Start )
    {
        const double angle = ( dy - m_piece2Start ) * m_foldFactor;
        const double r = m_foldCenterX - dx;
        rx = m_foldCenterX - cos( angle ) * r;
        ry = m_foldCenterY + sin( angle ) * r;
    }
    // else both x and y untouched
    else
    {
        rx = dx;
        ry = dy;
    }
}

////////////////////////////////////////////////////////////////////////////////

HorseshoeS::HorseshoeS( double exitMargin, double enterMargin, const Domain &domain, double offset, const Noise *noise )
    : HorseshoeMap( noise )
    , m_offset( offset )
{
    m_cube = domain.GetCube();

    m_exitSetQuotientMetrics = new HorseshoeExitSetQuotientMetrics( m_cube[1], exitMargin, 5 );

    const double sizeX = m_cube[0].GetLength();
    const double sizeY = m_cube[1].GetLength();

    m_shrinkCenter = m_cube[0].GetCenter();
    m_shrinkFactor = 0.33 - 2 * enterMargin;
    m_shrinkOffset = m_cube[0].m_min + sizeX * 0.166;

    m_stretchCenter = m_cube[1].GetCenter();
    m_stretchFactor = 5 + 2 * exitMargin;
    m_stretchOffset = m_cube[1].m_min + sizeY * 2.5;

    m_piece2Start = m_cube[1].m_max;
    m_piece3Start = m_piece2Start + sizeY;
    m_piece4Start = m_piece3Start + sizeY;
    m_piece5Start = m_piece4Start + sizeY;

    m_piece1Center = m_cube[0].m_min + sizeX * 0.166;
    m_piece3Center = m_cube[0].m_min + sizeX * 0.5;
    m_piece5Center = m_cube[0].m_min + sizeX * ( 1.0 - 0.166 );

    m_foldFactor = O_PI / sizeY;
    m_foldUpCenterX = ( m_piece1Center + m_piece3Center ) * 0.5;
    m_foldUpCenterY = m_cube[1].m_max;
    m_foldDownCenterX = ( m_piece3Center + m_piece5Center ) * 0.5;
    m_foldDownCenterY = m_cube[1].m_min;

    if ( m_offset != 0.0 )
    {
        SetupOffset( enterMargin, sizeX );
    }
}


void HorseshoeS::GetValue( const Point &d, Point &r ) const
{
    double x = d[0];
    double y = d[1];
    Shrink( x, y, x, y );
    Stretch( x, y, x, y );
    Fold( x, y, x, y );
    x += m_offset;
    r[0] = x;
    r[1] = y;
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( r );
    }
}


void HorseshoeS::SetupOffset( double enterMargin, double sizeX )
{
    assert( m_offset >= 0 );
    const double e = enterMargin * sizeX;
    const double w = ( 0.33 - 2 * enterMargin ) * sizeX;

    // nothing happens
    if ( m_offset < e )
    {
        return;
    }
    // everything is exit set
    if ( m_offset > 5 * e + 3 * w )
    {
        m_exitSetQuotientMetrics->SetIsTrivial( true );
        return;
    }

    // 5th piece contains exit set
    if ( m_offset < e + w )
    {
        const double p = ( m_offset - e ) / w;
        m_exitSetQuotientMetrics->SetXExitSet( 5, p, false, m_cube[0] );
        return;
    }
    // 5th piece is exit set
    m_exitSetQuotientMetrics->AddPieceToExitSet( 5 );
    if ( m_offset < 3 * e + w )
    {
        return;
    }

    // 3rd piece contains exit set
    if ( m_offset < 3 * e + 2 * w )
    {
        const double p = ( m_offset - 3 * e - w ) / w;
        m_exitSetQuotientMetrics->SetXExitSet( 3, p, true, m_cube[0] );
        return;
    }
    // 3rd piece is exit set
    m_exitSetQuotientMetrics->AddPieceToExitSet( 3 );
    if ( m_offset < 5 * e + 2 * w )
    {
        return;
    }

    // 1st piece contains exit set
    const double p = ( m_offset - 5 * e - 2 * w ) / w;
    m_exitSetQuotientMetrics->SetXExitSet( 1, p, false, m_cube[0] );
}


void HorseshoeS::Shrink( double dx, double dy, double &rx, double &ry ) const
{
    rx = ( dx - m_shrinkCenter ) * m_shrinkFactor + m_shrinkOffset;
    ry = dy; // y untouched
}


void HorseshoeS::Stretch( double dx, double dy, double &rx, double &ry ) const
{
    rx = dx; // x untouched
    ry = ( dy - m_stretchCenter ) * m_stretchFactor + m_stretchOffset;
}


void HorseshoeS::Fold( double dx, double dy, double &rx, double &ry ) const
{
    if ( dy > m_piece5Start )
    {
        rx = m_piece5Center + ( dx - m_piece1Center );
        ry = m_cube[1].m_min + ( dy - m_piece5Start );
    }
    else if ( dy > m_piece4Start )
    {
        const double angle = ( dy - m_piece4Start ) * m_foldFactor;
        const double r = m_foldDownCenterX - ( m_piece3Center - ( dx - m_piece1Center ) );
        rx = m_foldDownCenterX - cos( angle ) * r;
        ry = m_foldDownCenterY - sin( angle ) * r;
    }
    else if ( dy > m_piece3Start )
    {
        rx = ( m_piece3Center - ( dx - m_piece1Center ) );
        ry = m_cube[1].m_max - ( dy - m_piece3Start );
    }
    else if ( dy > m_piece2Start )
    {
        const double angle = ( dy - m_piece2Start ) * m_foldFactor;
        const double r = m_foldUpCenterX - dx;
        rx = m_foldUpCenterX - cos( angle ) * r;
        ry = m_foldUpCenterY + sin( angle ) * r;
    }
    // else both x and y untouched
    else
    {
        rx = dx;
        ry = dy;
    }
}

////////////////////////////////////////////////////////////////////////////////

HorseshoeG::HorseshoeG( double exitMargin, double enterMargin, const Domain &domain, double offset, const Noise *noise )
    : HorseshoeMap( noise )
    , m_offset( offset )
{
    m_cube = domain.GetCube();

    m_exitSetQuotientMetrics = new HorseshoeExitSetQuotientMetrics( m_cube[1], exitMargin, 5 );
    m_exitSetQuotientMetrics->AddPieceToExitSet( 3 );

    const double sizeX = m_cube[0].GetLength();
    const double sizeY = m_cube[1].GetLength();

    m_shrinkCenter = m_cube[0].GetCenter();
    m_shrinkFactor = 0.5 - 2 * enterMargin;
    m_shrinkOffset = m_cube[0].m_min + sizeX * 0.25;

    m_stretchCenter = m_cube[1].GetCenter();
    m_stretchFactor = 5 + 2 * exitMargin;
    m_stretchOffset = m_cube[1].m_min + sizeY * 2.5;

    m_piece2Start = m_cube[1].m_max;
    m_piece3Start = m_piece2Start + sizeY;
    m_piece4Start = m_piece3Start + sizeY;
    m_piece5Start = m_piece4Start + sizeY;

    m_piece1Center = m_cube[0].m_min + sizeX * 0.25;
    m_piece3Center = m_cube[0].m_min + sizeX * 1.25;
    m_piece5Center = m_cube[0].m_min + sizeX * 0.75;

    m_foldFactor = O_PI / sizeY;
    m_foldUpCenterX = ( m_piece1Center + m_piece3Center ) * 0.5;
    m_foldUpCenterY = m_cube[1].m_max;
    m_foldDownCenterX = ( m_piece3Center + m_piece5Center ) * 0.5;
    m_foldDownCenterY = m_cube[1].m_min;

    if ( m_offset != 0.0 )
    {
        SetupOffset( enterMargin, sizeX );
    }
}


void HorseshoeG::GetValue( const Point &d, Point &r ) const
{
    double x = d[0];
    double y = d[1];
    Shrink( x, y, x, y );
    Stretch( x, y, x, y );
    Fold( x, y, x, y );
    x += m_offset;
    r[0] = x;
    r[1] = y;
    if ( m_noise != nullptr )
    {
        m_noise->AddNoise( r );
    }
}


void HorseshoeG::SetupOffset( double enterMargin, double sizeX )
{
    assert( m_offset >= 0 );
    const double e = enterMargin * sizeX;
    const double w = ( 0.5 - 2 * enterMargin ) * sizeX;

    // nothing happens
    if ( m_offset < e )
    {
        return;
    }
    // everything is exit set
    if ( m_offset > 3 * e + 2 * w )
    {
        m_exitSetQuotientMetrics->SetIsTrivial( true );
        return;
    }

    // 5th piece contains exit set
    if ( m_offset < e + w )
    {
        const double p = ( m_offset - e ) / w;
        m_exitSetQuotientMetrics->SetXExitSet( 5, p, false, m_cube[0] );
        return;
    }
    // 5th piece is exit set
    m_exitSetQuotientMetrics->AddPieceToExitSet( 5 );
    if ( m_offset < 3 * e + w )
    {
        return;
    }
    // 1st piece contains exit set
    const double p = ( m_offset - 3 * e - w ) / w;
    m_exitSetQuotientMetrics->SetXExitSet( 1, p, false, m_cube[0] );
}


void HorseshoeG::Shrink( double dx, double dy, double &rx, double &ry ) const
{
    rx = ( dx - m_shrinkCenter ) * m_shrinkFactor + m_shrinkOffset;
    ry = dy; // y untouched
}


void HorseshoeG::Stretch( double dx, double dy, double &rx, double &ry ) const
{
    rx = dx; // x untouched
    ry = ( dy - m_stretchCenter ) * m_stretchFactor + m_stretchOffset;
}


void HorseshoeG::Fold( double dx, double dy, double &rx, double &ry ) const
{
    if ( dy > m_piece5Start )
    {
        rx = m_piece5Center + ( dx - m_piece1Center );
        ry = m_cube[1].m_min + ( dy - m_piece5Start );
    }
    else if ( dy > m_piece4Start )
    {
        const double angle = ( dy - m_piece4Start ) * m_foldFactor;
        const double r = m_foldDownCenterX - ( m_piece3Center - ( dx - m_piece1Center ) );
        rx = m_foldDownCenterX - cos( angle ) * r;
        ry = m_foldDownCenterY + sin( angle ) * r;
    }
    else if ( dy > m_piece3Start )
    {
        rx = ( m_piece3Center - ( dx - m_piece1Center ) );
        ry = m_cube[1].m_max - ( dy - m_piece3Start );
    }
    else if ( dy > m_piece2Start )
    {
        const double angle = ( dy - m_piece2Start ) * m_foldFactor;
        const double r = m_foldUpCenterX - dx;
        rx = m_foldUpCenterX - cos( angle ) * r;
        ry = m_foldUpCenterY + sin( angle ) * r;
    }
    // else both x and y untouched
    else
    {
        rx = dx;
        ry = dy;
    }
}
