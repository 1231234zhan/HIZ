
#include <vector>

#include "global.h"
#include "scene.h"
#include "triangle.h"

Scene::Scene(int width, int height)
    : buffer(width, height)
{
}

Scene::Scene(int width, int height, const tinyobj::ObjReader& reader)
    : buffer(width, height)
{
    auto& shapes = reader.GetShapes();
    auto& attrib = reader.GetAttrib();

    size_t num_faces = 0;

    for (size_t s = 0; s < shapes.size(); s++) {
        num_faces += shapes[s].mesh.num_face_vertices.size();
    }

    triangles.resize(num_faces);
    size_t all_index_offset = 0;

    // Code from https://github.com/tinyobjloader/tinyobjloader
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t point_index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = shapes[s].mesh.num_face_vertices[f];
            if (fv != 3) {
                ERRORM("Polygon not triangulated at solid %zu face %zu\n", s, f);
            }
            vec3 p[3];
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[point_index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                p[v] = vec3(vx, vy, vz);
            }
            triangles[all_index_offset] = Triangle(p[0], p[1], p[2]);
            all_index_offset++;
            point_index_offset += fv;
        }
    }
    INFO("Scene face num %zu\n", all_index_offset);
}

void Scene::transform_to_screen(const Camera& camera)
{
    vec4 viewport = vec4(0.0f, 0.0f, 1.0f * buffer.width, 1.0f * buffer.height);
    for (auto& tri : triangles) {
        tri.transform_to_screen(camera, viewport);
    }
}

void Scene::culling_faces(const std::vector<int>& tri_id)
{
    for (const int i : tri_id) {
        auto& tri = triangles[i];
        if (buffer.is_visible(buffer.root, tri)) {
            DEBUGM("Triangle %d VISIBLE\n", i);
            buffer.rasterization(tri, tri.color(), true);
        } else {
            DEBUGM("Triangle %d INVISIBLE\n", i);
        }
    }
}

void Scene::octree_occlusion(const std::vector<int>& tri_id)
{
    BBox3 box_all;
    if (tri_id.size() < 5) {
        culling_faces(tri_id);
        return;
    }
    for (const int i_tri : tri_id) {
        box_all.update(triangles[i_tri]);
    }

    // Divide big box to 8 small boxes
    /*
       Z x ---> X   
         | 0(4) | 1(5)
         | ---- | ----
         | 2(6) | 3(7)
         Y
    */

    std::vector<BBox3> boxs(8, box_all);
    flt x_mid = (box_all.x_max + box_all.x_min) / 2;
    flt y_mid = (box_all.y_max + box_all.y_min) / 2;
    flt z_mid = (box_all.z_max + box_all.z_min) / 2;

    boxs[0].x_max = boxs[2].x_max = boxs[4].x_max = boxs[6].x_max = x_mid;
    boxs[1].x_min = boxs[3].x_min = boxs[5].x_min = boxs[7].x_min = x_mid;

    boxs[0].y_max = boxs[1].y_max = boxs[4].y_max = boxs[5].y_max = y_mid;
    boxs[2].y_min = boxs[3].y_min = boxs[6].y_min = boxs[7].y_min = y_mid;

    boxs[0].z_max = boxs[1].z_max = boxs[2].z_max = boxs[3].z_max = z_mid;
    boxs[4].z_min = boxs[5].z_min = boxs[6].z_min = boxs[7].z_min = z_mid;

    // Test which box triangles belong to
    std::vector<int> tris[8], tris_edge;
    for (const int i_tri : tri_id) {
        BBox3 box_t = BBox3(triangles[i_tri]);
        bool flag = false;

        for (int i_box = 0; i_box < 8; i_box++) {
            if (boxs[i_box].is_contain(box_t)) {
                tris[i_box].push_back(i_tri);
                flag = true;
                break;
            }
        }

        // Triangle isn't included in any sub-boxes
        if (!flag) {
            tris_edge.push_back(i_tri);
        }
    }

    // Culling faces at front
    for (int i = 0; i < 4; i++)
        octree_occlusion(tris[i]);

    // Culling faces at edge
    culling_faces(tris_edge);

    // Culling faces at back
    for (int i = 4; i < 8; i++)
        octree_occlusion(tris[i]);
}

void Scene::zbuffer()
{
    INFO("Begin Z-Buffer algorithm\n");
    for (auto& tri : triangles) {
        buffer.rasterization(tri, tri.color());
    }
    INFO("End Z-Buffer algorithm\n\n");
}

void Scene::hierarchical_zbuffer()
{
    INFO("Begin Hierarchical Z-Buffer algorithm\n");
    buffer.clear_pyramid();
    buffer.build_pyramid();
    for (int i = 0; i < triangles.size(); i++) {
        auto& tri = triangles[i];
        if (buffer.is_visible(buffer.root, tri)) {
            DEBUGM("Triangle %d VISIBLE\n", i);
            buffer.rasterization(tri, tri.color(), true);
        } else {
            DEBUGM("Triangle %d INVISIBLE\n", i);
        }
    }
    INFO("End Hierarchical Z-Buffer algorithm\n");
}

void Scene::hierarchical_octree_zbuffer()
{
    INFO("Begin Hierarchical Z-Buffer with octree algorithm\n");
    buffer.clear_pyramid();
    buffer.build_pyramid();
    std::vector<int> tris(triangles.size());
    for (int i = 0; i < tris.size(); i++) {
        tris[i] = i;
    }
    octree_occlusion(tris);
    INFO("End Hierarchical Z-Buffer with octree algorithm\n");
}