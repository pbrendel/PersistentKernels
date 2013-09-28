/* 
 * File:   QualityFunction.hpp
 * Author: admin
 *
 * Created on October 16, 2012, 9:28 AM
 */

#ifndef QUALITYFUNCTION_HPP
#define	QUALITYFUNCTION_HPP

template <typename PersistenceData>
class QualityFunction
{
public:

    typedef typename PersistenceData::PersistenceDgmType PersistenceDgmType;

    virtual void Init(std::vector<PersistenceData *> &persistenceData, int filtrationsCount) = 0;
    virtual double Calculate(CRef<PersistenceDgmType> &dgm) = 0;
    virtual ~QualityFunction() { }

};

template <typename PersistenceData>
class QualityFunction1 : public QualityFunction<PersistenceData>
{

    int max;
    int min;

public:

    typedef typename PersistenceData::PersistenceDgmType PersistenceDgmType;

    void Init(std::vector<PersistenceData *> &persistenceData, int filtrationsCount)
    {
        max = 0;
        min = filtrationsCount;
        for (typename std::vector<PersistenceData *>::iterator i = persistenceData.begin(); i != persistenceData.end(); i++)
        {
            CRef<PersistenceDgmType> dgm = (*i)->GetPersistenceDgm();
            int longest = 0;
            BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
            {
                int diff = hc.death - hc.birth;
                if (diff > longest)
                {
                    longest = diff;
                }
            }
            if (longest < min && longest > 0) min = longest;
            if (longest > max) max = longest;
        }
        std::cout<<"min: "<<min<<" max: "<<max<<std::endl;
    }

    double Calculate(CRef<PersistenceDgmType> &dgm)
    {
        if (min == max)
        {
            return 0;
        }
        double value = min;
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
            double diff = hc.death - hc.birth;
            if (diff > value) value = diff;
        }
        return (value - min) / (max - min);
    }
};

template <typename PersistenceData>
class QualityFunction2 : public QualityFunction<PersistenceData>
{

    int max;
    int min;

public:

    typedef typename PersistenceData::PersistenceDgmType PersistenceDgmType;

    void Init(std::vector<PersistenceData *> &persistenceData, int filtrationsCount)
    {
        max = 0;
        min = filtrationsCount;
        for (typename std::vector<PersistenceData *>::iterator i = persistenceData.begin(); i != persistenceData.end(); i++)
        {
            CRef<PersistenceDgmType> dgm = (*i)->GetPersistenceDgm();
            int longest = 0;
            BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
            {
                int diff = hc.death - hc.birth;
                if (diff > longest)
                {
                    longest = diff;
                }
            }
            if (longest < min && longest > 0) min = longest;
            if (longest > max) max = longest;
        }
        std::cout<<"min: "<<min<<" max: "<<max<<std::endl;
    }

    double Calculate(CRef<PersistenceDgmType> &dgm)
    {
        if (min == max)
        {
            return 0;
        }
        double value = min;
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
            double diff = hc.death - hc.birth;
            if (diff > value) value = diff;
        }
        return (value == max) ? 1.0f : 0.0f;
    }
};

template <typename PersistenceData>
class QualityFunction3 : public QualityFunction<PersistenceData>
{
    int medium;
    int max;

public:

    typedef typename PersistenceData::PersistenceDgmType PersistenceDgmType;

    void Init(std::vector<PersistenceData *> &persistenceData, int filtrationsCount)
    {
        max = 0;
        std::map<int, int> histogram;
        for (typename std::vector<PersistenceData *>::iterator i = persistenceData.begin(); i != persistenceData.end(); i++)
        {
            CRef<PersistenceDgmType> dgm = (*i)->GetPersistenceDgm();
            int longest = 0;
            BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
            {
                int diff = hc.death - hc.birth;
                if (diff > longest)
                {
                    longest = diff;
                }
            }
            histogram[longest] = histogram[longest] + 1;
            if (longest > max)
            {
                max = longest;
            }
        }
        int m = 0;
        for (std::map<int, int>::iterator it = histogram.begin(); it != histogram.end(); it++)
        {
            if (it->second > m)
            {
                m = it->second;
                medium = it->first;
            }
        }
        std::cout<<"medium "<<medium<<" max "<<max<<std::endl;
    }

    double Calculate(CRef<PersistenceDgmType> &dgm)
    {
        int value = 0;
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
            int diff = hc.death - hc.birth;
            if (diff > value) value = diff;
        }
        if (value <= medium)
        {
            return 0;
        }
        return double(value - medium) / (max - medium);
    }
};

template <typename PersistenceData>
class QualityFunction4 : public QualityFunction<PersistenceData>
{

    int max;
    int min;

public:

    typedef typename PersistenceData::PersistenceDgmType PersistenceDgmType;

    void Init(std::vector<PersistenceData *> &persistenceData, int filtrationsCount)
    {
        max = 0;
        min = filtrationsCount;
        for (typename std::vector<PersistenceData *>::iterator i = persistenceData.begin(); i != persistenceData.end(); i++)
        {
            CRef<PersistenceDgmType> dgm = (*i)->GetPersistenceDgm();
            BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
            {
                int diff = hc.death - hc.birth;
                if (diff < min) min = diff;
                if (diff > max) max = diff;
            }
        }
    }

    double Calculate(CRef<PersistenceDgmType> &dgm)
    {
        if (min == max)
        {
            return 0;
        }
        double value = min;
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
            double diff = hc.death - hc.birth;
            if (diff > value) value = diff;
        }
        return (value == max) ? 1.0f : 0.0f;
    }
};

template <typename PersistenceData>
class QualityFunction5 : public QualityFunction<PersistenceData>
{

    int filtrationsCount;

public:

    typedef typename PersistenceData::PersistenceDgmType PersistenceDgmType;

    void Init(std::vector<PersistenceData *> &persistenceData, int filtrationsCount)
    {
        this->filtrationsCount = filtrationsCount;
    }

    double Calculate(CRef<PersistenceDgmType> &dgm)
    {
        if (filtrationsCount <= 0)
        {
            return 0.0;
        }
        BOOST_FOREACH(typename PersistenceDgmType::HomologyClass &hc, dgm().homologyClasses)
        {
         //   std::cout<<hc.death<<" "<<filtrationsCount<<std::endl;
        //    if (hc.death == filtrationsCount)
            {
                return 1.0;
            }
        }
        return 0.0;
    }
};

#endif	/* QUALITYFUNCTION_HPP */

