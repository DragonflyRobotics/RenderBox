#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <regex>
#include <string>
#include <vector>
#include "geometry.cpp"

struct Model {
    std::string name;
    std::vector<Point3d> vertices;
    std::vector<Triangle3d> triangles;
    std::vector<std::vector<int>> faces; // Not used in this example, but can be useful for more complex models
    double bbox[2][3]; // Bounding box: [min, max][x, y, z]
    double model_center[3]; // Center of the model
    double scale; // Scale factor
    double offset[3]; // Offset to center the model
    
    public:
    void rotateX(double theta_rad) {
        double cosTheta = std::cos(theta_rad);
        double sinTheta = std::sin(theta_rad);
        double max[3] = {bbox[1][0], 0.0, 0.0};
        double min[3] = {bbox[0][0], 0.0, 0.0};
        for (Triangle3d &t: triangles) {
            for (Point3d* p: t.as_points()) {
                p->y -= offset[1];
                p->z -= offset[2];

                double newY = cosTheta * p->y - sinTheta * p->z;
                double newZ = sinTheta * p->y + cosTheta * p->z;
                p->y = newY + offset[1];
                p->z = newZ + offset[2];

                min[1] = std::min(min[1], p->y);
                min[2] = std::min(min[2], p->z);
                max[1] = std::max(max[1], p->y);
                max[2] = std::max(max[2], p->z);
            }
        }

        bbox[0][0] = min[0];
        bbox[0][1] = min[1];
        bbox[0][2] = min[2];
        bbox[1][0] = max[0];
        bbox[1][1] = max[1];
        bbox[1][2] = max[2];
    }
    void rotateY(double theta_rad) {
        double cosTheta = std::cos(theta_rad);
        double sinTheta = std::sin(theta_rad);
        double max[3] = {0.0, bbox[1][1], 0.0};
        double min[3] = {0.0, bbox[0][1], 0.0};
        for (Triangle3d &t: triangles) {
            for (Point3d* p: t.as_points()) {
                p->x -= offset[0];
                p->z -= offset[2];

                double newX = cosTheta * p->x - sinTheta * p->z;
                double newZ = sinTheta * p->x + cosTheta * p->z;
                p->x = newX + offset[0];
                p->z = newZ + offset[2];

                min[0] = std::min(min[0], p->x);
                min[2] = std::min(min[2], p->z);
                max[0] = std::max(max[0], p->x);
                max[2] = std::max(max[2], p->z);
            }
        }

        bbox[0][0] = min[0];
        bbox[0][1] = min[1];
        bbox[0][2] = min[2];
        bbox[1][0] = max[0];
        bbox[1][1] = max[1];
        bbox[1][2] = max[2];
    }
    void rotateZ(double theta_rad) {
        double cosTheta = std::cos(theta_rad);
        double sinTheta = std::sin(theta_rad);
        double max[3] = {0.0, 0.0, bbox[1][2]};
        double min[3] = {0.0, 0.0, bbox[0][2]};
        for (Triangle3d &t: triangles) {
            for (Point3d* p: t.as_points()) {
                p->x -= offset[0];
                p->y -= offset[1];

                double newX = cosTheta * p->x - sinTheta * p->y;
                double newY = sinTheta * p->x + cosTheta * p->y;
                p->x = newX + offset[0];
                p->y = newY + offset[1];

                min[0] = std::min(min[0], p->x);
                min[1] = std::min(min[1], p->y);
                max[0] = std::max(max[0], p->x);
                max[1] = std::max(max[1], p->y);
            }
        }

        bbox[0][0] = min[0];
        bbox[0][1] = min[1];
        bbox[0][2] = min[2];
        bbox[1][0] = max[0];
        bbox[1][1] = max[1];
        bbox[1][2] = max[2];
    }
};

std::vector<std::string> split(std::string &line, char delim) {
    std::string l;
    std::stringstream ss(line);
    std::vector<std::string> tokens;
    while (std::getline(ss, l, delim)) {
        tokens.push_back(l);
    }
    return tokens;
}

Model load_obj(std::string path, int center[3], int scale = 1) {
    std::ifstream obj_file;
    obj_file.open(path, std::ios::in);
    std::string name;
    std::vector<std::vector<double>> vert;
    std::vector<std::vector<int>> faces;
    if (obj_file.is_open()) {
        std::string line;
        while (std::getline(obj_file, line)) {
            std::vector<std::string> tokens = split(line, ' ');
            if (tokens[0] == "v") {
                vert.push_back({
                    std::stod(tokens[1]),
                    std::stod(tokens[2]),
                    std::stod(tokens[3]),
                });
            }
            if (tokens[0] == "o") {
                name = tokens[1];
            }
            if (tokens[0] == "f") {
                // Face data, not used in this example
                auto part1 = split(tokens[1], '/');
                auto part2 = split(tokens[2], '/');
                auto part3 = split(tokens[3], '/');
                int v1 = std::stoi(part1[0]) - 1; // OBJ indices are 1-based
                int v2 = std::stoi(part2[0]) - 1;
                int v3 = std::stoi(part3[0]) - 1;
                faces.push_back({v1, v2, v3});
            }
        }
    }

    std::vector<Point3d> vertices;
    std::vector<double> vmin = vert[0];
    std::vector<double> vmax = vert[0];
    for (std::vector<double> v: vert) {
        v[0] *= scale;
        v[1] *= scale;
        v[2] *= scale;
        vmin[0] = std::min(vmin[0], v[0]);
        vmin[1] = std::min(vmin[1], v[1]);
        vmin[2] = std::min(vmin[2], v[2]);
        vmax[0] = std::max(vmax[0], v[0]);
        vmax[1] = std::max(vmax[1], v[1]);
        vmax[2] = std::max(vmax[2], v[2]);

        vertices.push_back(Point3d {v});
    }
    double center_x = (vmax[0] + vmin[0]) / 2;
    double center_y = (vmax[1] + vmin[1]) / 2;
    double center_z = (vmax[2] + vmin[2]) / 2;
    double offset_x = center[0] - center_x;
    double offset_y = center[1] - center_y;
    double offset_z = center[2] - center_z;
    for (Point3d &v : vertices) {
        v.x += offset_x;
        v.y += offset_y;
        v.z += offset_z;
    }
    return Model {
        name,
        vertices,
        {},
        faces,
        {{vmin[0], vmin[1], vmin[2]}, {vmax[0], vmax[1], vmax[2]}},
        {center_x, center_y, center_z},
        (double)scale,
        {offset_x, offset_y, offset_z},
    };
}

void mesh_model(Model &model) {
    std::vector<Triangle3d> meshed;
    for (auto face: model.faces) {
        meshed.push_back(Triangle3d {
            Point3d {model.vertices[face[0]].x, model.vertices[face[0]].y, model.vertices[face[0]].z},
            Point3d {model.vertices[face[1]].x, model.vertices[face[1]].y, model.vertices[face[1]].z},
            Point3d {model.vertices[face[2]].x, model.vertices[face[2]].y, model.vertices[face[2]].z},
        });
    }
    model.triangles = meshed;
    model.vertices = {};
}

void mesh2screen(Model &model) {
    std::vector<Triangle3d> triangles;
    for (Triangle3d t: model.triangles) {
        std::vector<std::vector<double>> points = {
            {t.a.x, t.a.y, t.a.z},
            {t.b.x, t.b.y, t.b.z},
            {t.c.x, t.c.y, t.c.z},
        };
        float cx = (t.a.x + t.b.x + t.c.x) / 3.0f;
        float cy = (t.a.y + t.b.y + t.c.y) / 3.0f;

        std::sort(points.begin(), points.end(), [cx, cy](const std::vector<double> &a, const std::vector<double> &b) {
            float angleA = atan2(a[1] - cy, a[0] - cx);
            float angleB = atan2(b[1] - cy, b[0] - cx);
            return angleA > angleB;  // COUNTER-CLOCKWISE (reverse for CW)
        });

        triangles.push_back(Triangle3d {
            Point3d {points[0]},
            Point3d {points[1]},
            Point3d {points[2]}
        });
    }
    std::sort(triangles.begin(), triangles.end(), [](const Triangle3d& t1, const Triangle3d& t2) {
        float z1 = (t1.a.z + t1.b.z + t1.c.z) / 3.0f;
        float z2 = (t2.a.z + t2.b.z + t2.c.z) / 3.0f;
        return z1 > z2; // Draw farther triangles first
    });
    model.triangles = triangles;
}
