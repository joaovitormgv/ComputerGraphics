#include <SDL.h>
#include <iostream>
#include "../include/Vec3.h"
#include "../include/Ray.h"
#include "../include/Sphere.h"
#include "../include/PontoLuminoso.h"
#include "../include/Plane.h"

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

    // Definir as dimensões dos pixels virtuais
    float Dx = wJanela / nCol;
    float Dy = hJanela / nLin;

    // Main loop - É dentro do main while loop que fazemos todas as interações com a janela
    bool isRunning = true; // variável de controle do loop
    SDL_Event event;       // variável para checar os eventos da janela

    while (isRunning)
    {
        // É possível interagir com a janela, com clicks, apertando teclas ou clicando para fechar a janela
        // essas interações são chamadas de "eventos", abaixo, verificamos se o evento de fechar janela ocorre
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

        // Definir a cor para a esfera

        // Aqui temos a estrutura para pintar um pixel, no caso, um loop para pintar todos os pixeis da janela
        for (int lin = 0; lin < nLin; lin++)
        {
            float yp = hJanela / 2.0f - Dy / 2.0f - lin * Dy;
            for (int col = 0; col < nCol; col++)
            {
                float xp = -wJanela / 2.0f + Dx / 2.0f + col * Dx;

                Vec3 pontoCentroPixel(xp, yp, -dJanela);

                Ray raio(Vec3(0.0f, 0.0f, 0.0f), pontoCentroPixel);

                float tEsfera = std::numeric_limits<float>::max();
                float tChao = std::numeric_limits<float>::max();
                float tFundo = std::numeric_limits<float>::max();

                bool hitEsfera = esfera.intersect(raio, tEsfera);
                bool hitChao = chao.intersect(raio, tChao);
                bool hitFundo = fundo.intersect(raio, tFundo);

                if (hitEsfera && (tEsfera < std::fabs(tChao) && tEsfera < std::fabs(tFundo)))
                {
                    // Ponto de interseção do raio com a esfera
                    Vec3 P_I = raio.origin + raio.direction * tEsfera;

                    // Vetor normal à superfície da esfera no ponto de interseção
                    Vec3 N = (P_I - esfera.center).normalize();

                    // Vetor que vai do ponto de interseção até o olho do observador
                    Vec3 v = -raio.direction.normalize();

                    // Vetor que vai do ponto de interseção até a fonte de luz
                    Vec3 l = (luz.position - P_I).normalize();

                    // vetor de reflexão da luz
                    Vec3 r = (N * 2 * N.dot(l) - l).normalize();

                    // Calculando produtos escalares
                    // impedindo que o produto escalar seja negativo
                    float NdotL = std::max(0.0f, N.dot(l));
                    float RdotV = std::max(0.0f, r.dot(v));

                    // Calcular a intensidade da reflexão difusa
                    Vec3 I_d = Ke_d * luz.intensity * NdotL;

                    // Calcular a intensidade da reflexão especular
                    Vec3 I_e = Ke_e * luz.intensity * std::pow(RdotV, m);

                    // Calcular a intensidade da reflexão ambiente
                    Vec3 I_a = Ke_a * luzAmb.intensity;

                    Vec3 I = I_d + I_e + I_a;

                    // Converter a intensidade para a faixa de 0 a 255
                    int R = std::min(255, std::max(0, (int)(I.x * 255)));
                    int G = std::min(255, std::max(0, (int)(I.y * 255)));
                    int B = std::min(255, std::max(0, (int)(I.z * 255)));

                    SDL_SetRenderDrawColor(renderer, R, G, B, 255);
                }
                else if (hitChao && tChao < tEsfera && tChao < tFundo)
                {
                    // Ponto de interseção do raio com o chão
                    Vec3 P_I = raio.origin + raio.direction * tChao;

                    Vec3 N = chao.normal;

                    Vec3 v = -raio.direction.normalize();

                    Vec3 l = (luz.position - P_I).normalize();

                    Ray raioSombra(P_I + N * 1e-4f, luz.position);
                    float tEsferaSombra = std::numeric_limits<float>::max();
                    bool hitEsferaSombra = esfera.intersect(raioSombra, tEsferaSombra);

                    if (hitEsferaSombra)
                    {
                        Vec3 I_a = Kc_a * luzAmb.intensity;

                        Vec3 I = I_a;

                        // Converter a intensidade para a faixa de 0 a 255
                        int R = std::min(255, std::max(0, (int)(I.x * 255)));
                        int G = std::min(255, std::max(0, (int)(I.y * 255)));
                        int B = std::min(255, std::max(0, (int)(I.z * 255)));

                        SDL_SetRenderDrawColor(renderer, R, G, B, 255);
                    }
                    else
                    {
                        Vec3 r = (N * 2 * N.dot(l) - l).normalize();

                        // Calcnulando produtos escalares
                        float NdotL = std::max(0.0f, N.dot(l));
                        float RdotV = std::max(0.0f, r.dot(v));

                        Vec3 I_d = Kc_d * luz.intensity * NdotL;

                        Vec3 I_e = Kc_e * luz.intensity * std::pow(RdotV, m_c);

                        Vec3 I_a = Kc_a * luzAmb.intensity;

                        Vec3 I = I_d + I_e + I_a;

                        // Converter a intensidade para a faixa de 0 a 255
                        int R = std::min(255, std::max(0, (int)(I.x * 255)));
                        int G = std::min(255, std::max(0, (int)(I.y * 255)));
                        int B = std::min(255, std::max(0, (int)(I.z * 255)));

                        SDL_SetRenderDrawColor(renderer, R, G, B, 255);
                    }
                }
                else if (hitFundo && tFundo < tEsfera)
                {
                    // Ponto de interseção do raio com o chão
                    Vec3 P_I = raio.origin + raio.direction * tFundo;

                    Vec3 N = fundo.normal;

                    Vec3 v = -raio.direction.normalize();

                    Vec3 l = (luz.position - P_I).normalize();

                    Ray raioSombra(P_I + N * 1e-4f, luz.position);
                    float tEsferaSombra = std::numeric_limits<float>::max();
                    bool hitEsferaSombra = esfera.intersect(raioSombra, tEsferaSombra);
                    
                    if (hitEsferaSombra) {
                        Vec3 I_a = Kf_a * luzAmb.intensity;

                        Vec3 I = I_a;    

                        // Converter a intensidade para a faixa de 0 a 255
                        int R = std::min(255, std::max(0, (int)(I.x * 255)));
                        int G = std::min(255, std::max(0, (int)(I.y * 255)));
                        int B = std::min(255, std::max(0, (int)(I.z * 255)));

                        SDL_SetRenderDrawColor(renderer, R, G, B, 255);
                    } else {
                        Vec3 r = (N * 2 * N.dot(l) - l).normalize();

                        // Calcnulando produtos escalares
                        float NdotL = std::max(0.0f, N.dot(l));
                        float RdotV = std::max(0.0f, r.dot(v));

                        Vec3 I_d = Kf_d * luz.intensity * NdotL;

                        Vec3 I_e = Kf_e * luz.intensity * std::pow(RdotV, m_f);

                        Vec3 I_a = Kf_a * luzAmb.intensity;

                        Vec3 I = I_d + I_e + I_a;

                        // Converter a intensidade para a faixa de 0 a 255
                        int R = std::min(255, std::max(0, (int)(I.x * 255)));
                        int G = std::min(255, std::max(0, (int)(I.y * 255)));
                        int B = std::min(255, std::max(0, (int)(I.z * 255)));

                        SDL_SetRenderDrawColor(renderer, R, G, B, 255);
                    }
                }
                else
                {
                    // Se não houver interseção, definir a cor de fundo
                    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
                }
                SDL_RenderDrawPoint(renderer, col, lin);
            }

            // Por fim, atualizamos a janela com o renderer que acabamos de pintar e tudo deve funcionar corretamente
        }
        SDL_RenderPresent(renderer);
    }

    // Destruir os objetos criados para limpar a memória
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}