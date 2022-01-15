#pragma once

#include <cstdio>
#include <glm/glm.hpp>

#ifdef DEBUG
#define IF_DEBUG 1
#else
#define IF_DEBUG 0
#endif

#define ERRORM(fmt, ...)                               \
    do {                                               \
        fprintf(stderr, "Error: " fmt, ##__VA_ARGS__); \
        exit(-1);                                      \
    } while (false)

#define INFO(fmt, ...)                                \
    do {                                              \
        fprintf(stdout, "Info: " fmt, ##__VA_ARGS__); \
    } while (false)

#define DEBUGM(fmt, ...)                                   \
    do {                                                   \
        if (IF_DEBUG)                                      \
            fprintf(stdout, "Debug: " fmt, ##__VA_ARGS__); \
    } while (false)

typedef unsigned char uchar;
typedef float flt;

typedef glm::vec4 vec4;
typedef glm::vec3 vec3;
typedef glm::vec2 vec2;
typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::ivec4 ivec4;
typedef glm::ivec3 ivec3;
typedef glm::ivec2 ivec2;

// Constant picure width and height
const int kWidth = 1000;
const int kHeight = 1000;

const flt kEps = 1e-12;
const flt kZero = 0.0;

template <typename T>
inline T clamp(T x, T a, T b)
{
    if (a > b)
        ERRORM("Clamp error\n");
    return (x < a) ? a : (b < x) ? b
                                 : x;
}

template <typename T>
inline bool inside(T x, T a, T b)
{
    if (a > b)
        ERRORM("Inside error\n");
    return (a <= x && x <= b) ? true : false;
}

template <typename T>
inline int sign(T x)
{
    return x > 0 ? 1 : (x < 0 ? -1 : 0);
}