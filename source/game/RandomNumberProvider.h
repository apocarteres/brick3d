#pragma once

class RandomNumberProvider {
public:
    RandomNumberProvider();
    virtual ~RandomNumberProvider();
    virtual int next_int(int high_limit) = 0;
    virtual float next_float(float high_limit) = 0;
private:

};

