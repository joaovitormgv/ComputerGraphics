#ifndef CONE_H
#define CONE_H

#include "Vec3.h"
#include "Ray.h"
#include "PontoLuminoso.h"

class Cone {
public:
    float altura;
    float raioBase;
    Vec3 direction; // Vetor de direção do cone
    Vec3 CentroBase; // Centro da base do cone

    Cone(float altura_, float raioBase_, Vec3 direction_, Vec3 CentroBase_);

    bool intersect(const Ray& ray, float& t) const;

    Vec3 calculateLighting(const Vec3& P_I, Ray& raio, const PontoLuminoso& luz, const PontoLuminoso& luzAmb, const Vec3& Kd, const Vec3& Ka, const Vec3& Ke, float m) const;
};

#endif //CONE_H