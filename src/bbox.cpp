
#include <algorithm>
#include <glm/glm.hpp>

#include "bbox.h"

template <int N>
BBox<N>::BBox()
{
    for (int i = 0; i < N; i++) {
        a[N][0] = INFINITY;
        a[N][1] = -INFINITY;
    }
}

template <int N>
BBox<N>::BBox(const Triangle& tri)
    : BBox()
{
    for (int i = 0; i < N; i++) {
        this->update(tri.p[i]);
    }
}

template <int N>
void BBox<N>::update(const Triangle& tri)
{
    for (int i = 0; i < N; i++) {
        this->update(tri.p[i]);
    }
}

template <int N>
void BBox<N>::update(const glm::vec<N, float, glm::defaultp>& vec)
{
    for (int i = 0; i < N; i++) {
        a[i][0] = std::min(a[i][0], vec[i]);
        a[i][1] = std::max(a[i][1], vec[i]);
    }
}

template <int N>
bool BBox<N>::is_contain(const BBox<N>& box)
{
    for (int i = 0; i < N; i++) {
        if (a[i][0] > box.a[i][0] + kEps)
            return false;
        if (a[i][1] < box.a[i][1] - kEps)
            return false;
    }
    return true;
}