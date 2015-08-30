#include "MockRandomNumberProvider.h"

MockRandomNumberProvider::MockRandomNumberProvider(int number) {
    this->number = number;
}

MockRandomNumberProvider::~MockRandomNumberProvider() {
}

int MockRandomNumberProvider::next_int(int high_limit){
    return number;
}

float MockRandomNumberProvider::next_float(float high_limit){
    return 1.0;
}

