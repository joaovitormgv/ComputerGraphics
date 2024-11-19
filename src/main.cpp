#include <SDL.h>
#include <iostream>
#include "../include/Vec3.h"
#include "../include/Ray.h"
#include "../include/Sphere.h"
#include "../include/PontoLuminoso.h"
#include "../include/Plane.h"
#include "../include/Cilindro.h"
#include "../include/Cone.h"

using namespace std;

const float wJanela = 60.0f;                    // Largura da janela em metros
const float hJanela = 60.0f;                    // Altura da janela em metros
const float dJanela = 30.0f;                    // Distância da janela virtual em metros
const float rEsfera = 40.0f;                    // Raio da esfera
const Vec3 centroEsfera(0.0f, 0.0f, -100.0f);   // Centro da esfera
const SDL_Color bgColor = {100, 100, 100, 255}; // Cor de fundo (cinza)
const int nCol = 500;                           // Número de colunas (pixels na horizontal)
const int nLin = 500;                           // Número de linhas (pixels na vertical)

PontoLuminoso luz(Vec3(0.0f, 60.0f, -30.0f), Vec3(0.7f, 0.7f, 0.7f));

PontoLuminoso luzAmb(Vec3(0.0f, 60.0f, -30.0f), Vec3(0.3f, 0.3f, 0.3f));

// Coeficientes de reflexão da esfera
const Vec3 Ke_d(0.7f, 0.2f, 0.2f); // Coeficiente de reflexão difusa (vermelho)
const Vec3 Ke_e(0.7f, 0.2f, 0.2f); // Coeficiente de reflexão especular (vermelho)
const Vec3 Ke_a(0.7f, 0.2f, 0.2f); // Coeficiente de reflexão ambiente (vermelho)

const float m = 10.0f; // Expoente de brilho (shininess)

// Plano do chão
const Plane chao(Vec3(0.0f, -rEsfera, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
const Vec3 Kc_d(0.2f, 0.7f, 0.2f); // Coeficiente de reflexão difusa (verde)
const Vec3 Kc_a = Kc_d;            // Coeficiente de reflexão ambiente (verde)
const Vec3 Kc_e(0.0f, 0.0f, 0.0f); //

const float m_c = 1.0f; // Expoente de brilho (shininess)

// Plano de fundo
const Plane fundo(Vec3(0.0f, 0.0f, -200.0f), Vec3(0.0f, 0.0f, 1.0f));
const Vec3 Kf_d(0.3f, 0.3f, 0.7f); // Coeficiente de reflexão difusa (azul)
const Vec3 Kf_a = Kf_d;            // Coeficiente de reflexão ambiente (azul)
const Vec3 Kf_e(0.0f, 0.0f, 0.0f); // Coeficiente de reflexão especular (azul)

const float m_f = 1.0f; // Expoente de brilho (shininess)

void setRenderColor(SDL_Renderer *renderer, const Vec3 &I)
{
    const int R = std::min(255, std::max(0, (int)(I.x * 255)));
    const int G = std::min(255, std::max(0, (int)(I.y * 255)));
    const int B = std::min(255, std::max(0, (int)(I.z * 255)));
    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
}

int main(int argc, char *argv[])
{
    // Inicializar a biblioteca para poder usar suas funções
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Não foi possível inicializar o SDL! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    // Criar uma janela
    SDL_Window *window = SDL_CreateWindow(
        "CG I - Raycaster",      // Título da Janela
        SDL_WINDOWPOS_UNDEFINED, // Posição inicial X
        SDL_WINDOWPOS_UNDEFINED, // Posição inicial Y
        nCol,                    // Largura da janela em pixels
        nLin,                    // Altura da janela em pixels
        SDL_WINDOW_SHOWN         // Flags
    );

    // Verificar se a janela foi criada corretamente
    if (!window)
    {
        SDL_Log("Criação da janela falhou! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Criar o objeto renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_Log("Criação do renderer falhou! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Definir a cor de fundo
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Criar a esfera
    Sphere esfera(centroEsfera, rEsfera);

    // Criar o cilindro
    float alturaCilindro = 1.5 * rEsfera;
    float raioCilindro = rEsfera / 3;
    Vec3 dirCilindro = Vec3(-1 / std::sqrt(3), 1 / std::sqrt(3), 1 / std::sqrt(3));
    Vec3 centroCilindro = centroEsfera;

    // Vec3 dirCilindro = Vec3(0.0f, 1.0f, 0.0f);
    Cilindro cilindro(alturaCilindro, raioCilindro, dirCilindro.normalize(), centroCilindro);
    Vec3 Kd_c = Vec3(0.2f, 0.3f, 0.8f);
    Vec3 Ka_c = Kd_c;
    Vec3 Ke_c = Kd_c;

    // Criar o cone
    float raioBase = 1.5 * raioCilindro;
    float alturaCone = raioBase/3;
    Vec3 dirCone = dirCilindro;
    // Vec3 dirCone = Vec3(0.0, 1.0, 0);

    Vec3 centroBase = centroCilindro + dirCilindro * alturaCilindro;
    // Vec3 centroBase = centroEsfera + Vec3(0.0, 1.0, 0.0).normalize() * 2 * rEsfera;

    Cone cone(alturaCone, raioBase, dirCone.normalize(), centroBase);
    Vec3 Kd_cone = Vec3(0.8f, 0.3f, 0.2f);

    // Definir as dimensões dos pixels virtuais
    float Dx = wJanela / nCol;
    float Dy = hJanela / nLin;

    // Main loop - É dentro do main while loop que fazemos todas as interações com a janela
    bool isRunning = true; // variável de controle do loop
    SDL_Event event;       // variável para checar os eventos da janela

while (isRunning)
{
    // Tratamento de eventos
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            isRunning = false;
        }
    }

    // Limpar o renderer com a cor de fundo
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);

    // Laço para cada pixel da janela
    for (int lin = 0; lin < nLin; lin++)
    {
        float yp = hJanela / 2.0f - Dy / 2.0f - lin * Dy;
        for (int col = 0; col < nCol; col++)
        {
            float xp = -wJanela / 2.0f + Dx / 2.0f + col * Dx;

            Vec3 pontoCentroPixel(xp, yp, -dJanela);
            Vec3 dr = (pontoCentroPixel - Vec3(0.0f, 0.0f, 0.0f)).normalize(); // Direção do raio

            Ray raio(Vec3(0.0f, 0.0f, 0.0f), dr);

            // Inicialização dos tempos de interseção
            float tEsfera = std::numeric_limits<float>::max();
            float tChao = std::numeric_limits<float>::max();
            float tFundo = std::numeric_limits<float>::max();
            float tCilindro = std::numeric_limits<float>::max();
            float tCone = std::numeric_limits<float>::max();

            // Verificar interseções com os objetos
            bool hitEsfera = esfera.intersect(raio, tEsfera);
            bool hitChao = chao.intersect(raio, tChao);
            bool hitFundo = fundo.intersect(raio, tFundo);
            bool hitCilindro = cilindro.intersect(raio, tCilindro);
            bool hitCone = cone.intersect(raio, tCone);

            // Encontrar o menor t e o objeto correspondente
            float menor_t = std::numeric_limits<float>::max();
            int objeto = -1;

            if (hitEsfera && tEsfera < menor_t) {
                menor_t = tEsfera;
                objeto = 0; // Índice da esfera
            }

            if (hitCone && tCone < menor_t) {
                menor_t = tCone;
                objeto = 6; // Índice do cone
            }

            if (hitCilindro && tCilindro < menor_t) {
                menor_t = tCilindro;
                objeto = 3; // Índice do cilindro
            }

            if (hitChao && tChao < menor_t) {
                menor_t = tChao;
                objeto = 1; // Índice do chão
            }

            if (hitFundo && tFundo < menor_t) {
                menor_t = tFundo;
                objeto = 2; // Índice do fundo
            }

            // Renderizar com base no objeto com menor_t
            if (objeto != -1 && menor_t < std::numeric_limits<float>::max()) {
                Vec3 P_I = raio.origin + raio.direction * menor_t;
                Vec3 I;

                switch (objeto) {
                    case 0: // Esfera
                        I = esfera.calculateLighting(P_I, raio, luz, luzAmb, Ke_d, Ke_a, Ke_e, m);
                        break;
                    case 1: // Chão
                        I = chao.calculateLighting(esfera, P_I, raio, luz, luzAmb, Kc_d, Kc_a, Kc_e, m_c);
                        break;
                    case 2: // Fundo
                        I = fundo.calculateLighting(esfera, P_I, raio, luz, luzAmb, Kf_d, Kf_a, Kf_e, m_f);
                        break;
                    case 3: // Cilindro
                        I = cilindro.calculateLighting(P_I, raio, luz, luzAmb, Kd_c, Ka_c, Ke_c, m);
                        break;
                    case 6: // Cone
                        I = cone.calculateLighting(P_I, raio, luz, luzAmb, Kd_cone, Kd_cone, Kd_cone, m);
                        break;
                    // Adicione outros casos se houver mais objetos
                    default:
                        I = Vec3(0.0f, 0.0f, 0.0f); // Cor padrão
                        break;
                }

                setRenderColor(renderer, I);
            }
            else {
                // Se não houver interseção, definir a cor de fundo
                SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            }

            // Desenhar o ponto
            SDL_RenderDrawPoint(renderer, col, lin);
        }
    }

    // Atualizar a janela com o conteúdo renderizado
    SDL_RenderPresent(renderer);
}

    // Destruir os objetos criados para limpar a memória
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}