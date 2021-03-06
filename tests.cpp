// pbrendel (c) 2013-21

#include "tests.h"
#include "dataWriter.h"
#include "domain.h"
#include "exitSetQuotientMetrics.h"
#include "horseshoeMap.h"
#include "localKernelsPersistence.h"
#include "map.h"
#include "noise.h"
#include "qualityFunction.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using o::DynArray;
using o::Ptr;


TestParams::TestParams( const std::string &params )
{
    m_domainType = TestParams::DomainType::Uniform;
    m_domainSize = 11;
    m_domainCube.SetDimension( 1 );
    m_domainCube[0] = Interval( 0, 1 );
    m_mapType = MapType::LinearDiscontinous;
    m_noiseDelta = 0;
    m_metricsType = MetricsType::Default;
    m_testDomainType = DomainType::Random;
    m_testDomainSize = 100;
    m_epsilonsInterval = Interval( 0.05, 0.2 );
    m_epsilonsCount = 20;
    m_alpha = 0.01;
    m_beta = 0.5;
    m_restrictionRadius = 0.2;
    m_qualityFunctionNumber = 1;
    m_filename = "output.txt";

    const char *separator = "--";
    size_t separatorSize = 2;
    size_t prevIndex = params.find( separator );
    while ( prevIndex != std::string::npos )
    {
        size_t index = params.find( separator, prevIndex + separatorSize );
        ParseParams( params.substr( prevIndex, index - prevIndex - 1 ) );
        prevIndex = index;
    }
}


void TestParams::Print( std::ostream &str ) const
{
    str << "domain " << static_cast<uint>( m_domainType ) << " " << m_domainSize << std::endl;
    for ( uint i = 0; i < m_domainCube.GetDimension(); ++i )
    {
        str << m_domainCube[i].m_min << " " << m_domainCube[i].m_max << std::endl;
    }
    str << "map " << static_cast<uint>( m_mapType ) << " ";
    for ( DynArray<double>::ConstIterator it = m_mapParams.Begin(); it != m_mapParams.End(); ++it )
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    str << "noise " << m_noiseDelta << std::endl;
    str << "metrics " << static_cast<uint>( m_metricsType ) << std::endl;
    str << "epsilons " << m_epsilonsInterval.m_min << " " << m_epsilonsInterval.m_max << " " << m_epsilonsCount << std::endl;
    str << "alpha " << m_alpha << std::endl;
    str << "beta " << m_beta << std::endl;
    str << "test domain " << static_cast<uint>( m_testDomainType ) << " " << m_testDomainSize << std::endl;
    str << "radius " << m_restrictionRadius << std::endl;
    str << "quality function: " << m_qualityFunctionNumber << std::endl;
    str << "filename: " << m_filename << std::endl;
}


Ptr<Domain> TestParams::CreateDomain() const
{
    Cube cube;
    CreateDomainCube( cube );
    const uint dim = GetDomainDim();
    if ( m_domainType == DomainType::Uniform )
    {
        DynArray<uint> resolution;
        for ( uint i = 0; i < dim; ++i )
        {
            resolution.PushBack( m_domainSize );
        }
        return new UniformCube( cube, resolution, nullptr );
    }
    else if ( m_domainType == DomainType::UniformWithHole )
    {
        DynArray<uint> resolution;
        for ( uint i = 0; i < dim; ++i )
        {
            resolution.PushBack( m_domainSize );
        }
        // test!!!
        Cube hole;
        hole.SetDimension( 2 );
        hole[0] = Interval( 0.33, 0.66 );
        hole[1] = Interval( 0, 1 );
        return new UniformCubeWithHole( cube, hole, resolution, nullptr );
    }
    else if ( m_domainType == DomainType::Random )
    {
        return new RandomCube( cube, m_domainSize, nullptr );
    }
    else
    {
        // test!!!
        Cube hole;
        hole.SetDimension( 2 );
        hole[0] = Interval( 0.33, 0.66 );
        hole[1] = Interval( 0, 1 );
        return new RandomCubeWithHole( cube, hole, m_domainSize, nullptr );
    }
}


Ptr<Map> TestParams::CreateMap( const Domain &domain, const Noise *noise ) const
{
    if ( m_mapType == MapType::Linear1d || m_mapType == MapType::Linear2d )
    {
        return new LinearMap( GetRangeDim(), m_mapParams, noise );
    }
    else if ( m_mapType == MapType::LinearDiscontinous )
    {
        return new TestDisc2To1( domain, m_mapParams[0], noise );
    }
    else if ( m_mapType == MapType::HorseshoeU )
    {
        return new HorseshoeU( 0.1, 0.1, domain, m_mapParams[0], noise );
    }
    else if ( m_mapType == MapType::HorseshoeS )
    {
        return new HorseshoeS( 0.1, 0.1, domain, m_mapParams[0], noise );
    }
    else if ( m_mapType == MapType::HorseshoeG )
    {
        return new HorseshoeG( 0.1, 0.1, domain, m_mapParams[0], noise );
    }
    else if ( m_mapType == MapType::Translation1d || m_mapType == MapType::Translation2d )
    {
        Point offset( GetRangeDim() );
        offset[0] = m_mapParams[0];
        if ( offset.GetDimension() > 1 )
        {
            offset[1] = m_mapParams[1];
        }
        return new Translation( GetRangeDim(), offset, noise );
    }
    return nullptr;
}


Ptr<Noise> TestParams::CreateNoise() const
{
    const uint dim = GetRangeDim();
    DynArray<double> deltas;
    for ( uint i = 0; i < dim; ++i )
    {
        deltas.PushBack( m_noiseDelta );
    }
    return new Noise( deltas );
}


Ptr<Metrics> TestParams::CreateMetrics( const Domain &domain, const Map &map ) const
{
    if ( m_metricsType == MetricsType::QuotientExitSet )
    {
        return new ExitSetQuotientMetrics( domain, map, EuclideanMetrics::Get() );
    }
    else
    {
        return new EuclideanMetrics();
    }
}


Ptr<Domain> TestParams::CreateTestDomain() const
{
    Cube cube;
    CreateDomainCube( cube );
    const uint dim = GetDomainDim();
    if ( m_testDomainType == DomainType::Uniform )
    {
        DynArray<uint> resolution;
        for ( uint i = 0; i < dim; ++i )
        {
            resolution.PushBack( m_testDomainSize );
        }
        return new UniformCube( cube, resolution, nullptr );
    }
    else
    {
        return new RandomCube( cube, m_testDomainSize, nullptr );
    }
}


Ptr<QualityFunction> TestParams::CreateQualityFunction() const
{
    if ( m_qualityFunctionNumber == 2 ) return new QualityFunction2();
    if ( m_qualityFunctionNumber == 3 ) return new QualityFunction3();
    if ( m_qualityFunctionNumber == 4 ) return new QualityFunction4();
    if ( m_qualityFunctionNumber == 5 ) return new QualityFunction5();
    return new QualityFunction1();
}


void TestParams::CreateEpsilons( DynArray<double> &outEpsilons ) const
{
    double e = m_epsilonsInterval.m_min;
    const double delta = ( m_epsilonsInterval.m_max - m_epsilonsInterval.m_min ) / ( m_epsilonsCount - 1 );
    for ( uint i = 0; i < m_epsilonsCount; ++i )
    {
        outEpsilons.PushBack( e );
        e += delta;
    }
}


void TestParams::CreateDomainCube( Cube &outCube ) const
{
    const uint domainDim = GetDomainDim();
    const uint domainCubeDim = m_domainCube.GetDimension();
    outCube.SetDimension( domainDim );
    for ( uint i = 0; i < domainCubeDim; ++i )
    {
        outCube[i] = m_domainCube[i];
    }
    const Interval defaultInterval = ( domainCubeDim > 0 ) ? m_domainCube[domainCubeDim - 1] : Interval( 0, 1 );
    for ( uint i = domainCubeDim; i < domainDim; ++i )
    {
        outCube[i] = defaultInterval;
    }
}


bool TestParams::ParseDouble( std::istream &stream, double &outValue ) const
{
    double v;
    if ( stream >> v )
    {
        outValue = v;
        return true;
    }
    return false;
}


bool TestParams::ParseUint( std::istream &stream, uint &outValue ) const
{
    uint v;
    if ( stream >> v )
    {
        outValue = v;
        return true;
    }
    return false;
}


bool TestParams::ParseString( std::istream &stream, std::string &outValue ) const
{
    std::string v;
    if ( stream >> v )
    {
        outValue = v;
        return true;
    }
    return false;
}


void TestParams::ParseParams( const std::string &params )
{
    std::istringstream stream( params );
    std::string str;
    stream >> str;
    if ( str == "--domain" )
    {
        ParseDomain( stream );
    }
    else if ( str == "--map" )
    {
        ParseMap( stream );
    }
    else if ( str == "--noise" )
    {
        ParseDouble( stream, m_noiseDelta );
    }
    else if ( str == "--metrics" )
    {
        ParseMetrics( stream );
    }
    else if ( str == "--test" )
    {
        ParseTest( stream );
    }
    else if ( str == "--epsilons" )
    {
        ParseEpsilons( stream );
    }
    else if ( str == "--alpha" )
    {
        ParseDouble( stream, m_alpha );
    }
    else if ( str == "--beta" )
    {
        ParseDouble( stream, m_beta );
    }
    else if ( str == "--radius" )
    {
        ParseDouble( stream, m_restrictionRadius );
    }
    else if ( str == "--quality" )
    {
        ParseUint( stream, m_qualityFunctionNumber );
    }
    else if ( str == "--out" )
    {
        ParseString( stream, m_filename );
    }
}


void TestParams::ParseDomain( std::istream &stream )
{
    std::string str;
    if ( stream >> str )
    {
        if ( str == "uniform" )
        {
            m_domainType = DomainType::Uniform;
        }
        else if ( str == "uniform_with_hole" )
        {
            m_domainType = DomainType::UniformWithHole;
        }
        else if ( str == "random" )
        {
            m_domainType = DomainType::Random;
        }
        else if ( str == "random_with_hole" )
        {
            m_domainType = DomainType::RandomWithHole;
        }
        else
        {
            std::cout << "error parsing params: unknown domain type: " << str << std::endl;
        }
    }
    ParseUint( stream, m_domainSize );
    m_domainCube.Clear();
    double min;
    while ( stream >> min )
    {
        double max;
        if ( !ParseDouble( stream, max ) )
        {
            max = min + 1;
        }
        m_domainCube.AddDimension( Interval( min, max ) );
    }
    if ( m_domainCube.GetDimension() == 0 )
    {
        m_domainCube.AddDimension( Interval( 0, 1 ) );
    }
}


void TestParams::ParseMap( std::istream &stream )
{
    std::string str;
    if ( stream >> str )
    {
        if ( str == "linear1d" )
        {
            m_mapParams.PushBack( 1.0 );
            m_mapType = MapType::Linear1d;
            ParseDouble( stream, m_mapParams[0] );
        }
        else if ( str == "linear2d" )
        {
            m_mapParams.PushBack( 1.0 );
            m_mapParams.PushBack( 0.0 );
            m_mapType = MapType::Linear2d;
            ParseDouble( stream, m_mapParams[0] );
            ParseDouble( stream, m_mapParams[1] );
        }
        else if ( str == "linear_discontinous" )
        {
            m_mapParams.PushBack( 1.0 );
            m_mapType = MapType::LinearDiscontinous;
            ParseDouble( stream, m_mapParams[0] );
        }
        else if ( str == "horseshoe_u" )
        {
            m_mapParams.PushBack( 0.0 );
            m_mapType = MapType::HorseshoeU;
            ParseDouble( stream, m_mapParams[0] );
        }
        else if ( str == "horseshoe_s" )
        {
            m_mapParams.PushBack( 0.0 );
            m_mapType = MapType::HorseshoeS;
            ParseDouble( stream, m_mapParams[0] );
        }
        else if ( str == "horseshoe_g" )
        {
            m_mapParams.PushBack( 0.0 );
            m_mapType = MapType::HorseshoeG;
            ParseDouble( stream, m_mapParams[0] );
        }
        else if ( str == "translation1d" )
        {
            m_mapParams.PushBack( 0.1 );
            m_mapType = MapType::Translation1d;
            ParseDouble( stream, m_mapParams[0] );
        }
        else if ( str == "translation2d" )
        {
            m_mapParams.PushBack( 0.1 );
            m_mapParams.PushBack( 0.0 );
            m_mapType = MapType::Translation2d;
            ParseDouble( stream, m_mapParams[0] );
            ParseDouble( stream, m_mapParams[1] );
        }
        else
        {
            std::cout << "error parsing params: unknown map type: " << str << std::endl;
        }
    }
}


void TestParams::ParseMetrics( std::istream &stream )
{
    std::string str;
    if ( stream >> str )
    {
        if ( str == "default" || str == "euclidean" )
        {
            m_metricsType = MetricsType::Default;
        }
        else if ( str == "quotient_exit_set" )
        {
            m_metricsType = MetricsType::QuotientExitSet;
        }
        else
        {
            std::cout << "error parsing params: unknown metrics type: " << str << std::endl;
        }
    }
}


void TestParams::ParseTest( std::istream &stream )
{
    std::string str;
    if ( stream >> str )
    {
        if ( str == "uniform" )
        {
            m_testDomainType = DomainType::Uniform;
        }
        else if ( str == "random" )
        {
            m_testDomainType = DomainType::Random;
        }
        else
        {
            std::cout << "error parsing params: unknown test domain type: " << str << std::endl;
        }
    }
    ParseUint( stream, m_testDomainSize );
}


void TestParams::ParseEpsilons( std::istream &stream )
{
    double min;
    double max;
    if ( ParseDouble( stream, min ) )
    {
        if ( !ParseDouble( stream, max ) )
        {
            max = min + 1;
        }
        m_epsilonsInterval = Interval( min, max );
        ParseUint( stream, m_epsilonsCount );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Tests::Run( int argc, char **argv )
{
    if ( argc < 2 )
    {
        std::cout << "usage: program_name tests_list.txt|test_params" << std::endl;
    }
    else if ( argc == 2 )
    {
        RunList( std::string( argv[1] ) );
    }
    else
    {
        std::ostringstream stream;
        for ( int i = 1; i < argc; i++ )
        {
            stream << argv[i] << " ";
        }
        RunSingle( stream.str() );
    }
}


void Tests::RunSingle( const std::string &paramsString )
{
    srand( static_cast<uint>( time( 0 ) ) );

    TestParams testParams( paramsString );
    testParams.Print( std::cout );

    Ptr<Domain> domain = testParams.CreateDomain();
    Ptr<Noise> noise = testParams.CreateNoise();
    Ptr<Map> map = testParams.CreateMap( *domain, noise.Get() );
    Ptr<Metrics> domainMetrics = testParams.CreateMetrics( *domain, *map );
    Ptr<Metrics> graphMetrics = new MaxDomainRangeMetrics( *domainMetrics, *domainMetrics, domain->GetDimension(), map->GetDimension() );
    Ptr<Domain> testDomain = testParams.CreateTestDomain();
    Ptr<QualityFunction> qualityFunction = testParams.CreateQualityFunction();
    DynArray<double> epsilons;
    testParams.CreateEpsilons( epsilons );

    std::ofstream output( testParams.GetFilename().c_str() );

    PersistenceData persistenceData;
    LocalKernelsPersistence::Compute_Alg1( *domain, *map, *testDomain, epsilons, testParams.GetRestrictionRadius(), *domainMetrics, *graphMetrics, persistenceData );

    qualityFunction->Init( persistenceData, epsilons.GetSize() );
    for ( PersistenceData::Iterator i = persistenceData.Begin(); i != persistenceData.End(); ++i )
    {
        i->ApplyMap( *map );
        i->CalculateQuality( *qualityFunction );
        
        output << *i;
    }

    output.close();
}


void Tests::RunList( const std::string &filename )
{
    std::ifstream input( filename.c_str() );
    if ( !input.is_open() )
    {
        std::cout << "cannot open file " << filename << std::endl;
        return;
    }
    std::string line;
    while ( getline( input, line ) )
    {
        RunSingle( line );
    }
}
