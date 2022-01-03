#pragma once

#include <string>
#include <vector>

#include "bbox.h"
#include "global.h"
#include "misc.h"

class ZBufNode {
public:
    enum nodetype {
        Leaf,
        Node,
    };
    nodetype isleaf;
    flt depth;
    int fa;

    BBox2 box;

    // If not a leaf
    union {
        struct {
            int child[4];
        };
        struct {
            int child_tl, child_tr, child_bl, child_br;
        };
    };

    // If a leaf
    Color color;
    ZBufNode();
};

class ZBuf {
public:
    int width;
    int height;

    // Hierarchical z-buffer data
    // 0 --- (width * height - 1) : tree leaves node
    std::vector<ZBufNode> n_array;
    int root;

    ZBuf(int, int);

    int array_id(int x, int y);

    flt depth(int x, int y);
    // Set pixel with or without z-buffer updated
    void set_pixel(int x, int y, flt, const Color&, bool = false);
    void save_file(const std::string&);
    // Rasterize with or without z-buffer updated
    void rasterization(const Triangle&, const Color&, bool = false);

    // Build hierarchical z-buffer from bottom to root
    void build_pyramid();
    // Clear all the hierarchical z-buffer nodes
    void clear_pyramid();
    // Check if a triangle is visible in hierarchical z-buffer
    bool is_visible(int, const BBox2&, flt);
    bool is_visible(int, const Triangle&);
};