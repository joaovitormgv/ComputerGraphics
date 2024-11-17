#ifndef CILINDRO_H
#define CILINDRO_H

#include "Vec3.h"
#include "Ray.h"
#include "PontoLuminoso.h"

class Cilindro {

    // Funções auxiliares
    bool isInsideHeight(const Vec3 &P_I) const {
        float h = (P_I - CentroBase).dot(direction);
        return h >= 0 && h <= altura;
    }

    static float getSmallestPositiveRoot(float t1, float t2) {
        if (t1 > 0 && t2 > 0) return std::fmin(t1, t2);
        if (t1 > 0) return t1;
        if (t2 > 0) return t2;
        return -1; // Indica que não há raiz positiva
    }

public:
    float altura;
    float raioBase;
    Vec3 direction; // Vetor de direção do cilindro
    Vec3 CentroBase; // Centro da base do cilindro



    Cilindro(float altura_, float raioBase_, Vec3 direction_, Vec3 CentroBase_);

    bool intersect(const Ray& ray, float& t) const;

    Vec3 calculateLighting(const Vec3& P_I, Ray& raio, const PontoLuminoso& luz, const PontoLuminoso& luzAmb, const Vec3& Kd, const Vec3& Ka, const Vec3& Ke, float m) const;
};

#endif //CILINDRO_H
