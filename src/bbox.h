#pragma once

#include <glm/glm.hpp>

#include "global.h"
#include "triangle.h"

template <int N> class BBox
{
private:
    /* data */
public:
    union 
    {
        struct 
        {
            flt x_min, x_max, y_min, y_max, z_min, z_max;
        };
        struct 
        {
            flt x[2], y[2], z[2];
        };
        struct 
        {
            flt a[3][2];
        };
    };
    
    BBox();
    BBox(const Triangle&);

    void update(const Triangle&);
    void update(const glm::vec<N, float, glm::defaultp>&);

    bool is_contain(const BBox<N>&);
};

// Explicitly declare used bbox class
template class BBox<3>;
template class BBox<2>;

typedef BBox<2> BBox2;
typedef BBox<3> BBox3;