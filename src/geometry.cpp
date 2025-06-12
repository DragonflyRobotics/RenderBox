#ifndef GEOMETRY
#define GEOMETRY


#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
#include <array>

struct Point2d {
    int x;
    int y;

    public:
    Point2d (int x, int y) {
        this->x = x;
        this->y = y;
    }
    Point2d (std::vector<int> v) {
        assert(v.size() == 2);
        x = v[0];
        y = v[1];
    }
    Point2d () {
        x = 0;
        y = 0;
    }
};

struct Point3d {
    double x;
    double y;
    double z;

    public:
    Point3d (double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Point3d (std::vector<double> v) {
        assert(v.size() == 3);
        x = v[0];
        y = v[1];
        z = v[2];
    }
    Point3d () {
        x = 0;
        y = 0;
        z = 0;
    }

};

struct Vector {
    Point2d a;
    Point2d b;
    int x;
    int y;

    public:
    Vector (Point2d from, Point2d to) {
        a = from;
        b = to;
        x = to.x - from.x;
        y = to.y - from.y;
    }

    double magnitude() {
        return std::sqrt((double)(std::pow(x, 2) + std::pow(y, 2)));
    }

    double dot(Vector b) {
        return x*b.x + y*b.y;
    }

    double cross(Vector b) {
        return x*b.y - y*b.x;
    }
};

struct Triangle2d {
    Point2d a;
    Point2d b;
    Point2d c;

    public:
    Triangle2d (Point2d p1, Point2d p2, Point2d p3) {
        a = p1;
        b = p2;
        c = p3;
    }

    bool is_point_inside(Point2d test_point) {
        Vector v1 = Vector(a, b);
        Vector v2 = Vector(b, c);
        Vector v3 = Vector(c, a);

        double cross1 = v1.cross(Vector(a,test_point));
        double cross2 = v2.cross(Vector(b,test_point));
        double cross3 = v3.cross(Vector(c,test_point));
        // printf("Cross1: %f Cross2: %f Cross3: %f\n", cross1, cross2, cross3);
        return (cross1 <= 0) && (cross2 <= 0) && (cross3 <= 0);    
    }
};

struct Triangle3d {
    Point3d a;
    Point3d b;
    Point3d c;

    public:
    Triangle3d (Point3d p1, Point3d p2, Point3d p3) {
        a = p1;
        b = p2;
        c = p3;
    }

    void rotateY(double angle) {
        double rad = angle * M_PI / 180.0;
        double cosA = std::cos(rad);
        double sinA = std::sin(rad);

        double newX = a.x * cosA - a.z * sinA;
        double newZ = a.x * sinA + a.z * cosA;
        a.x = newX;
        a.z = newZ;

        newX = b.x * cosA - b.z * sinA;
        newZ = b.x * sinA + b.z * cosA;
        b.x = newX;
        b.z = newZ;

        newX = c.x * cosA - c.z * sinA;
        newZ = c.x * sinA + c.z * cosA;
        c.x = newX;
        c.z = newZ;
    }

    double getDepth(Point2d test_point) const {
        Vector pa = Vector(test_point, Point2d{(int)a.x, (int)a.y});
        Vector pb = Vector(test_point, Point2d{(int)b.x, (int)b.y});
        Vector pc = Vector(test_point, Point2d{(int)c.x, (int)c.y});
        double A_area = 0.5 * std::abs(pb.cross(pc));
        double B_area = 0.5 * std::abs(pa.cross(pc));
        double C_area = 0.5 * std::abs(pa.cross(pb));
        double total_area = A_area + B_area + C_area;
        return ((A_area * a.z) + (B_area * b.z) + (C_area * c.z)) / total_area;
    }

    std::array<Point3d*, 3> as_points() {
        return {&a, &b, &c};
    }
};


#endif // !GEOMETRY
