#include "../include/Cilindro.h"
#include <iostream>

Cilindro::Cilindro(float altura_, float raioBase_, Vec3 direction_, Vec3 CentroBase_) : altura(altura_), raioBase(raioBase_), direction(direction_), CentroBase(CentroBase_) {}

bool Cilindro::intersect(const Ray &ray, float &t) const {
    Vec3 w = ray.origin - CentroBase;

    // float a = 1 - std::pow(ray.direction.dot(direction), 2);
    float a = ray.direction.dot(ray.direction) - std::pow(ray.direction.dot(direction), 2);
    if (std::fabs(a) < 1e-6) return false;

    float b = 2 * (w.dot(ray.direction) - w.dot(direction) * ray.direction.dot(direction));
    float c = w.dot(w) - std::pow(raioBase, 2) - std::pow(w.dot(direction), 2);

    float delta = b * b - 4 * a * c;

    if (delta < 0) return false;  // Sem interseção

    float sqrtDelta = std::sqrt(delta);
    float t1 = (-b - sqrtDelta) / (2.0f * a);
    float t2 = (-b + sqrtDelta) / (2.0f * a);

    float t_cand = getSmallestPositiveRoot(t1, t2);

    if (t_cand > 0 && t_cand < t) {
        Vec3 P_I = ray.origin + ray.direction * t_cand;
        if (isInsideHeight(P_I)) {
            t = t_cand;
            return true;
        }
    }

    return false;  // Fora dos limites do cilindro
}

Vec3 Cilindro::calculateLighting(const Vec3 &P_I, Ray &raio, const PontoLuminoso &luz, const PontoLuminoso &luzAmb, const Vec3 &Kd, const Vec3 &Ka, const Vec3 &Ke, float m) const {
    const float h = (P_I - CentroBase).dot(direction); // Altura do ponto de interseção

    const Vec3 B = CentroBase + direction * h; // Projeção do ponto de interseção no eixo do cilindro

    // Vetor normal à superfície do cilindro no ponto de interseção
    const Vec3 N = (P_I - B).normalize();

    const Vec3 v = -raio.direction.normalize(); // Vetor que vai do ponto de interseção ao olho do observador

    const Vec3 l = (luz.position - P_I).normalize(); // Vetor que vai do ponto de interseção até a luz

    const Vec3 r = (N * 2 * N.dot(l) - l).normalize(); // Vetor de reflexão da luz

    const float NdotL = std::max(0.0f, N.dot(l)); // Produto escalar entre o vetor normal e o vetor que vai do ponto de interseção até a luz

    const float RdotV = std::max(0.0f, r.dot(v)); // Produto escalar entre o vetor de reflexão da luz e o vetor que vai do ponto de interseção ao olho do observador
    // Iluminação ambiente
    Vec3 Ia = Ka * luzAmb.intensity;

    // Iluminação difusa
    Vec3 Id = Kd * luz.intensity * NdotL;

    // Iluminação especular
    Vec3 Ie = Ke * luz.intensity * std::pow(RdotV, m);

    return Ia + Id + Ie;
}




