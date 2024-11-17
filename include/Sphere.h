#ifndef SPHERE_H
#define SPHERE_H

#include "Vec3.h"
#include "Ray.h"
#include "PontoLuminoso.h"
#include "Sphere.h"

class Sphere {
public:
    Vec3 center;
    float radius;

    Sphere(Vec3 center_, float radius_);

    bool intersect(const Ray& ray, float& t) const;

    Vec3 calculateLighting(const Vec3& P_I, Ray& raio, const PontoLuminoso& luz, const PontoLuminoso& luzAmb, const Vec3& Kd, const Vec3& Ka, const Vec3& Ke, float m) const;
};

#endif // SPHERE_H