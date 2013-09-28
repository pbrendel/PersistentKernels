/* 
 * File:   HorseshoeMap.hpp
 * Author: Piotr Brendel
 */

#ifndef HORSESHOEMAP_HPP
#define	HORSESHOEMAP_HPP

#include <cmath>
#include <vector>
#include <cassert>

#include "Point.hpp"
#include "Map.hpp"
#include "Domain.hpp"
#include "Metric.hpp"

////////////////////////////////////////////////////////////////////////////////

class HorseshoeExitSetQuotientMetric : public Metric
{
    EuclideanMetric euclideanMetric;

    int tresholdsCount;
    double *tresholds;

    std::vector<int> exitSetIndices;
    std::map<int, int> prevExitSet;
    std::map<int, int> nextExitSet;

    int xExitSetIndex;
    double xExitSetValue;
    bool xExitSetInvert;
    
    bool isTrivial;
    
public:

    HorseshoeExitSetQuotientMetric(const Interval &yInterval, double exitMargin, int piecesCount)
    {
        isTrivial = false;
        tresholdsCount = piecesCount + 3;
        tresholds = new double[tresholdsCount];
        double pieceDelta = yInterval.GetLength() / (piecesCount + 2 * exitMargin);
        double marginDelta = pieceDelta * exitMargin;
        double t = yInterval.min;
        tresholds[0] = yInterval.min;
        t += marginDelta;
        tresholds[1] = t;
        for (int i = 0; i < piecesCount; i++)
        {
            t += pieceDelta;
            tresholds[2 + i] = t;
        }
        tresholds[tresholdsCount - 1] = yInterval.max;
        for (int i = 0; i < piecesCount + 2; i += 2)
        {
            exitSetIndices.push_back(i);
        }
        UpdatePrevNextExitSet();
        xExitSetIndex = -1;
        xExitSetValue = 0;
        xExitSetInvert = false;
    }

    ~HorseshoeExitSetQuotientMetric()
    {
        delete [] tresholds;
    }

    double Distance(const Point &x, const Point &y)
    {
        if (isTrivial)
        {
            return 0;
        }
        assert(x.size() == 2);
        assert(y.size() == 2);
        int ix = GetPieceIndex(x[1]);
        int iy = GetPieceIndex(y[1]);
        if (!IsInExitSet(x, ix) && !IsInExitSet(y, iy) && prevExitSet[ix] == prevExitSet[iy]/* && nextExitSet[ix] = nextExitSet[iy]*/)
        {
            return std::min(euclideanMetric.Distance(x, y), DistanceToExitSet(x, ix) + DistanceToExitSet(y, iy));
        }
        else
        {
            return DistanceToExitSet(x, ix) + DistanceToExitSet(y, iy);
        }
    }

    void AddPieceToExitSet(int pieceIndex)
    {
        exitSetIndices.push_back(pieceIndex);
        std::sort(exitSetIndices.begin(), exitSetIndices.end());
        UpdatePrevNextExitSet();
    }

    void RemovePieceFromExitSet(int pieceIndex)
    {
        std::vector<int>::iterator it = std::find(exitSetIndices.begin(), exitSetIndices.end(), pieceIndex);
        if (it != exitSetIndices.end())
        {
            exitSetIndices.erase(it);
        }
        UpdatePrevNextExitSet();
    }
    
    void SetXExitSet(int index, float progress, bool isInvert, const Interval &xInterval)
    {
        xExitSetIndex = index;
        xExitSetInvert = isInvert;
        if (isInvert)
        {
            xExitSetValue = progress * xInterval.GetLength() + xInterval.min;
        }
        else
        {
            xExitSetValue = (1.0 - progress) * xInterval.GetLength() + xInterval.min;
        }
    }

    void SetIsTrivial(bool trivial)
    {
        isTrivial = trivial;
    }
    
    bool IsInExitSet(const Point &p)
    {
        if (isTrivial)
        {
            return true;
        }
        return IsInExitSet(p, GetPieceIndex(p[1]));
    }

    double DistanceToExitSet(const Point &p)
    {
        if (isTrivial)
        {
            return 0;
        }
        return DistanceToExitSet(p, GetPieceIndex(p[1]));
    }

    
private:

    void UpdatePrevNextExitSet()
    {
        assert(exitSetIndices.size() > 0);
        prevExitSet.clear();
        nextExitSet.clear();
        int first = exitSetIndices[0];
        int last = exitSetIndices[exitSetIndices.size() - 1];
        for (int i = 0; i < tresholdsCount - 1; i++)
        {
            if (IsExitSetIndex(i))
            {
                continue;
            }
            int index = first;
            for (int j = i - 1; j > first; j--)
            {
                if (IsExitSetIndex(j))
                {
                    index = j;
                    break;
                }
            }
            prevExitSet[i] = index;
            index = last;
            for (int j = i + 1; j < last; j++)
            {
                if (IsExitSetIndex(j))
                {
                    index = j;
                    break;
                }
            }
            nextExitSet[i] = index;
        } 
    }

    int GetPieceIndex(double y)
    {
        for (int i = 0; i < tresholdsCount - 1; i++)
        {
            if (tresholds[i + 1] > y)
            {
                return i;
            }
        }
        return tresholdsCount - 2;
    }

    bool IsExitSetIndex(int pieceIndex)
    {
        if (isTrivial)
        {
            return true;
        }
        return (std::find(exitSetIndices.begin(), exitSetIndices.end(), pieceIndex) != exitSetIndices.end());        
    }
    
    bool IsInExitSet(const Point &p, int pieceIndex)
    {
        if (isTrivial)
        {
            return true;
        }
        if (xExitSetIndex != -1 && pieceIndex == xExitSetIndex)
        {
            return xExitSetInvert ? (p[0] <= xExitSetValue) : (p[0] >= xExitSetValue);
        }
        return IsExitSetIndex(pieceIndex);              
    }

    double DistanceToExitSet(const Point &p, int pieceIndex)
    {
        if (isTrivial)
        {
            return 0;
        }
        if (IsInExitSet(p, pieceIndex)) // exit set
        {
            return 0;
        }
        double y = p[1];
        if (xExitSetIndex != -1 && pieceIndex == xExitSetIndex)
        {
            double x = p[0];
            double xDist = xExitSetInvert ? (x - xExitSetValue) : (xExitSetValue - x);
            double yDist = std::min(abs(y - tresholds[prevExitSet[pieceIndex] + 1]), abs(y - tresholds[nextExitSet[pieceIndex]]));
            /*
            double prev = prevExitSet[pieceIndex];
            double prevTreshold = tresholds[prevExitSet[pieceIndex] + 1];
            double next = nextExitSet[pieceIndex];
            double nextTreshold = tresholds[nextExitSet[pieceIndex]];
            double resY = std::min(abs(y - tresholds[prevExitSet[pieceIndex] + 1]), abs(y - tresholds[nextExitSet[pieceIndex]]));
            double res = std::min(xDist, yDist);
            */
            return std::min(xDist, yDist);
        }        
        return std::min(abs(y - tresholds[prevExitSet[pieceIndex] + 1]), abs(y - tresholds[nextExitSet[pieceIndex]]));
    }

    friend std::ostream &operator<<(std::ostream &str, const HorseshoeExitSetQuotientMetric &metric);
};

std::ostream &operator<<(std::ostream &str, const HorseshoeExitSetQuotientMetric &metric)
{
    str<<"tresholds:"<<std::endl;
    for (int i = 0; i < metric.tresholdsCount; i++)
    {
        str<<metric.tresholds[i]<<std::endl;
    }
    str<<"exit set:"<<std::endl;
    for (int i = 0; i < metric.exitSetIndices.size(); i++)
    {
        str<<"["<<metric.tresholds[metric.exitSetIndices[i]]<<", "<<metric.tresholds[metric.exitSetIndices[i] + 1]<<"]"<<std::endl;
    }
    str<<"prev and next exit sets:"<<std::endl;
    std::map<int, int>::const_iterator itPrev = metric.prevExitSet.begin();
    std::map<int, int>::const_iterator itNext = metric.nextExitSet.begin();
    while (itPrev != metric.prevExitSet.end())
    {
        str<<itPrev->first<<" "<<itPrev->second<<" "<<itNext->second<<std::endl;
        itPrev++;
        itNext++;
    }
    if (metric.xExitSetIndex != -1)
    {
        std::cout<<"x exit set index: "<<metric.xExitSetIndex<<" value "<<metric.xExitSetValue<<" invert: "<<metric.xExitSetInvert<<std::endl;
    }
    return str;
}

////////////////////////////////////////////////////////////////////////////////

class HorseshoeMap : public Map
{
protected:

    HorseshoeExitSetQuotientMetric *exitSetQuotientMetric;

public:

    HorseshoeMap(Noise *noise = 0) : Map(2, noise)
    {
        exitSetQuotientMetric = 0;
    }

    ~HorseshoeMap()
    {
        delete exitSetQuotientMetric;
    }

    Metric *GetExitSetQuotientMetric()
    {
        return exitSetQuotientMetric;
    }

};

////////////////////////////////////////////////////////////////////////////////

class HorseshoeU : public HorseshoeMap
{
    Cube cube;
    
    double shrinkCenter;
    double shrinkFactor;
    double shrinkOffset;

    double stretchCenter;
    double stretchFactor;
    double stretchOffset;

    double piece2Start;
    double piece3Start;

    double foldFactor;    
    double foldCenterX;
    double foldCenterY;

    double offset;
    
public:

    HorseshoeU(double exitMargin, double enterMargin, Domain *domain, double offset = 0, Noise *noise = 0) : HorseshoeMap(noise)
    {
        cube = domain->GetCube();

        exitSetQuotientMetric = new HorseshoeExitSetQuotientMetric(cube[1], exitMargin, 3);
        std::cout<<*exitSetQuotientMetric<<std::endl;

        double sizeX = cube[0].GetLength();
        double sizeY = cube[1].GetLength();

        shrinkCenter = cube[0].GetCenter();
        shrinkFactor = 0.5 - 2 * enterMargin;
        shrinkOffset = cube[0].min + sizeX * 0.25;

        stretchCenter = cube[1].GetCenter() * 0.5;
        stretchFactor = 3 + 2 * exitMargin;
        stretchOffset = cube[1].min + sizeY * 1.5;

        piece2Start = cube[1].max;
        piece3Start = piece2Start + sizeY;

        foldFactor = M_PI / sizeY;
        foldCenterX = cube[0].GetCenter();
        foldCenterY = cube[1].max;
        
        this->offset = offset;
        if (offset != 0)
        {
            SetupOffset(enterMargin, sizeX);
        }
    }

    void GetValue(const Point &d, Point &r)
    {
        double x = d[0];
        double y = d[1];
        Shrink(x, y, x, y);
        Stretch(x, y, x, y);
        Fold(x, y, x, y);
        x += offset;
        r[0] = x;
        r[1] = y;
        if (this->noise != 0)
        {
            this->noise->AddNoise(r);
        }
    }

private:

    void SetupOffset(double enterMargin, double sizeX)
    {
        assert(offset >= 0);
        double e = enterMargin * sizeX;
        double w = (0.5 - 2 * enterMargin) * sizeX;
        
        // nothing happens
        if (offset < e)
        {
            return;
        }
        // everything is exit set
        if (offset > 3 * e + 2 * w)
        {
            exitSetQuotientMetric->SetIsTrivial(true);
            return;
        }
        
        // 3rd piece contains exit set
        if (offset < e + w)
        {
            double p = (offset - e) / w;
            exitSetQuotientMetric->SetXExitSet(3, p, true, cube[0]);
            return;
        }
        // 3rd piece is exit set
        exitSetQuotientMetric->AddPieceToExitSet(3);
        if (offset < 3 * e + w)
        {
            return;
        }
        // 1st piece contains exit set
        double p = (offset - 3 * e - w) / w;
        exitSetQuotientMetric->SetXExitSet(1, p, false, cube[0]);
    }
    
    void Shrink(double dx, double dy, double &rx, double &ry)
    {
        rx = (dx - shrinkCenter) * shrinkFactor + shrinkOffset;
        ry = dy; // y untouched
    }

    void Stretch(double dx, double dy, double &rx, double &ry)
    {        
        rx = dx; // x untouched
        ry = (dy - stretchCenter) * stretchFactor + stretchOffset;
    }

    void Fold(double dx, double dy, double &rx, double &ry)
    {
        if (dy > piece3Start)
        {
            rx = cube[0].max - (dx - cube[0].min);
            ry = cube[1].max - (dy - piece3Start);
        }
        else if (dy > piece2Start)
        {
            double angle = (dy - piece2Start) * foldFactor;
            double r = foldCenterX - dx;
            rx = foldCenterX - cos(angle) * r;
            ry = foldCenterY + sin(angle) * r;
        }
        // else both x and y untouched
        else
        {
            rx = dx;
            ry = dy;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

class HorseshoeS : public HorseshoeMap
{
    Cube cube;

    double shrinkCenter;
    double shrinkFactor;
    double shrinkOffset;

    double stretchCenter;
    double stretchFactor;
    double stretchOffset;

    double piece2Start;
    double piece3Start;
    double piece4Start;
    double piece5Start;

    double piece1Center;
    double piece3Center;
    double piece5Center;

    double foldFactor;
    double foldUpCenterX;
    double foldUpCenterY;
    double foldDownCenterX;
    double foldDownCenterY;

    double offset;
    
public:

    HorseshoeS(double exitMargin, double enterMargin, Domain *domain, double offset, Noise *noise = 0) : HorseshoeMap(noise)
    {
        cube = domain->GetCube();

        exitSetQuotientMetric = new HorseshoeExitSetQuotientMetric(cube[1], exitMargin, 5);
        std::cout<<*exitSetQuotientMetric<<std::endl;

        double sizeX = cube[0].GetLength();
        double sizeY = cube[1].GetLength();

        shrinkCenter = cube[0].GetCenter();
        shrinkFactor = 0.33 - 2 * enterMargin;
        shrinkOffset = cube[0].min + sizeX * 0.166;

        stretchCenter = cube[1].GetCenter();
        stretchFactor = 5 + 2 * exitMargin;
        stretchOffset = cube[1].min + sizeY * 2.5;

        piece2Start = cube[1].max;
        piece3Start = piece2Start + sizeY;
        piece4Start = piece3Start + sizeY;
        piece5Start = piece4Start + sizeY;

        piece1Center = cube[0].min + sizeX * 0.166;
        piece3Center = cube[0].min + sizeX * 0.5;
        piece5Center = cube[0].min + sizeX * (1.0 - 0.166);

        foldFactor = M_PI / sizeY;
        foldUpCenterX = (piece1Center + piece3Center) * 0.5;
        foldUpCenterY = cube[1].max;
        foldDownCenterX = (piece3Center + piece5Center) * 0.5;
        foldDownCenterY = cube[1].min;
        
        this->offset = offset;
        if (offset != 0)
        {
            SetupOffset(enterMargin, sizeX);
        }        
    }

    void GetValue(const Point &d, Point &r)
    {
        double x = d[0];
        double y = d[1];
        Shrink(x, y, x, y);
        Stretch(x, y, x, y);
        Fold(x, y, x, y);
        x += offset;
        r[0] = x;
        r[1] = y;
        if (this->noise != 0)
        {
            this->noise->AddNoise(r);
        }
    }

private:

    void SetupOffset(double enterMargin, double sizeX)
    {
        assert(offset >= 0);
        double e = enterMargin * sizeX;
        double w = (0.33 - 2 * enterMargin) * sizeX;
        
        // nothing happens
        if (offset < e)
        {
            return;
        }
        // everything is exit set
        if (offset > 5 * e + 3 * w)
        {
            exitSetQuotientMetric->SetIsTrivial(true);
            return;
        }
        
        // 5th piece contains exit set
        if (offset < e + w)
        {
            double p = (offset - e) / w;
            exitSetQuotientMetric->SetXExitSet(5, p, false, cube[0]);
            return;
        }
        // 5th piece is exit set
        exitSetQuotientMetric->AddPieceToExitSet(5);
        if (offset < 3 * e + w)
        {
            return;
        }
        
        // 3rd piece contains exit set
        if (offset < 3 * e + 2 * w)
        {
            double p = (offset - 3 * e - w) / w;
            exitSetQuotientMetric->SetXExitSet(3, p, true, cube[0]);
            return;
        }
        // 3rd piece is exit set
        exitSetQuotientMetric->AddPieceToExitSet(3);
        if (offset < 5 * e + 2 * w)
        {
            return;
        }
                       
        // 1st piece contains exit set
        double p = (offset - 5 * e - 2 * w) / w;
        exitSetQuotientMetric->SetXExitSet(1, p, false, cube[0]);
    }
    
    void Shrink(double dx, double dy, double &rx, double &ry)
    {
        rx = (dx - shrinkCenter) * shrinkFactor + shrinkOffset;
        ry = dy; // y untouched
    }

    void Stretch(double dx, double dy, double &rx, double &ry)
    {
        rx = dx; // x untouched
        ry = (dy - stretchCenter) * stretchFactor + stretchOffset;
    }

    void Fold(double dx, double dy, double &rx, double &ry)
    {        
        if (dy > piece5Start)
        {
            rx = piece5Center + (dx - piece1Center);
            ry = cube[1].min + (dy - piece5Start);
        }        
        else if (dy > piece4Start)
        {
            double angle = (dy - piece4Start) * foldFactor;
            double r = foldDownCenterX - (piece3Center - (dx - piece1Center));
            rx = foldDownCenterX - cos(angle) * r;
            ry = foldDownCenterY - sin(angle) * r;
        }
        else if (dy > piece3Start)
        {
            rx = (piece3Center - (dx - piece1Center));
            ry = cube[1].max - (dy - piece3Start);
        }        
        else if (dy > piece2Start)
        {
            double angle = (dy - piece2Start) * foldFactor;
            double r = foldUpCenterX - dx;
            rx = foldUpCenterX - cos(angle) * r;
            ry = foldUpCenterY + sin(angle) * r;
        }         
        // else both x and y untouched
        else
        {
            rx = dx;
            ry = dy;
        }         
    }
};

////////////////////////////////////////////////////////////////////////////////

class HorseshoeG : public HorseshoeMap
{
    Cube cube;

    double shrinkCenter;
    double shrinkFactor;
    double shrinkOffset;

    double stretchCenter;
    double stretchFactor;
    double stretchOffset;

    double piece2Start;
    double piece3Start;
    double piece4Start;
    double piece5Start;

    double piece1Center;
    double piece3Center;
    double piece5Center;

    double foldFactor;
    double foldUpCenterX;
    double foldUpCenterY;
    double foldDownCenterX;
    double foldDownCenterY;
    
    double offset;

public:

    HorseshoeG(double exitMargin, double enterMargin, Domain *domain, double offset = 0, Noise *noise = 0) : HorseshoeMap(noise)
    {
        cube = domain->GetCube();

        exitSetQuotientMetric = new HorseshoeExitSetQuotientMetric(cube[1], exitMargin, 5);
        exitSetQuotientMetric->AddPieceToExitSet(3);
        std::cout<<*exitSetQuotientMetric<<std::endl;

        double sizeX = cube[0].GetLength();
        double sizeY = cube[1].GetLength();

        shrinkCenter = cube[0].GetCenter();
        shrinkFactor = 0.5 - 2 * enterMargin;
        shrinkOffset = cube[0].min + sizeX * 0.25;

        stretchCenter = cube[1].GetCenter();
        stretchFactor = 5 + 2 * exitMargin;
        stretchOffset = cube[1].min + sizeY * 2.5;

        piece2Start = cube[1].max;
        piece3Start = piece2Start + sizeY;
        piece4Start = piece3Start + sizeY;
        piece5Start = piece4Start + sizeY;

        piece1Center = cube[0].min + sizeX * 0.25;
        piece3Center = cube[0].min + sizeX * 1.25;
        piece5Center = cube[0].min + sizeX * 0.75;

        foldFactor = M_PI / sizeY;
        foldUpCenterX = (piece1Center + piece3Center) * 0.5;
        foldUpCenterY = cube[1].max;
        foldDownCenterX = (piece3Center + piece5Center) * 0.5;
        foldDownCenterY = cube[1].min;
        
        this->offset = offset;
        if (offset != 0)
        {
            SetupOffset(enterMargin, sizeX);
        }        
    }

    void GetValue(const Point &d, Point &r)
    {
        double x = d[0];
        double y = d[1];
        Shrink(x, y, x, y);
        Stretch(x, y, x, y);
        Fold(x, y, x, y);
        x += offset;
        r[0] = x;
        r[1] = y;
        if (this->noise != 0)
        {
            this->noise->AddNoise(r);
        }
    }

private:

    void SetupOffset(double enterMargin, double sizeX)
    {
        assert(offset >= 0);
        double e = enterMargin * sizeX;
        double w = (0.5 - 2 * enterMargin) * sizeX;
        
        // nothing happens
        if (offset < e)
        {
            return;
        }
        // everything is exit set
        if (offset > 3 * e + 2 * w)
        {
            exitSetQuotientMetric->SetIsTrivial(true);
            return;
        }
        
        // 5th piece contains exit set
        if (offset < e + w)
        {
            double p = (offset - e) / w;
            exitSetQuotientMetric->SetXExitSet(5, p, false, cube[0]);
            return;
        }
        // 5th piece is exit set
        exitSetQuotientMetric->AddPieceToExitSet(5);
        if (offset < 3 * e + w)
        {
            return;
        }
        // 1st piece contains exit set
        double p = (offset - 3 * e - w) / w;
        exitSetQuotientMetric->SetXExitSet(1, p, false, cube[0]);
    }
    
    void Shrink(double dx, double dy, double &rx, double &ry)
    {
        rx = (dx - shrinkCenter) * shrinkFactor + shrinkOffset;
        ry = dy; // y untouched
    }

    void Stretch(double dx, double dy, double &rx, double &ry)
    {
        rx = dx; // x untouched
        ry = (dy - stretchCenter) * stretchFactor + stretchOffset;
    }

    void Fold(double dx, double dy, double &rx, double &ry)
    {
        if (dy > piece5Start)
        {
            rx = piece5Center + (dx - piece1Center);
            ry = cube[1].min + (dy - piece5Start);
        }
        else if (dy > piece4Start)
        {
            double angle = (dy - piece4Start) * foldFactor;
            double r = foldDownCenterX - (piece3Center - (dx - piece1Center));
            rx = foldDownCenterX - cos(angle) * r;
            ry = foldDownCenterY + sin(angle) * r;
        }
        else if (dy > piece3Start)
        {
            rx = (piece3Center - (dx - piece1Center));
            ry = cube[1].max - (dy - piece3Start);
        }
        else if (dy > piece2Start)
        {
            double angle = (dy - piece2Start) * foldFactor;
            double r = foldUpCenterX - dx;
            rx = foldUpCenterX - cos(angle) * r;
            ry = foldUpCenterY + sin(angle) * r;
        }
        // else both x and y untouched
        else
        {
            rx = dx;
            ry = dy;
        }
    }
};

#endif	/* HORSESHOEMAP_HPP */

