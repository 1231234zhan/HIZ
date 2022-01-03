#pragma once

#include <chrono>
#include <string>

#include "global.h"

class Camera {
public:
    mat4 lookat;
    mat4 perspective;

    Camera();
    Camera(const mat4& lookat, const mat4& perspective);
};

class Color {
public:
    union {
        struct
        {
            uchar c[3];
        };
        struct
        {
            uchar r, g, b;
        };
    };

    Color();
    Color(uchar, uchar, uchar);
};

class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> _now, _end;

public:
    Timer();
    void start();
    void end();
    void output(const std::string&);
    void end_and_output(const std::string&);
};