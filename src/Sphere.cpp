#include "../include/Sphere.h"
#include <cmath>

Sphere::Sphere(Vec3 center_, float radius_) : center(center_), radius(radius_) {}

bool Sphere::intersect(const Ray &ray, float &t) const
{
    Vec3 v = ray.origin - center; // Vetor indo do centro da esfera até a origem do raio
    // Coeficientes da equação de segundo grau
    // onde a variável é t, o multiplicador do raio direção
    // se houver uma rais de t, o raio tangencia a superfície da esfera
    // se houver duas raízes, o raio atravessa a esfera
    // se não houver raízes, o raio não toca a esfera
    float a = ray.direction.dot(ray.direction);
    float b = 2 * ray.direction.dot(v);
    float c = v.dot(v) - radius * radius;
    float delta = b * b - 4 * a * c;
    if (delta > 0) {
        // Duas raízes 
        float t1 = (-b + std::sqrt(delta)) / (2.0f * a);
        float t2 = (-b - std::sqrt(delta)) / (2.0f * a);
        // A menor raiz positiva é a que interessa
        float t_cand = std::fmin(t1, t2);
        if (t_cand > 0 && t_cand < t) {
            t = t_cand;
            return true;
        }
    } else if (delta == 0) {
        // Uma raiz
        float t1 = -b / (2.0f * a);
        if (t1 > 0 && t1 < t) {
            t = t1;
            return true;
        }
    }
    // Nenhuma raiz ou t_cand não é menor que t
    return false;
}