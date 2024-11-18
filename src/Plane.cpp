#include "../include/Plane.h"
#include "../include/Vec3.h"
#include "../include/Ray.h"
#include <cmath>
#include <Sphere.h>
#include <iostream>

Plane::Plane(Vec3 point_, Vec3 normal_) : point(point_), normal(normal_.normalize()) {}

bool Plane::intersect(const Ray &ray, float &t) const{
    float denom = ray.direction.dot(normal);
    if (std::abs(denom) > 1e-6)
    {
        Vec3 p0l0 = point - ray.origin; // Vetor do ponto da origem do raio até o ponto do plano
        t = p0l0.dot(normal) / denom;   // Distância do ponto de origem do raio até o ponto de interseção
        return t >= 0;
    }
    return false;
}

Vec3 Plane::calculateLighting(const Sphere &esfera, const Vec3 &P_I, Ray &raio, const PontoLuminoso &luz, const PontoLuminoso &luzAmb, const Vec3 &Kd, const Vec3 &Ka, const Vec3 &Ke, float m) const {

    // Vetor normal à superfície do plano no ponto de interseção
    const Vec3 N = normal;

    // Vetor que vai do ponto de interseção ao olho do observador
    const Vec3 v = -raio.direction.normalize();

    // Vetor que vai do ponto de interseção até a luz
    const Vec3 l = (luz.position - P_I).normalize();

    const Ray raioSombra(P_I + N * 1e-4f, luz.position);
    float tEsferaSombra = std::numeric_limits<float>::max();
    bool hitEsferaSombra = esfera.intersect(raioSombra, tEsferaSombra);

    if (hitEsferaSombra) {
        // Se houver sombra, apenas luz Ambiente influencia

        const Vec3 I_a = Ka * luzAmb.intensity;

        return I_a;
    }

    // Se não houver sombra ...
    const Vec3 r = (N * 2 * N.dot(l) - l).normalize();

    const float NdotL = std::max(0.0f, N.dot(l));

    const float RdotV = std::max(0.0f, r.dot(v));

    const Vec3 I_d = Kd * luz.intensity * NdotL;

    const Vec3 I_e = Ke * luz.intensity * std::pow(RdotV, m);

    const Vec3 I_a = Ka * luzAmb.intensity;

    return I_d + I_e + I_a;

}
