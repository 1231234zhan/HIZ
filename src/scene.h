#pragma once

#include <vector>

#include "global.h"
#include "tiny_obj_loader.h"
#include "triangle.h"
#include "zbuf.h"

class Scene {
private:
    /* data */
public:
    std::vector<Triangle> triangles;
    ZBuf buffer;

    Scene(int, int);
    Scene(int, int, const tinyobj::ObjReader&);

    void culling_faces(const std::vector<int>&);
    
    // called by `hierarchical_octree_zbuffer()`
    // Recursively partion space and do occlusion
    void octree_occlusion(const std::vector<int>&);

    void transform_to_screen(const Camera& camera);

    void zbuffer();
    void hierarchical_zbuffer();
    void hierarchical_octree_zbuffer();
};
