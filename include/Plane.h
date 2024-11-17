#ifndef PLANE_H
#define PLANE_H

#include <Sphere.h>

#include "Vec3.h"
#include "Ray.h"
#include "PontoLuminoso.h"

class Plane {
public:
    Vec3 point;
    Vec3 normal;

    Plane(Vec3 point_, Vec3 normal_);

    bool intersect(const Ray& ray, float& t) const;

    Vec3 calculateLighting(const Sphere &esfera, const Vec3 &P_I, Ray &raio, const PontoLuminoso &luz, const PontoLuminoso &luzAmb, const Vec3 &Kd, const
                           Vec3 &Ka, const Vec3 &Ke, float m) const;
};

#endif // PLANE_H