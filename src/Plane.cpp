#include "../include/Plane.h"
#include "../include/Vec3.h"
#include "../include/Ray.h"
#include <cmath>

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