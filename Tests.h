// pbrendel (c) 2013-21

#pragma once

#include "cube.h"
#include "Core/ptr.h"

#include <string>

class Domain;
class Map;
class Metrics;
class Noise;
class QualityFunction;


class TestParams
{
public:

    TestParams( const std::string &params );

    void Print( std::ostream &str ) const;

    o::Ptr<Domain> CreateDomain() const;
    o::Ptr<Map> CreateMap( const Domain &domain, const Noise *noise ) const;
    o::Ptr<Noise> CreateNoise() const;
    o::Ptr<Metrics> CreateMetrics( const Domain &domain, const Map &map ) const;
    o::Ptr<Domain> CreateTestDomain() const;
    o::Ptr<QualityFunction> CreateQualityFunction() const;
    void CreateEpsilons( o::DynArray<double> &outEpsilons ) const;

    constexpr uint GetAlgorithmId() const
    {
        return m_algorithmId;
    }

    constexpr uint GetDomainDim() const
    {
        if ( m_mapType == MapType::Linear1d || m_mapType == MapType::Translation1d ) return 1;
        return 2;
    }

    constexpr uint GetRangeDim() const
    {
        if ( m_mapType == MapType::Linear1d || m_mapType == MapType::LinearDiscontinous || m_mapType == MapType::Translation1d ) return 1;
        return 2;
    }

    constexpr double GetAlpha() const
    {
        return m_alpha;
    }

    constexpr double GetBeta() const
    {
        return m_beta;
    }

    constexpr double GetRestrictionRadius() const
    {
        return m_restrictionRadius;
    }

    const std::string &GetOutputFilename() const
    {
        return m_outputFilename;
    }

    constexpr bool GetShowGraph() const
    {
        return m_showGraph;
    }

private:

    enum class DomainType : uint
    {
        Uniform,
        UniformWithHole,
        Random,
        RandomWithHole,
    };

    enum class MapType : uint
    {
        Linear1d,
        Linear2d,
        LinearDiscontinous,
        HorseshoeU,
        HorseshoeG,
        HorseshoeS,
        Translation1d,
        Translation2d,
    };

    enum class MetricsType : uint
    {
        Default, // Euclidean
        QuotientExitSet,
    };

    void CreateDomainCube( Cube &outCube ) const;
    bool ParseDouble( std::istream &stream, double &outValue ) const;
    bool ParseUint( std::istream &stream, uint &outValue ) const;
    bool ParseBool( std::istream &stream, bool &outValue ) const;
    bool ParseString( std::istream &stream, std::string &outValue ) const;
    void ParseParams( const std::string &params );
    void ParseDomain( std::istream &stream );
    void ParseMap( std::istream &stream );
    void ParseMetrics( std::istream &stream );
    void ParseTest( std::istream &stream );
    void ParseEpsilons( std::istream &stream );

    uint m_algorithmId;

    DomainType m_domainType;
    uint m_domainSize;
    Cube m_domainCube;

    MapType m_mapType;
    o::DynArray<double> m_mapParams;
    double m_noiseDelta;
    double m_alpha;
    double m_beta;

    MetricsType m_metricsType;

    DomainType m_testDomainType;
    uint m_testDomainSize;
    Interval m_epsilonsInterval;
    uint m_epsilonsCount;
    double m_restrictionRadius;

    uint m_qualityFunctionNumber;

    std::string m_outputFilename;

    bool m_showGraph;
};

////////////////////////////////////////////////////////////////////////////////

class Tests
{
public:

    static void Run( int argc, char **argv );

private:

    static void RunSingle( const std::string &paramsString );
    static void RunList( const std::string &filename );
    static bool ShowGraph( const std::string &filename );
};
