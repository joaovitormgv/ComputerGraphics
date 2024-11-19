#include "../include/Cone.h"
#include <iostream>
#include <cmath> // Para std::sqrt e std::pow
#include <limits> // Para std::numeric_limits

// Construtor do Cone
Cone::Cone(float altura_, float raioBase_, Vec3 direction_, Vec3 CentroBase_) 
    : altura(altura_), raioBase(raioBase_), direction(direction_.normalize()), CentroBase(CentroBase_) {}

// Função de Interseção com o Cone
bool Cone::intersect(const Ray &ray, float &t) const {
    // Definindo o vértice do cone
    Vec3 vertice = CentroBase + direction * altura;
    
    // Calculando o ângulo do cone a partir da altura e do raio da base
    float theta = std::atan(raioBase / altura); // theta = arctan(r/h)
    float cosTheta = std::cos(theta);
    float cosTheta2 = cosTheta * cosTheta;
    
    // Vetor do vértice para a origem do raio
    Vec3 m = ray.origin - vertice;
    
    // Coeficientes da equação quadrática a*t^2 + b*t + c = 0
    float a = std::pow(ray.direction.dot(direction), 2) - cosTheta2;
    float b = 2.0f * ((ray.direction.dot(direction) * m.dot(direction)) - (ray.direction.dot(m)) * cosTheta2);
    float c = std::pow(m.dot(direction), 2) - (m.dot(m)) * cosTheta2;
    
    // Resolver a equação quadrática
    float discriminant = b * b - 4.0f * a * c;
    
    // Verificar se há interseção
    if (discriminant < 0.0f) {
        return false; // Sem interseção
    }
    
    // Calcular as raízes
    float sqrtDiscriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t2 = (-b + sqrtDiscriminant) / (2.0f * a);
    
    // Encontrar a menor raiz positiva
    float t_cand = std::numeric_limits<float>::max();
    if (t1 > 1e-4f && t1 < t_cand) { // 1e-4f para evitar interseções muito próximas
        t_cand = t1;
    }
    if (t2 > 1e-4f && t2 < t_cand) {
        t_cand = t2;
    }
    
    if (t_cand == std::numeric_limits<float>::max()) {
        return false; // Nenhuma raiz válida
    }
    
    // Calcular o ponto de interseção
    Vec3 P_I = ray.origin + ray.direction * t_cand;
    
    // Calcular a projeção do ponto de interseção no eixo do cone para obter h
    float h = (P_I - CentroBase).dot(direction);
    
    // Verificar se o ponto de interseção está dentro da altura do cone
    if (h < 0.0f || h > altura) {
        return false; // Fora do intervalo do cone
    }
    
    // Atualizar t com o valor encontrado
    t = t_cand;
    return true;
}



// Função de Cálculo de Iluminação para o Cone
Vec3 Cone::calculateLighting(const Vec3& P_I, Ray& raio, const PontoLuminoso& luz, const PontoLuminoso& luzAmb, 
                             const Vec3& Kd, const Vec3& Ka, const Vec3& Ke, float m) const {
    
    // Definindo o vértice do cone
    Vec3 vertice = CentroBase + direction * altura; // Correto
    
    // Calculando a altura do ponto em relação à base do cone
    float h = (P_I - CentroBase).dot(direction); // Corrigido
    
    // Projeção do ponto de interseção no eixo do cone
    Vec3 B = CentroBase + direction * h;
    
    // Vetor normal no ponto de interseção
    Vec3 N = (P_I - B).normalize();
    
    // Calculando a normal ajustada pela inclinação do cone
    float k = altura / raioBase; // Corrigido para h/R
    N = (N - direction * k).normalize();
    
    // Verificar a direção da normal
    // Se estiver invertida, inverta
    if (N.dot(raio.direction) > 0) {
        N = -N;
    }
    
    // Vetor que vai do ponto de interseção até o observador (raio inverso)
    Vec3 v = -raio.direction.normalize();
    
    // Vetor que vai do ponto de interseção até a luz
    Vec3 l = (luz.position - P_I).normalize();
    
    // Vetor refletido da luz no ponto de interseção
    Vec3 r = (N * 2.0f * N.dot(l) - l).normalize();
    
    // Calculando os produtos escalares
    float NdotL = std::max(0.0f, N.dot(l));    // Difuso
    float RdotV = std::max(0.0f, r.dot(v));    // Especular
    
    // Componentes de iluminação
    Vec3 Ia = Ka * luzAmb.intensity;                       // Ambiente
    Vec3 Id = Kd * luz.intensity * NdotL;                  // Difusa
    Vec3 Ie = Ke * luz.intensity * std::pow(RdotV, m);     // Especular
    
    // Retornando a soma das componentes de iluminação
    return Ia + Id + Ie;
}
