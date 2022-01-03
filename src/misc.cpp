
#include <chrono>
#include <iostream>
#include <string>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "global.h"
#include "misc.h"

Camera::Camera()
    : lookat(glm::lookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0)))
    , perspective(glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f))
{
}

Camera::Camera(const mat4& lookat, const mat4& perspective)
    : lookat(lookat)
    , perspective(perspective)
{
}

Color::Color()
    : r(0)
    , g(0)
    , b(0)
{
}

Color::Color(uchar red, uchar green, uchar blue)
    : r(red)
    , g(green)
    , b(blue)
{
}

Timer::Timer()
{
}

void Timer::start()
{
    _now = std::chrono::steady_clock::now();
}

void Timer::end()
{
    _end = std::chrono::steady_clock::now();
}

void Timer::output(const std::string& s)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _now);
    std::cout << s << " " << ms.count() << "ms" << std::endl;
}

void Timer::end_and_output(const std::string& s)
{
    _end = std::chrono::steady_clock::now();
    this->output(s);
}