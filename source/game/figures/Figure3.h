#pragma once

#include "AbstractFigure.h"

class Figure3 : public AbstractFigure {
protected:
    virtual geometry make_geometry_top();
    virtual geometry make_geometry_right();
    virtual geometry make_geometry_bottom();
    virtual geometry make_geometry_left();

public:
    Figure3();
};

