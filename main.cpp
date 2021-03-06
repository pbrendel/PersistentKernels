// pbrendel (c) 2013-21

#include "Tests.h"
#include "Core/perfCounter.h"

#include <iostream>

int main( int argc, char *argv[] )
{
    PerfCounter pc;
    pc.Reset();
    Tests::Run( argc, argv );
    std::cout << "finished in " << pc.Reset() << " seconds" << std::endl;

    return 0;
}
