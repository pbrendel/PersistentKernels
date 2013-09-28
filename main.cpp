/*
 * File:   main.cpp
 * Author: Piotr Brendel
 */

#include <iostream>
#include <vector>
#include <set>

#include "HomologyTraits.hpp"
#include "Persistence.hpp"
#include "KernelsPersistence.hpp"
#include "LocalKernelsPersistence.hpp"
#include "LocalKernels.hpp"

#include "Noise.hpp"
#include "Domain.hpp"
#include "Map.hpp"
#include "HorseshoeMap.hpp"

#include "MapSupplier.hpp"

#include "Tests.h"

#include <ctime>

int main(int argc, char *argv[])
{
    time_t t = time(0);
    Tests::DoTest(argc, argv);
    std::cout<<"finished in "<<(time(0) - t)<<" seconds"<<std::endl;

    return 0;
}
