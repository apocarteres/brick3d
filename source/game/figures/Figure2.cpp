#include "Figure2.h"

Figure2::Figure2() {
    init_geometry();
}

geometry Figure2::make_geometry_top() {
    geometry g;
    geometry_bits b;
    b.set(0, 1);
    b.set(1, 1);
    for (int i = 0; i < 2; ++i) {
        g.push_back(b);
    }
    return g;
}

geometry Figure2::make_geometry_right() {
    return make_geometry_top();
}

geometry Figure2::make_geometry_bottom() {
    return make_geometry_top();
}

geometry Figure2::make_geometry_left() {
    return make_geometry_top();
}



