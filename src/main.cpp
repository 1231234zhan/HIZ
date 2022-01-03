
#include <cstdio>
#include <string>

#include <glm/ext.hpp>

#include "global.h"
#include "misc.h"
#include "scene.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

void read_objfile(const std::string inputfile, tinyobj::ObjReader& objreader)
{
    tinyobj::ObjReaderConfig objreader_config;

    if (!objreader.ParseFromFile(inputfile, objreader_config)) {
        if (!objreader.Error().empty()) {
            ERRORM("TinyObjReader %s", objreader.Error());
        }
        ERRORM("TinyObjReader error");
    }

    if (!objreader.Warning().empty()) {
        INFO("TinyObjReader %s", objreader.Warning());
    }
}

int main(int argc, char** argv)
{
    std::string inputfile = "input.obj";
    flt distance = 5.0f;
    if (argc > 1) {
        inputfile = std::string(argv[1]);
    }

    if (argc > 2) {
        distance = std::stof(argv[2]);
    }

    tinyobj::ObjReader objreader;
    Timer timer;

    timer.start();
    read_objfile(inputfile, objreader);
    timer.end_and_output("Read OBJ elasped time: ");

    Scene scene(kWidth, kHeight, objreader);

    mat4 Look = glm::lookAt(vec3(-distance, distance, distance), vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 Pers = glm::perspective(glm::radians(60.0f), 1.0f * kWidth / kHeight, 0.1f, 100.0f);
    Camera camera(Look, Pers);

    // We'll do transformation ahead of each occlusion method
    scene.transform_to_screen(camera);

    timer.start();
    scene.zbuffer();
    timer.end_and_output("Z-Buffer algorithm elapsed time: ");
    scene.buffer.save_file("zbuffer.ppm");

    timer.start();
    scene.hierarchical_zbuffer();
    timer.end_and_output("Hierarchical Z-Buffer algorithm elapsed  time: ");
    scene.buffer.save_file("hie-zbuffer.ppm");

    timer.start();
    scene.hierarchical_octree_zbuffer();
    timer.end_and_output("Hierarchical Z-Buffer with octree algorithm elapsed time: ");
    scene.buffer.save_file("hie-zbuffer-octree.ppm");

    return 0;
}