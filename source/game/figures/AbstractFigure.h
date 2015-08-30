#pragma once

#include <vector>
#include <bitset>
#include <map>
#include <assert.h>

#define GEOMETRY_SIZE 4
#define MAX_ROTATION_INDEX 3

typedef std::bitset<GEOMETRY_SIZE> geometry_bits;
typedef std::vector<geometry_bits> geometry;

class AbstractFigure {
private:
    std::map<char, geometry> geo;
protected:
    virtual geometry make_geometry_top() = 0;
    virtual geometry make_geometry_right() = 0;
    virtual geometry make_geometry_bottom() = 0;
    virtual geometry make_geometry_left() = 0;
    
    virtual void init_geometry() {        
        geo[0] = make_geometry_right();
        geo[1] = make_geometry_bottom();
        geo[2] = make_geometry_left();
        geo[3] = make_geometry_top();
    }

public:
    AbstractFigure();
    virtual ~AbstractFigure();

    virtual geometry find_geometry(char i) {
        assert(geo.count(i) > 0);
        return geo[i];
    }
};
