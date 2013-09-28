/* 
 * File:   Tests.h
 * Author: Piotr Brendel
 */

#ifndef TESTS_H
#define	TESTS_H

#include "Interval.hpp"
#include "Cube.hpp"
#include "Domain.hpp"
#include "Map.hpp"
#include "HorseshoeMap.hpp"
#include "Persistence.hpp"
#include "QualityFunction.hpp"
//#include "LocalKernelsPersistence.hpp"
#include "LocalKernelsPersistenceNew.hpp"
#include <ctime>

template <typename LocalKernelsPersistence>
class TestParams
{
public:

    typedef typename LocalKernelsPersistence::PersistenceData PersistenceData;

private:

    enum DomainType
    {
        DomainTypeUniform,
        DomainTypeUniformWithHole,
        DomainTypeRandom,
        DomainTypeRandomWithHole,
    };

    enum MapType
    {
        MapLinear1d,
        MapLinear2d,
        MapLinearDiscontinous,
        MapHorseshoeU,
        MapHorseshoeG,
        MapHorseshoeS,
        MapTranslation1d,
        MapTranslation2d,
    };

    enum MetricType
    {
        MetricTypeDefault, // Euclidean
        MetricTypeQuotientExitSet,
    };

    DomainType domainType;
    int domainSize;
    Cube domainCube;

    MapType mapType;
    std::vector<double> mapParams;
    double noiseDelta;
    double alpha;
    double beta;

    MetricType metricType;

    DomainType testDomainType;
    int testDomainSize;
    Interval epsilonsInterval;
    int epsilonsCount;
    double restrictionRadius;

    int qualityFunctionNumber;

    std::string filename;

    EuclideanMetric defaultMetric;

public:

    TestParams(std::string params)
    {
        domainType = DomainTypeUniform;
        domainSize = 11;
        domainCube.push_back(Interval(0, 1));
        mapType = MapLinearDiscontinous;
        noiseDelta = 0;
        metricType = MetricTypeDefault;
        testDomainType = DomainTypeRandom;
        testDomainSize = 100;
        epsilonsInterval = Interval(0.05, 0.2);
        epsilonsCount = 20;
        alpha = 0.01;
        beta = 0.5;
        restrictionRadius = 0.2;
        qualityFunctionNumber = 1;
        filename = "output.txt";

        const char *separator = "--";
        size_t separatorSize = 2;
        size_t prevIndex = params.find(separator);
        while (prevIndex != std::string::npos)
        {
            size_t index = params.find(separator, prevIndex + separatorSize);
            ParseParams(params.substr(prevIndex, index - prevIndex - 1));
            prevIndex = index;
        }
    }

    int GetDomainDim()
    {
        if (mapType == MapLinear1d || mapType == MapTranslation1d) return 1;
        else return 2;
    }

    int GetRangeDim()
    {
        if (mapType == MapLinear1d || mapType == MapLinearDiscontinous || mapType == MapTranslation1d) return 1;
        else return 2;
    }

    void CreateDomainCube(Cube &cube)
    {
        cube.clear();
        Interval defaultInterval = (domainCube.size() > 0) ? domainCube[domainCube.size() - 1] : Interval(0, 1);
        for (int i = 0; i < domainCube.size(); i++)
        {
            cube.push_back(domainCube[i]);
        }
        int dim = GetDomainDim();
        while (cube.size() < dim)
        {
            cube.push_back(defaultInterval);
        }
    }

    Domain *CreateDomain()
    {
        Cube cube;
        CreateDomainCube(cube);
        int dim = GetDomainDim();
        if (domainType == DomainTypeUniform)
        {
            std::vector<int> resolution;
            for (int i = 0; i < dim; i++)
            {
                resolution.push_back(domainSize);
            }
            return new UniformCube(dim, cube, resolution);
        }
        else if (domainType == DomainTypeUniformWithHole)
        {
            std::vector<int> resolution;
            for (int i = 0; i < dim; i++)
            {
                resolution.push_back(domainSize);
            }
            // test!!!
            Cube hole;
            hole.push_back(Interval(0.33, 0.66));
            hole.push_back(Interval(0, 1));
            return new UniformCubeWithHole(dim, cube, hole, resolution);
        }
        else if (domainType == DomainTypeRandom)
        {
            return new RandomCube(dim, cube, domainSize);
        }
        else
        {
            // test!!!
            Cube hole;
            hole.push_back(Interval(0.33, 0.66));
            hole.push_back(Interval(0, 1));
            return new RandomCubeWithHole(dim, cube, hole, domainSize);
        }
    }

    Map *CreateMap(Domain *domain, Noise *noise)
    {
        if (mapType == MapLinear1d || mapType == MapLinear2d)
        {
            return new LinearMap(GetRangeDim(), mapParams, noise);
        }
        else if (mapType == MapLinearDiscontinous)
        {
            return new TestDisc2To1(domain, mapParams[0], noise);
        }
        else if (mapType == MapHorseshoeU)
        {
            return new HorseshoeU(0.1, 0.1, domain, mapParams[0], noise);
        }
        else if (mapType == MapHorseshoeS)
        {
            return new HorseshoeS(0.1, 0.1, domain, mapParams[0], noise);
        }
        else if (mapType == MapHorseshoeG)
        {
            return new HorseshoeG(0.1, 0.1, domain, mapParams[0], noise);
        }
        if (mapType == MapTranslation1d || mapType == MapTranslation2d)
        {
            return new Translation(GetRangeDim(), mapParams, noise);
        }
    }

    Noise *CreateNoise()
    {
        int dim = GetRangeDim();
        std::vector<double> deltas;
        for (int i = 0; i < dim; i++)
        {
            deltas.push_back(noiseDelta);
        }
        return new Noise(dim, deltas);
    }

private:

    Metric *GetQuotientExitSetMetric(Map *map)
    {
        HorseshoeMap *horseshoeMap = dynamic_cast<HorseshoeMap*>(map);
        if (horseshoeMap != 0)
        {
            return horseshoeMap->GetExitSetQuotientMetric();
        }
        return &defaultMetric;
    }

public:

    Metric *CreateMetric(Domain *domain, Map *map)
    {
        if (metricType == MetricTypeQuotientExitSet)
        {
            return new ExitSetQuotientMetric(domain, map);
        }
        else
        {
            return new EuclideanMetric();
        }
    }
    
    Domain *CreateTestDomain()
    {
        Cube cube;
        CreateDomainCube(cube);
        int dim = GetDomainDim();
        if (testDomainType == DomainTypeUniform)
        {
            std::vector<int> resolution;
            for (int i = 0; i < dim; i++)
            {
                resolution.push_back(testDomainSize);
            }
            return new UniformCube(dim, cube, resolution);
        }
        else
        {
            return new RandomCube(dim, cube, testDomainSize);
        }
    }

    void CreateEpsilons(std::vector<double> &epsilons)
    {
        double e = epsilonsInterval.min;
        float delta = (epsilonsInterval.max - epsilonsInterval.min) / (epsilonsCount - 1);
        for (int i = 0; i < epsilonsCount; i++)
        {
            epsilons.push_back(e);
            e += delta;
        }
    }

    double GetAlpha()
    {
        return alpha;
    }

    double GetBeta()
    {
        return beta;
    }

    double GetRestrictionRadius()
    {
        return restrictionRadius;
    }

    QualityFunction<PersistenceData> *GetQualityFunction()
    {
        if (qualityFunctionNumber == 2) return new QualityFunction2<PersistenceData>();
        if (qualityFunctionNumber == 3) return new QualityFunction3<PersistenceData>();
        if (qualityFunctionNumber == 4) return new QualityFunction4<PersistenceData>();
        if (qualityFunctionNumber == 5) return new QualityFunction5<PersistenceData>();
        return new QualityFunction1<PersistenceData>();
    }

    std::string GetFilename()
    {
        return filename;
    }

    void Print(std::ostream &str)
    {
        str<<"domain "<<domainType<<" "<<domainSize<<std::endl;
        for (int i = 0; i < domainCube.size(); i++)
        {
            str<<domainCube[i].min<<" "<<domainCube[i].max<<std::endl;
        }
        str<<"map "<<mapType<<" ";
        for (std::vector<double>::iterator it = mapParams.begin(); it != mapParams.end(); it++)
        {
            std::cout<<*it<<" ";
        }
        std::cout<<std::endl;
        str<<"noise "<<noiseDelta<<std::endl;
        str<<"metric "<<metricType<<std::endl;
        str<<"epsilons "<<epsilonsInterval.min<<" "<<epsilonsInterval.max<<" "<<epsilonsCount<<std::endl;
        str<<"alpha "<<alpha<<std::endl;
        str<<"beta "<<beta<<std::endl;
        str<<"test domain "<<testDomainType<<" "<<testDomainSize<<std::endl;
        str<<"radius "<<restrictionRadius<<std::endl;
        str<<"quality function: "<<qualityFunctionNumber<<std::endl;
        str<<"filename: "<<filename<<std::endl;
    }

private:

    void ParseDouble(std::istream &stream, double &value)
    {
        double v;
        if (stream >> v)
        {
            value = v;
        }
    }

    void ParseInt(std::istream &stream, int &value)
    {
        int v;
        if (stream >> v)
        {
            value = v;
        }
    }

    void ParseString(std::istream &stream, std::string &value)
    {
        std::string v;
        if (stream >> v)
        {
            value = v;
        }
    }

    void ParseParams(std::string params)
    {
        std::istringstream stream(params);
        std::string str;
        stream >> str;
        if (str == "--domain")
        {
            ParseDomain(stream);
        }
        else if (str == "--map")
        {
            ParseMap(stream);
        }
        else if (str == "--noise")
        {
            ParseDouble(stream, this->noiseDelta);
        }
        else if (str == "--metric")
        {
            ParseMetric(stream);
        }
        else if (str == "--test")
        {
            ParseTest(stream);
        }
        else if (str == "--epsilons")
        {
            ParseEpsilons(stream);
        }
        else if (str == "--alpha")
        {
            ParseDouble(stream, this->alpha);
        }
        else if (str == "--beta")
        {
            ParseDouble(stream, this->beta);
        }
        else if (str == "--radius")
        {
            ParseDouble(stream, this->restrictionRadius);
        }
        else if (str == "--quality")
        {
            ParseInt(stream, this->qualityFunctionNumber);
        }
        else if (str == "--out")
        {
            ParseString(stream, this->filename);
        }
    }

    void ParseDomain(std::istream &stream)
    {
        std::string str;
        if (stream >> str)
        {
            if (str == "uniform")
            {
                domainType = DomainTypeUniform;
            }
            else if (str == "uniform_with_hole")
            {
                domainType = DomainTypeUniformWithHole;
            }
            else if (str == "random")
            {
                domainType = DomainTypeRandom;
            }
            else if (str == "random_with_hole")
            {
                domainType = DomainTypeRandomWithHole;
            }
            else
            {
                std::cout<<"error parsing params: unknown domain type: "<<str<<std::endl;
            }
        }
        int size;
        if (stream >> size)
        {
            domainSize = size;
        }
        domainCube.clear();
        double min;
        while (stream >> min)
        {
            double max;
            if (!(stream >> max))
            {
                max = min + 1;
            }
            domainCube.push_back(Interval(min, max));
        }
        if (domainCube.size() == 0)
        {
            domainCube.push_back(Interval(0, 1));
        }
    }

    void ParseMap(std::istream &stream)
    {
        std::string str;
        if (stream >> str)
        {
            if (str == "linear1d")
            {
                mapParams.push_back(1.0);
                mapType = MapLinear1d;
                ParseDouble(stream, mapParams[0]);
            }
            else if (str == "linear2d")
            {
                mapParams.push_back(1.0);
                mapParams.push_back(0.0);
                mapType = MapLinear2d;
                ParseDouble(stream, mapParams[0]);
                ParseDouble(stream, mapParams[1]);
            }
            else if (str == "linear_discontinous")
            {
                mapParams.push_back(1.0);
                mapType = MapLinearDiscontinous;
                ParseDouble(stream, mapParams[0]);
            }
            else if (str == "horseshoe_u")
            {
                mapParams.push_back(0.0);
                mapType = MapHorseshoeU;
                ParseDouble(stream, mapParams[0]);
            }
            else if (str == "horseshoe_s")
            {
                mapParams.push_back(0.0);
                mapType = MapHorseshoeS;
                ParseDouble(stream, mapParams[0]);
            }
            else if (str == "horseshoe_g")
            {
                mapParams.push_back(0.0);
                mapType = MapHorseshoeG;
                ParseDouble(stream, mapParams[0]);
            }
            else if (str == "translation1d")
            {
                mapParams.push_back(0.1);
                mapType = MapTranslation1d;
                ParseDouble(stream, mapParams[0]);
            }
            else if (str == "translation2d")
            {
                mapParams.push_back(0.1);
                mapParams.push_back(0.0);
                mapType = MapTranslation2d;
                ParseDouble(stream, mapParams[0]);
                ParseDouble(stream, mapParams[1]);
            }
            else
            {
                std::cout<<"error parsing params: unknown map type: "<<str<<std::endl;
            }
        }
    }
    
    void ParseMetric(std::istream &stream)
    {
        std::string str;
        if (stream >> str)
        {
            if (str == "default" || str == "euclidean")
            {
                metricType = MetricTypeDefault;
            }
            else if (str == "quotient_exit_set")
            {
                metricType = MetricTypeQuotientExitSet;
            }
            else
            {
                std::cout<<"error parsing params: unknown metric type: "<<str<<std::endl;
            }
        }
    }

    void ParseTest(std::istream &stream)
    {
        std::string str;
        if (stream >> str)
        {
            if (str == "uniform")
            {
                testDomainType = DomainTypeUniform;
            }
            else if (str == "random")
            {
                testDomainType = DomainTypeRandom;
            }
            else
            {
                std::cout<<"error parsing params: unknown test domain type: "<<str<<std::endl;
            }
        }
        int size;
        if (stream >> size)
        {
            testDomainSize = size;
        }
    }

    void ParseEpsilons(std::istream &stream)
    {
        double min;
        double max;
        if (stream >> min)
        {
            if (stream >> max)
            {

            }
            else
            {
                max = min + 1;
            }
            epsilonsInterval = Interval(min, max);
            int count;
            if (stream >> count)
            {
                epsilonsCount = count;
            }
        }
    }
};

class Tests
{
    typedef LocalKernelsPersistenceFastT<HomologyTraits> LocalKernelsPersistence;
    typedef LocalKernelsPersistence::PersistenceData PersistenceData;

public:

    static void SingleTest(std::string paramsString)
    {
        srand(time(0));

        TestParams<LocalKernelsPersistence> params(paramsString);
        params.Print(std::cout);

        Domain *domain = params.CreateDomain();
        Noise *noise = params.CreateNoise();
        Map *map = params.CreateMap(domain, noise);
        Metric *metric = params.CreateMetric(domain, map);
        Metric *graphMetric = new MaxDomainRangeMetric(metric, domain->GetDimension(), metric, map->GetDimension());
        Domain *testDomain = params.CreateTestDomain();
        std::vector<double> epsilons;
        params.CreateEpsilons(epsilons);
        QualityFunction<PersistenceData> *qualityFunction = params.GetQualityFunction();

        std::ofstream output(params.GetFilename().c_str());

        LocalKernelsPersistence lkp;
//        std::vector<PersistenceData *> persistenceData = lkp.Compute(domain, map, testDomain, epsilons, params.GetRestrictionRadius(), domainMetric, graphMetric);
        std::vector<PersistenceData *> persistenceData = lkp.Compute(domain, map, params.GetAlpha(), params.GetBeta(), metric);
        qualityFunction->Init(persistenceData, epsilons.size());
        for (std::vector<PersistenceData *>::iterator i = persistenceData.begin(); i != persistenceData.end(); i++)
        {
            (*i)->ApplyMap(map);
            (*i)->CalculateQuality(qualityFunction);
            (*i)->Print(output);
            //(*i)->PrintExitSet(output, dynamic_cast<HorseshoeExitSetQuotientMetric*>(domainMetric));
        }

        output.close();

        delete domain;
        delete map;
        delete metric;
        delete graphMetric;
        delete noise;
        delete qualityFunction;

        for (std::vector<PersistenceData *>::iterator i = persistenceData.begin(); i != persistenceData.end(); i++)
        {
            delete (*i);
        }
    }

    static void TestList(std::string filename)
    {
        std::ifstream input(filename.c_str());
        if (!input.is_open())
        {
            std::cout<<"cannot open file "<<filename<<std::endl;
            return;
        }
        std::string line;
        while (getline(input, line))
        {
            SingleTest(line);
        }
    }

    static void DoTest(int argc, char **argv)
    {
        if (argc < 2)
        {
            std::cout<<"usage: program_name tests_list.txt|test_params"<<std::endl;
        }
        else if (argc == 2)
        {
            TestList(std::string(argv[1]));
        }
        else
        {
            std::ostringstream stream;
            for (int i = 1; i < argc; i++)
            {
                stream<<argv[i]<<" ";
            }
            SingleTest(stream.str());
        }
    }
};

#endif	/* TESTS_H */

