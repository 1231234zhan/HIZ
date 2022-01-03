
#include <cstdio>

#include "glm/gtc/epsilon.hpp"
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "global.h"
#include "triangle.h"

Triangle::Triangle() { }

Triangle::Triangle(const vec3& p1, const vec3& p2, const vec3& p3)
{
    p[0] = p1;
    p[1] = p2;
    p[2] = p3;
}

void Triangle::transform_to_screen(const Camera& camera, const vec4& viewport)
{
    for (int i = 0; i < 3; i++) {
        p[i] = glm::project(p[i], mat4(1.0f), camera.perspective * camera.lookat, viewport);
    }
}

Color Triangle::color()
{
    vec3 vec = (p1() + p2() + p3()) / 3.0f;
    uchar r = static_cast<uchar>(256 * (vec.x - std::floor(vec.x)));
    uchar g = static_cast<uchar>(256 * (vec.y - std::floor(vec.y)));
    uchar b = static_cast<uchar>(256 * (vec.z - std::floor(vec.z)));
    return Color(r, g, b);
}