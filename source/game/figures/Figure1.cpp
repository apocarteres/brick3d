#include "Figure1.h"

Figure1::Figure1() {
    init_geometry();
}

geometry Figure1::make_geometry_top() {
    geometry g;
    geometry_bits b;
    b.set(1, 1);
    for (int i = 0; i < 4; ++i) {
        g.push_back(b);
    }
    return g;
}

geometry Figure1::make_geometry_right() {
    geometry g;
    geometry_bits r1, r2;
    r2.set(0, 1);
    r2.set(1, 1);
    r2.set(2, 1);
    r2.set(3, 1);
    g.push_back(r1);    
    g.push_back(r2);
    return g;
}

geometry Figure1::make_geometry_bottom() {
    return make_geometry_top();
}

geometry Figure1::make_geometry_left() {
    return make_geometry_right();
}



