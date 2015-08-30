#include "Figure4.h"

Figure4::Figure4() {
    init_geometry();
}

geometry Figure4::make_geometry_top() {
    geometry g;
    geometry_bits r1;
    r1.set(1, 1);
    g.push_back(r1);
    geometry_bits r2;
    r2.set(0, 1);
    r2.set(1, 1);
    g.push_back(r2);
    geometry_bits r3;
    r3.set(0, 1);
    g.push_back(r3);
    return g;
}

geometry Figure4::make_geometry_right() {
    geometry g;
    geometry_bits r1;
    r1.set(1, 1);
    r1.set(2, 1);
    g.push_back(r1);

    geometry_bits r2;
    r2.set(0, 1);
    r2.set(1, 1);
    g.push_back(r2);
    return g;
}

geometry Figure4::make_geometry_bottom() {
    geometry g;
    geometry_bits r1;
    r1.set(0, 1);
    g.push_back(r1);

    geometry_bits r2;
    r2.set(0, 1);
    r2.set(1, 1);
    g.push_back(r2);

    geometry_bits r3;
    r3.set(1, 1);
    g.push_back(r3);
    return g;
}

geometry Figure4::make_geometry_left() {
    geometry g;
    geometry_bits r1;
    r1.set(0, 1);
    r1.set(1, 1);
    g.push_back(r1);

    geometry_bits r2;
    r2.set(1, 1);
    r2.set(2, 1);
    g.push_back(r2);
    return g;
}



