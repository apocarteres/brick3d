#pragma once

#include "RandomNumberProvider.h"

class MockRandomNumberProvider : public RandomNumberProvider {
public:
    MockRandomNumberProvider(int number = 1);
    virtual ~MockRandomNumberProvider();
    virtual int next_int(int );
    virtual float next_float(float);
private:
    int number;
};

