# AP1 — CG I — 2026.2

Calculadora da energia RGB na direção de observação do enunciado, com relatório das contas e auditoria em JSON. Python 3, somente biblioteca padrão, sem instalação de pacotes.

Na raiz do repositório:

```sh
python3 ap1/solver.py --k 4 --output ap1/resultados
# Ou informe os seis dígitos reais (a matrícula não é gravada):
python3 ap1/solver.py --matricula 751004 --output ap1/resultados
python3 -m unittest discover -s ap1 -v
```

Os valores acima são exemplos; a matrícula 751004 é a do exemplo do enunciado, não a do aluno. Sem `--output`, a explicação aparece no terminal. O resultado específico depende da matrícula real.

## Relação com o código anterior

As operações vetoriais, a equação raio–esfera, a interseção raio–plano e o modelo de Phong foram adaptados de `include/Vec3.h`, `src/Vec3.cpp`, `src/Sphere.cpp`, `src/Plane.cpp` e `src/main.cpp`. Os fontes C++ e a configuração SDL foram preservados. Aqui usamos números de precisão dupla, aceitamos a raiz positiva de saída quando a origem está dentro da esfera, limitamos sombras à distância da fonte e desativamos o especular quando N·l ≤ 0.

O programa original renderiza uma imagem; esta AP pede uma única direção e os passos numéricos. Por isso a calculadora não precisa de SDL, câmera com resolução de imagem ou janela. Foram acrescentadas as interseções analíticas com cilindro e cone finitos e as faces triangulares da pirâmide. A malha da pirâmide representa exatamente suas faces planas; as superfícies curvas não são aproximadas por triângulos.

## Convenções a conferir

- “Anel cilíndrico”: lateral de cilindro finito sem tampas e sem espessura. Só um raio é fornecido; um tubo espesso precisaria de outro raio. Esta interpretação ainda não foi confirmada nos materiais do professor.
- Cone sem disco de base. Pirâmide com base quadrada em y=0 e ápice em C4+(0,H4,0).
- Os vetores D1 e D3 indicam a direção do eixo e são normalizados antes de aplicar a altura em metros.
- Raio na direção `Cα−PE`, com t>0. O alvo não é necessariamente a superfície visível: outro objeto pode estar à frente, e podem existir interseções depois do alvo.
- Phong com normal externa geométrica; sem inversão automática da normal de superfícies abertas. Sem atenuação por distância (nenhum coeficiente fornecido), sem correção gama e sem reflexão recursiva.
- Sombras duras: a luz pontual é bloqueada apenas por uma interseção anterior à fonte. Ambiente permanece presente na sombra.
- Somar as três parcelas primeiro; limitar cada canal RGB a [0,1] somente ao final.
- Arestas de pirâmide e ápice de cone são pontos de normal não única; o ápice singular gera erro explícito. Os dez raios do enunciado não atingem o ápice do cone.

## O que é exigido na entrega

O PDF pede explicar cada passo, com expressões, valores dos termos e resultados. Código sozinho não substitui essa explicação. No Classroom, a atividade AP1.2026 indica entrega em **20/10/2026 às 12h**. A calculadora gera Markdown e JSON; a matrícula real e o resultado correspondente estão registrados abaixo. Nenhuma atividade foi entregue e nenhum arquivo foi publicado no GitHub.

## Caso da matrícula 553078

`k = (300 + 5 + 5 + 3 + 0 + 7 + 8) % 10 = 8`. O raio atinge primeiro a face +x da pirâmide, em `(197.59425890, 70.75903947, 184.05078664)`. O anel bloqueia a luz a cerca de 63.611994 m desse ponto, antes da fonte. Por isso a energia final é somente a ambiente: **RGB = (0.25, 0.125, 0.25)**. Os resultados dessa matrícula acompanham o pacote. A suíte inclui uma conferência independente da interseção com o plano usando aritmética racional e do pertencimento do ponto de bloqueio ao anel.
