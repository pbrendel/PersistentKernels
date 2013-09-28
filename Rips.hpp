/* 
 * File:   Rips.hpp
 * Author: Piotr Brendel
 */

#ifndef RIPS_HPP
#define	RIPS_HPP

#include <vector>
#include <set>
#include <deque>

#include <iostream>

#include "Utils.hpp"
#include "Metric.hpp"
#include "IndexMetric.hpp"
#include "ExitSetQuotientIndexMetric.hpp"

template <typename PointType>
class RipsT
{
    typedef PointType Point;

    struct Vertex
    {
        Point *point;
        int label;
        int cc;

        Vertex(Point *p) : point(p), cc(-1)
        {
        }
    };

    typedef std::vector<int> IntVector;
    typedef std::vector<int> Simplex;

    std::vector<Vertex> verts;
    std::vector<IntVector> neighbours;
    std::vector<Simplex> simplices;
    IntVector connectedComponents;

  //  Metric *tmpMetric;

public:

    RipsT()
    {

    }

    template <typename MetricType>
    RipsT(std::vector<Point> &points, MetricType *metric, double epsilon, int) // last param is just a placeholder
    {
        Create(points, metric, epsilon);
    }

    template <typename MetricType>
    void Create(std::vector<Point> &points, MetricType *metric, double epsilon, int maxRipsDim = 1) // last param is just a placeholder
    {
 //       tmpMetric = metric;

        CreateVertsAndNeighbours(points, metric, epsilon);
        CreateEdges();
    }

    template <typename MetricType>
    void CreateVertsAndNeighbours(std::vector<Point> &points, MetricType *metric, double epsilon, bool gluePoints = false)
    {
        for (typename std::vector<Point>::iterator i = points.begin(); i != points.end(); i++)
        {
            verts.push_back(Vertex(&(*i)));
        }
        if (gluePoints)
        {
            for (typename std::vector<Vertex>::iterator v1 = verts.begin(); v1 != verts.end(); v1++)
            {
                typename std::vector<Vertex>::iterator v2 = v1;
                v2++;
                while (v2 != verts.end())
                {
                    if (metric->Distance(*v1->point, *v2->point) == 0)
                    {
                        v2 = verts.erase(v2);
                    }
                    else
                    {
                        v2++;
                    }
                }
            }
        }
        int label = 0;
        for (typename std::vector<Vertex>::iterator v = verts.begin(); v != verts.end(); v++)
        {
            v->label = label++;
        }
        for (typename std::vector<Vertex>::iterator v1 = verts.begin(); v1 != verts.end(); v1++)
        {
            IntVector n;
            typename std::vector<Vertex>::iterator v2 = v1;
            v2++;
            while (v2 != verts.end())
            {
                //std::cout<<v1->label<<" "<<v2->label<<" "<<metric->Distance(*v1->point, *v2->point)<<std::endl;
                if (metric->Distance(*v1->point, *v2->point) <= epsilon)
                {
                    n.push_back(v2->label);
                }
                v2++;
            }
            neighbours.push_back(n);
        }
    }

    void CreateVertsAndNeighbours(std::vector<Point> &points, IndexMetric *metric, double epsilon, bool gluePoints = false)
    {
        for (typename std::vector<Point>::iterator i = points.begin(); i != points.end(); i++)
        {
            verts.push_back(Vertex(&(*i)));
        }
        if (gluePoints)
        {
            // cannot glue points with precomputed index metric
            assert(false);
        }
        int label = 0;
        for (typename std::vector<Vertex>::iterator v = verts.begin(); v != verts.end(); v++)
        {
            v->label = label++;
        }
        int vertsCount = verts.size();
        for (int i = 0; i < vertsCount; i++)
        {
            IntVector n;
            for (int j = i + 1; j < vertsCount; j++)
            {
                //std::cout<<v1->label<<" "<<v2->label<<" "<<metric->Distance(*v1->point, *v2->point)<<std::endl;
                if (metric->Distance(i, j, points[i], points[j]) <= epsilon)
                {
                    n.push_back(j);
                }
            }
            neighbours.push_back(n);
        }
    }

    void CreateEdges()
    {
        Simplex vertex(1);
        for (int i = 0; i < neighbours.size(); i++)
        {
            IntVector n = neighbours[i];
            if (n.size() == 0)
            {
                vertex[0] = i;
                simplices.push_back(vertex);
            }
            else
            {
                for (IntVector::iterator v = n.begin(); v != n.end(); v++)
                {
                    Simplex s;
                    s.push_back(i);
                    s.push_back(*v);
                    simplices.push_back(s);
                }
            }
        }
    }

    void GetSimplices(std::vector<std::vector<int> > &simplices)
    {
        simplices.assign(this->simplices.begin(), this->simplices.end());
    }

    template <typename ComplexType>
    void GetComplex(ComplexType &complex)
    {
        for (std::vector<IntVector>::iterator s = simplices.begin(); s != simplices.end(); s++)
        {
            std::set<int> s1(s->begin(), s->end());
            complex.addSimplex(s1);
        }
    }

private:

    static bool EdgesComparer(const Simplex &a, const Simplex &b)
    {
        if (a[0] < b[0]) return true;
        if (a[0] == b[1]) return a[1] < b[1];
        return false;
    }

public:

    void Calculate0thHomologyGroup()
    {
        int size = verts.size();
        for (int i = 0; i < size; i++)
        {
            IntVector n = neighbours[i];
            for (IntVector::iterator v = n.begin(); v != n.end(); v++)
            {
                neighbours[*v].push_back(i);
            }
        }
        for (int i = 0; i < size; i++)
        {
            if (verts[i].cc != -1)
            {
                continue;
            }
            int cc = connectedComponents.size();
            connectedComponents.push_back(i);
            std::deque<int> q;
            q.push_back(i);
            verts[i].cc = cc;
            while (!q.empty())
            {
                IntVector n = neighbours[q.front()];
                for (IntVector::iterator v = n.begin(); v != n.end(); v++)
                {
                    if (verts[*v].cc == -1)
                    {
                        verts[*v].cc = cc;
                        q.push_back(*v);
                    }
                }
                q.pop_front();
            }
        }
    }

    int GetConnectedComponentsNumber()
    {
        if (connectedComponents.size() == 0)
        {
            Calculate0thHomologyGroup();
        }
        return connectedComponents.size();
    }

    void GetProjectionMap(RipsT &graph, std::map<int, int> &projection)
    {
        for (IntVector::iterator v = connectedComponents.begin(); v != connectedComponents.end(); v++)
        {
            projection[*v] = graph.connectedComponents[graph.verts[*v].cc];
        }
    }

    void PrintConnectedComponents(std::ostream &str)
    {
        std::cout<<"connected components:"<<std::endl;
        for (IntVector::iterator it = connectedComponents.begin(); it != connectedComponents.end(); it++)
        {
            str<<*it<<std::endl;
        }
    }

    void Print(std::ostream &str)
    {
        for (typename std::vector<Vertex>::iterator v = verts.begin(); v != verts.end(); v++)
        {
            std::cout<<v->label<<" = "<<*v->point<<std::endl;
        }
        for (std::vector<IntVector>::iterator s = simplices.begin(); s != simplices.end(); s++)
        {
            std::cout<<"["<<*s<<"]"<<std::endl;
        }
    }

    void DoTest(int label, double epsilon)
    {
        /*
        Vertex v = verts[label];
        Point *p1 = v.point;
        for (int i = 0; i < verts.size(); i++)
        {
            if (i == label)
            {
                continue;
            }
            Point *p2 = verts[i - 1].point;
            Point x1(4);
            Point x2(4);
            x1[0] = (*p1)[0];
            x1[1] = (*p1)[1];
            x1[2] = 0;
            x1[3] = 0;
            x2[0] = (*p2)[0];
            x2[1] = (*p2)[1];
            x2[2] = 0;
            x2[3] = 0;
            double distX = tmpMetric->Distance(x1, x2);
            x1[0] = (*p1)[2];
            x1[1] = (*p1)[3];
            x2[0] = (*p2)[2];
            x2[1] = (*p2)[3];
            double distY = tmpMetric->Distance(x1, x2);
            if (distX <= epsilon && distY > epsilon)
            {
                std::cout<<label<<" "<<i<<std::endl<<"["<<*p1<<"] ["<<*p2<<"]"<<std::endl<<distX<<" "<<distY<<std::endl;
                tmpMetric->Distance(x1, x2);
            }
        }
         * */
    }

    template <typename PointType1>
    friend std::ostream &operator<<(std::ostream &str, RipsT<PointType1> &rips);
};

template <typename PointType>
std::ostream &operator<<(std::ostream &str, RipsT<PointType> &rips)
{
    rips.Print(str);
    return str;
}

#endif	/* RIPS_HPP */

