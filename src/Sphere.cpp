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

Vec3 Sphere::calculateLighting(const Vec3& P_I, Ray& raio, const PontoLuminoso& luz, const PontoLuminoso& luzAmb, const Vec3& Kd, const Vec3& Ka, const Vec3& Ke, float m) const {

    // Vetor normal à superfície da esfera no ponto de interseção
    const Vec3 N = (P_I - center).normalize();

    // Vetor que vai do ponto de interseção ao olho do observador
    const Vec3 v = -raio.direction.normalize();

    // Vetor que vai do ponto de interseção até a luz
    const Vec3 l = (luz.position - P_I).normalize();

    // Vetor de reflexão da luz
    const Vec3 r = (N * 2 *N.dot(l) - l).normalize();

    const float NdotL = std::max(0.0f, N.dot(l));
    const float RdotV = std::max(0.0f, r.dot(v));

    // Calcular vetores de iluminação
    // Intensidade de reflexão difusa
    const Vec3 I_d = Kd * luz.intensity * NdotL;

    // Intensidade de reflexão especular
    const Vec3 I_e = Ke * luz.intensity * std::pow(RdotV, m);

    // Intensidade de reflexão ambiente
    const Vec3 I_a = Ka * luzAmb.intensity;

    return I_d + I_e + I_a;

}