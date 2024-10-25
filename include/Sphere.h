#ifndef SPHERE_H
#define SPHERE_H

#include "Vec3.h"
#include "Ray.h"

class Sphere {
public:
    Vec3 center;
    float radius;

    Sphere(Vec3 center_, float radius_);

    bool intersect(const Ray& ray, float& t) const;
};

#endif // SPHERE_H