#pragma once

#include <random>
#include <cstdlib>
#include <ctime>
#include "RandomNumberProvider.h"

class StdLibRandomProvider : public RandomNumberProvider{
public:
    StdLibRandomProvider();
    virtual ~StdLibRandomProvider();
    virtual int next_int(int high_limit);
    virtual float next_float(float high_limit);
private:
    std::default_random_engine rnd_e;
};

