#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include "x11.cpp"
#include "colors.cpp"
#include "geometry.cpp"
#include "obj_parser.cpp"
#include <random>
#include <vector>
#include <unistd.h>
#include <chrono>

int getRandomNumber(int min = 0, int max = 100) {
    static std::random_device rd;                         // Seed
    static std::mt19937 gen(rd());                        // Random number generator
    std::uniform_int_distribution<> distrib(min, max);    // Uniform distribution

    return distrib(gen);
}

int main() 
{
    X11_Handle handle = initializeX11(); 
    int center[3] = {640/2, 480/2, 0}; // Center of the model
    int max_brightness = 240; // Maximum brightness for the red channel
    Model obj = load_obj("cube.obj", center, 120);
    mesh_model(obj);
    obj.rotateY(M_PI-0.1);
    obj.rotateZ(M_PI);
    mesh2screen(obj);
    while (true) {
        XSetForeground(handle.display, handle.graphics_context, RGB2Hex(255, 255, 255));
        XFillRectangle(handle.display, handle.backbuffer, handle.graphics_context, 0, 0, 640, 480);
        // obj.rotateX(M_PI/10);
        obj.rotateY(M_PI/100);
        // obj.rotateZ(M_PI/10);
        mesh2screen(obj);
        // auto t2 = mesh2screen(t3);
        std::vector<std::vector<float>> zbuffer(480, std::vector<float>(640, std::numeric_limits<float>::infinity()));
        for (const Triangle3d &t: obj.triangles) {
            Triangle2d small_t = Triangle2d {
                Point2d {(int)t.a.x, (int)t.a.y},
                Point2d {(int)t.b.x, (int)t.b.y},
                Point2d {(int)t.c.x, (int)t.c.y}
            };
            auto minx = std::min({t.a.x, t.b.x, t.c.x});
            auto miny = std::min({t.a.y, t.b.y, t.c.y});
            auto maxx = std::max({t.a.x, t.b.x, t.c.x});
            auto maxy = std::max({t.a.y, t.b.y, t.c.y});
            for (int w = minx; w <= maxx; w++) { 
                for (int h=miny; h <= maxy; h++) {
                    if (small_t.is_point_inside(Point2d{w, h})) {
                        double d = t.getDepth(Point2d{w, h});
                        if (d < zbuffer[h][w]) {
                            zbuffer[h][w] = d;
                        } else {
                            continue; // Skip if the depth is not less than the current zbuffer value
                        }
                        int red_intensity = max_brightness - (int)(((d - obj.bbox[0][2]) / (obj.bbox[1][2]-obj.bbox[0][2])) * max_brightness);
                        drawpixel(handle, w, h, RGB2Hex(red_intensity, 0, 0)); //green
                    }
                }
            }
        }
        XCopyArea(handle.display, handle.backbuffer, handle.window,
                  handle.graphics_context, 0, 0, 640, 480, 0, 0);
        XFlush(handle.display);
    }
    destroyX11(handle);
    return 0;
}
