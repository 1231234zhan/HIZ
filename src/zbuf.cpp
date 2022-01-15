
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "bbox.h"
#include "glm/gtc/epsilon.hpp"
#include "global.h"
#include "triangle.h"
#include "zbuf.h"

ZBufNode::ZBufNode()
    : isleaf(Leaf)
    , depth(INFINITY)
    , fa(-1)
{
    for (int i = 0; i < 4; i++)
        child[i] = -1;
}

ZBuf::ZBuf(int w, int h)
{
    width = w;
    height = h;
    n_array.resize(w * h);
    root = -1;
}

int ZBuf::array_id(int x, int y)
{
    return y * width + x;
}

flt ZBuf::depth(int x, int y)
{
    return n_array[array_id(x, y)].depth;
}

void ZBuf::set_pixel(int x, int y, flt dp, const Color& color, bool isupdate)
{
    int id = array_id(x, y);
    n_array[id].color = color;
    n_array[id].depth = dp;

    if (isupdate) {
        int node_id = id;
        while (true) {
            int node_fa = n_array[node_id].fa;
            if (node_fa == -1)
                break;
            flt fa_dp = n_array[node_fa].depth;

            if (dp < fa_dp) {
                n_array[node_fa].depth = dp;
            } else {
                break;
            }
            node_id = node_fa;
        }
    }
}

void ZBuf::save_file(const std::string& s)
{
    using namespace std;
    ofstream file(s);
    file << "P3\n"
         << width << " " << height << "\n255\n";
    for (int x_t = 0; x_t < width; x_t++) {
        for (int y_t = 0; y_t < height; y_t++) {
            auto& color = n_array[array_id(x_t, y_t)].color;
            file << static_cast<int>(color.r) << " " << static_cast<int>(color.g) << " " << static_cast<int>(color.b) << "  ";
        }
        file << "\n";
    }
}

int sign_distance(const vec2 p_test, const vec2& p1, const vec2& p2)
{
    flt A = p1.y - p2.y;
    flt B = p2.x - p1.x;
    flt C = p1.x * p2.y - p1.y * p2.x;

    return sign(A * p_test.x + B * p_test.y + C);
}

bool inside_triangle(const vec2& p_test, const Triangle& tri)
{
    int t = 0;
    for (int i = 0; i < 3; i++) {
        vec2 p1 = vec2(tri.p[i]);
        vec2 p2 = vec2(tri.p[(i + 1) % 3]);
        if (i == 0) {
            t = sign_distance(p_test, p1, p2);
        } else {
            if (t * sign_distance(p_test, p1, p2) < 0)
                return false;
        }
    }
    return true;
}

flt calc_depth(const vec2& p_test, const Triangle& tri)
{
    const vec3& p1 = tri.p1();
    const vec3& p2 = tri.p2();
    const vec3& p3 = tri.p3();

    flt A = (p2.y - p1.y) * (p3.z - p1.z) - (p3.y - p1.y) * (p2.z - p1.z);
    flt B = (p2.z - p1.z) * (p3.x - p1.x) - (p3.z - p1.z) * (p2.x - p1.x);
    flt C = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
    flt D = -(A * p1.x + B * p1.y + C * p1.z);

    if (glm::epsilonEqual(C, kZero, kEps)) {
        // maybe invisible
        return INFINITY;
        // ERRORM("Points in a triangle maybe overlap\n");
    }

    flt depth = -(A * p_test.x + B * p_test.y + D) / C;
    return depth;
}

void ZBuf::rasterization(const Triangle& tri, const Color& color, bool isupdate)
{
    // Triangle bounding box
    const vec3& p1 = tri.p1();
    flt x_fmin = p1.x, x_fmax = p1.x;
    flt y_fmin = p1.y, y_fmax = p1.y;

    for (int i = 1; i < 3; i++) {
        x_fmin = std::min(x_fmin, tri.p[i].x);
        x_fmax = std::max(x_fmax, tri.p[i].x);
        y_fmin = std::min(y_fmin, tri.p[i].y);
        y_fmax = std::max(y_fmax, tri.p[i].y);
    }

    DEBUGM("Triangle bbox info: %f  %f  %f  %f\n", x_fmin, x_fmax, y_fmin, y_fmax);
    int x_min = std::max(static_cast<int>(std::floor(x_fmin)), 0);
    int x_max = std::min(static_cast<int>(std::ceil(x_fmax)), kWidth - 1);
    int y_min = std::max(static_cast<int>(std::floor(y_fmin)), 0);
    int y_max = std::min(static_cast<int>(std::ceil(y_fmax)), kHeight - 1);

    // Test each pixel in BBox
    for (int x_t = x_min; x_t <= x_max; x_t++) {
        for (int y_t = y_min; y_t <= y_max; y_t++) {
            vec2 p_t = vec2(x_t, y_t);
            if (inside_triangle(p_t, tri)) {
                flt depth_pt = calc_depth(p_t, tri);
                if (depth_pt < depth(x_t, y_t)) {
                    set_pixel(x_t, y_t, depth_pt, color, isupdate);
                }
            }
        }
    }
}

void ZBuf::build_pyramid()
{
    INFO("Begin build pyramid\n");
    int now_width = width;
    int now_height = height;
    int last_width;
    int last_height;

    // For the leaves layer of the buffer, initialize bbox value
    for (int y_t = 0; y_t < now_height; y_t++) {
        for (int x_t = 0; x_t < now_width; x_t++) {
            auto& box = n_array[array_id(x_t, y_t)].box;
            box.x_min = x_t - 0.5f;
            box.x_max = x_t + 0.5f;
            box.y_min = y_t - 0.5f;
            box.y_max = y_t + 0.5f;
        }
    }

    // Generate H x W matrix map
    // map[y][x] -> node_id
    std::vector<std::vector<int>> last_map, now_map;

    last_map.resize(now_height);
    for (int y_t = 0; y_t < now_height; y_t++) {
        last_map[y_t].resize(now_width);
        for (int x_t = 0; x_t < now_width; x_t++) {
            last_map[y_t][x_t] = array_id(x_t, y_t);
        }
    }
    DEBUGM("Pyramid matrix size at bottom layer: %d x %d  (H x W)\n", now_height, now_width);

     while (!(now_height == 1 && now_width == 1)) {
        last_width = now_width;
        last_height = now_height;
        now_height = now_height % 2 ? now_height / 2 + 1 : now_height / 2;
        now_width = now_width % 2 ? now_width / 2 + 1 : now_width / 2;

        // now_map resizes to new matrix size
        now_map.resize(now_height);
        for (int y_t = 0; y_t < now_height; y_t++) {
            now_map[y_t].resize(now_width);
        }

        DEBUGM("Pyramid matrix size: %d x %d  (H x W)\n", now_height, now_width);

        for (int y_t = 0; y_t < now_height; y_t++) {
            for (int x_t = 0; x_t < now_width; x_t++) {
                ZBufNode node;

                // Node type
                node.isleaf = ZBufNode::Node;

                // Assign node_id to child[]
                // -1 if a child doesn't exist
                for (int y_offset = 0; y_offset < 2; y_offset++) {
                    for (int x_offset = 0; x_offset < 2; x_offset++) {
                        int x_last = 2 * x_t + x_offset;
                        int y_last = 2 * y_t + y_offset;
                        if (x_last >= last_width || y_last >= last_height) {
                            node.child[2 * y_offset + x_offset] = -1;
                        } else {
                            node.child[2 * y_offset + x_offset] = last_map[y_last][x_last];
                        }
                    }
                }

                if (node.child_tl == -1) {
                    ERRORM("Top left child shouldn't be empty\n");
                }

                // Depth
                node.depth = n_array[node.child[0]].depth;
                for (int i = 1; i < 4; i++) {
                    if (node.child[i] != -1)
                        node.depth = std::max(node.depth, n_array[node.child[i]].depth);
                }

                // BBox
                node.box.x_min = n_array[node.child[0]].box.x_min;
                node.box.x_max = n_array[node.child[0]].box.x_max;
                node.box.y_min = n_array[node.child[0]].box.y_min;
                node.box.y_max = n_array[node.child[0]].box.y_max;
                for (int i = 1; i < 4; i++) {
                    if (node.child[i] != -1) {
                        node.box.x_min = std::min(node.box.x_min, n_array[node.child[i]].box.x_min);
                        node.box.x_max = std::max(node.box.x_max, n_array[node.child[i]].box.x_max);
                        node.box.y_min = std::min(node.box.y_min, n_array[node.child[i]].box.y_min);
                        node.box.y_max = std::max(node.box.y_max, n_array[node.child[i]].box.y_max);
                    }
                }

                // Father
                node.fa = -1;

                int node_id = n_array.size();
                n_array.push_back(node);

                // Children's Father
                for (int i = 0; i < 4; i++) {
                    if (node.child[i] != -1) {
                        n_array[node.child[i]].fa = node_id;
                    }
                }

                // Assign present node_id to new_map
                now_map[y_t][x_t] = node_id;
            }
        }

        // Switch now_map to last_map
        // For the next iteration usage
        last_map.swap(now_map);

        if (last_map.size() != now_height || last_map[0].size() != now_width) {
            ERRORM("Swapped map size isn't correct\n");
        }
    }

    root = last_map[0][0];

    // Check potential errors
    if (root != n_array.size() - 1) {
        ERRORM("Root is not the last element of node_array\n");
    }
    for (int i = 0; i < n_array.size() - 1; i++) {
        if (n_array[i].fa < 0 || n_array[i].fa > n_array.size() - 1)
            ERRORM("Father of node %d is invalid, the value is %d. array size %d\n",
                i, n_array[i].fa, n_array.size());

        if (n_array[i].isleaf == ZBufNode::Leaf) {
            for (int c = 0; c < 4; c++) {
                if (n_array[i].child[c] != -1)
                    ERRORM("Nodes %d are leave node but children's id not equal to -1\n", i);
            }
        }
    }

    INFO("End build pyramid\n");
}


void ZBuf::clear_pyramid(){
    DEBUGM("Clear pyramid\n");
    root = -1;
    // Delete all the node except leaves
    n_array.resize(width * height);
}


bool ZBuf::is_visible(int now_node_id, const BBox2& box, flt depth)
{
    auto& now_node = n_array[now_node_id];

    for (int i = 0; i < 4; i++) {
        if (now_node.child[i] == -1)
            continue;
        int child_id = now_node.child[i];
        auto& child_box = n_array[child_id].box;

        if (child_box.is_contain(box)) {
            return is_visible(child_id, box, depth);
        }
    }

    // Box invisible
    if (now_node.depth < depth)
        return false;
    // Visible
    return true;
}

bool ZBuf::is_visible(int now_node_id, const Triangle& tri)
{
    BBox2 box = BBox2(tri);

    flt depth = INFINITY;
    for (int i = 0; i < 3; i++)
        depth = std::min(depth, tri.p[i].z);

    return is_visible(now_node_id, box, depth);
}
