#pragma once

#include "global.h"
#include "misc.h"

class Triangle {
private:
    /* data */
public:
    vec3 p[3];

    Triangle();
    Triangle(const vec3&, const vec3&, const vec3&);

    inline const vec3& p1() const { return this->p[0]; }
    inline const vec3& p2() const { return this->p[1]; }
    inline const vec3& p3() const { return this->p[2]; }
    void transform_to_screen(const Camera& camera, const vec4& viewport);
    
    // Paint an triangle face depending on barycentric coordinates
    Color color();
};
