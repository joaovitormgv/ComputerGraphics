#ifndef PLANE_H
#define PLANE_H

#include "Vec3.h"
#include "Ray.h"

class Plane {
public:
    Vec3 point;
    Vec3 normal;

    Plane(Vec3 point_, Vec3 normal_);

    bool intersect(const Ray& ray, float& t) const;
};

#endif // PLANE_H