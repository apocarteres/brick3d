#include "StdLibRandomProvider.h"

StdLibRandomProvider::StdLibRandomProvider() {
    rnd_e.seed(std::time(0));
}

StdLibRandomProvider::~StdLibRandomProvider() {
}

int StdLibRandomProvider::next_int(int high_limit) {
    std::uniform_int_distribution<int> d(1, high_limit - 1);
    return d(rnd_e);
}

float StdLibRandomProvider::next_float(float high_limit) {
    std::uniform_real_distribution<float> d(0.0f, high_limit);
    return d(rnd_e);

}

