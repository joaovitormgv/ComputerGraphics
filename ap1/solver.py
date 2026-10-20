#!/usr/bin/env python3
"""AP1 CG I 2026.2: um raio, interseções analíticas e iluminação de Phong.

Sem dependências externas. Convenções explícitas no README.md.
As equações de esfera, plano e Phong seguem o raycaster C++ deste repo.
"""
import argparse
import json
import math
from pathlib import Path

EPS = 1e-8


def add(a, b): return tuple(x + y for x, y in zip(a, b))
def sub(a, b): return tuple(x - y for x, y in zip(a, b))
def mul(a, s): return tuple(x * s for x in a)
def dot(a, b): return sum(x * y for x, y in zip(a, b))
def norm(a): return math.sqrt(dot(a, a))
def unit(a):
    if norm(a) < EPS:
        raise ValueError("Vetor nulo não tem direção unitária")
    return mul(a, 1 / norm(a))
def cross(a, b):
    return (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])


def roots(a, b, c):
    """Inclui degeneração linear e raízes negativas para auditoria."""
    if abs(a) < 1e-12:
        return [] if abs(b) < 1e-12 else [-c / b]
    delta = b*b - 4*a*c
    tol = 1e-12 * max(1, b*b, abs(4*a*c))
    if delta < -tol:
        return []
    if abs(delta) <= tol:
        return [-b / (2*a)]
    # Forma estável evita cancelamento no cálculo de uma das raízes.
    q = -0.5 * (b + math.copysign(math.sqrt(delta), b))
    return sorted([q/a, c/q])


def scene(k):
    r2 = 20 + k
    centers = [(100-k, 0, 200+k), (100-k, abs(20*k-r2/4), 200+k),
               (200+k, 0, 100-k), (150+k, 0, 150+k)]
    return dict(k=k, alpha=1+k%4, centers=centers,
                eye=(60*k, 100+20*k, 600-60*k), target=centers[k%4],
                radius1=10+k, height1=5+20*k, axis1=unit((abs(k), abs(k-1), abs(k-4))),
                radius2=r2, radius3=30+k, height3=10+50*k,
                axis3=unit((abs(k), abs(k-4), abs(k-1))),
                side4=20+10*k, height4=20+40*k)


def intersections(s, origin, direction):
    """Retorna todas as interseções positivas e os cálculos auditáveis.

    direction deve ser unitário; t é distância em metros.
    Anel = superfície lateral de cilindro finito, sem tampas/espessura.
    Cone = superfície lateral finita sem base. Pirâmide inclui sua base.
    """
    hits, trace = [], []

    def record(name, t, normal):
        if t > EPS:
            hits.append(dict(object=name, t=t, point=add(origin, mul(direction, t)), normal=unit(normal)))

    # Esfera: |O + td - C2|² = R2².
    w = sub(origin, s['centers'][1])
    a, b, c = dot(direction, direction), 2*dot(direction, w), dot(w, w)-s['radius2']**2
    rr = roots(a, b, c)
    trace.append(dict(object='esfera', a=a, b=b, c=c, delta=b*b-4*a*c, roots=rr))
    for t in rr:
        record('esfera', t, sub(add(origin, mul(direction, t)), s['centers'][1]))

    # Cilindro e cone: decompor w e d em partes axial e perpendicular.
    for index, name in [(1, 'anel cilíndrico'), (3, 'cone')]:
        center, axis = s['centers'][index-1], s[f'axis{index}']
        radius, height = s[f'radius{index}'], s[f'height{index}']
        w = sub(origin, center)
        z0, zd = dot(w, axis), dot(direction, axis)
        wp, dp = sub(w, mul(axis, z0)), sub(direction, mul(axis, zd))
        if index == 1:
            a, b, c = dot(dp, dp), 2*dot(wp, dp), dot(wp, wp)-radius**2
        else:
            q = (radius/height)**2
            a = dot(dp, dp)-q*zd**2
            b = 2*(dot(wp, dp)+q*(height-z0)*zd)
            c = dot(wp, wp)-q*(height-z0)**2
        rr = roots(a, b, c)
        detail = dict(object=name, axis=axis, z0=z0, zd=zd, wp=wp, dp=dp,
                      a=a, b=b, c=c, delta=b*b-4*a*c, roots=rr, candidates=[])
        for t in rr:
            z = z0+t*zd
            accepted = t > EPS and -EPS <= z <= height+EPS
            detail['candidates'].append(dict(t=t, height=z, accepted=accepted))
            if accepted:
                radial = add(wp, mul(dp, t))
                normal = radial if index == 1 else add(radial, mul(axis, q*(height-z)))
                if norm(normal) <= EPS:
                    raise ValueError('Raio atinge o ápice singular do cone; normal indefinida')
                record(name, t, normal)
        trace.append(detail)

    # Pirâmide: interseção com o plano de cada triângulo + baricêntricas.
    center, half, h = s['centers'][3], s['side4']/2, s['height4']
    vertices = [add(center, p) for p in [(-half,0,-half), (half,0,-half),
                                        (half,0,half), (-half,0,half), (0,h,0)]]
    faces = [(0,4,1), (1,4,2), (2,4,3), (3,4,0), (0,1,2), (0,2,3)]
    interior = add(center, (0, h/4, 0))
    for f, ids in enumerate(faces):
        A, B, C = [vertices[i] for i in ids]
        e1, e2 = sub(B, A), sub(C, A)
        n = unit(cross(e1, e2))
        if dot(n, sub(interior, A)) > 0:
            n = mul(n, -1)
        denom = dot(n, direction)
        detail = dict(object=f'pirâmide face {f+1}', vertices=[A,B,C], normal=n, denom=denom)
        if abs(denom) > EPS:
            t = dot(n, sub(A, origin))/denom
            p = add(origin, mul(direction, t))
            v = sub(p, A)
            aa, ab, bb = dot(e1,e1), dot(e1,e2), dot(e2,e2)
            det = aa*bb-ab*ab
            u = (bb*dot(v,e1)-ab*dot(v,e2))/det
            vcoord = (aa*dot(v,e2)-ab*dot(v,e1))/det
            accepted = t > EPS and u >= -EPS and vcoord >= -EPS and u+vcoord <= 1+EPS
            detail.update(t=t, barycentric=[1-u-vcoord,u,vcoord], accepted=accepted)
            if accepted:
                record(detail['object'], t, n)
        trace.append(detail)

    # Plano xz: y=0, normal +y.
    detail = dict(object='chão', denom=direction[1])
    if abs(direction[1]) > EPS:
        t = -origin[1]/direction[1]
        detail.update(t=t, accepted=t > EPS)
        record('chão', t, (0,1,0))
    trace.append(detail)
    return sorted(hits, key=lambda hit: hit['t']), trace


def solve(k):
    s = scene(k)
    direction = unit(sub(s['target'], s['eye']))
    hits, trace = intersections(s, s['eye'], direction)
    if not hits:
        raise ValueError('Raio sem interseção: cor de fundo não especificada')
    hit = hits[0]
    p, n = hit['point'], hit['normal']
    light = (300,300,1000)
    l, v = unit(sub(light,p)), mul(direction,-1)
    r = sub(mul(n, 2*dot(n,l)), l)
    nl, rv = dot(n,l), dot(r,v)
    # Superfícies abertas podem ser vistas por dentro; registrar normal geométrica.
    # Phong usa a normal externa, sem inverter automaticamente para o observador.
    shadow_origin = add(p, mul(l, 1e-6))
    shadow_hits, shadow_trace = intersections(s, shadow_origin, l)
    distance = norm(sub(light,shadow_origin))
    blockers = [h for h in shadow_hits if h['t'] < distance-EPS]
    visible = not blockers
    diffuse = max(0,nl) if visible else 0.0
    specular = max(0,rv)**3 if visible and nl > 0 else 0.0
    material = (0.5,0.25,0.5)
    ambient = mul(material,0.5)
    diff_rgb, spec_rgb = mul(material,diffuse), mul(material,specular)
    raw = add(add(ambient,diff_rgb),spec_rgb)
    return dict(scene=s, direction=direction, intersections=hits, trace=trace,
                hit=hit, light=light, l=l, v=v, r=r, n_dot_l=nl, r_dot_v=rv,
                shadow_origin=shadow_origin, shadow_distance=distance,
                shadow_hits=shadow_hits, blockers=blockers, shadow_trace=shadow_trace,
                visible=visible, diffuse_factor=diffuse, specular_factor=specular,
                ambient=ambient, diffuse=diff_rgb, specular=spec_rgb,
                raw=raw, rgb=tuple(min(1,max(0,x)) for x in raw))


def fmt(value):
    if isinstance(value, (tuple,list)):
        return '(' + ', '.join(fmt(x) for x in value) + ')'
    if isinstance(value, float):
        return f'{value:.10f}'
    return str(value)


def report(result):
    s, hit = result['scene'], result['hit']
    lines = [f'# AP1 — Computação Gráfica I — k = {s["k"]}', '',
             'Cálculo da energia na direção de observação. Valores em metros; vetores em coordenadas do mundo.', '',
             '## 1. Convenções', '',
             'Anel cilíndrico interpretado como superfície lateral de um cilindro finito, sem tampas e sem espessura. '
             'Cone sem base; pirâmide com base e ápice acima do chão. Direções dos eixos normalizadas. '
             'Usamos Phong com normais externas, sombras duras, sem atenuação por distância '
             '(o enunciado não fornece coeficientes), sem reflexões recursivas e sem correção gama. '
             'O raio é uma semirreta: t > 0, sem limitar t à distância até o alvo. '
             'Estas convenções devem ser conferidas com as adotadas na disciplina.', '',
             '## 2. Parâmetros e raio', '',
             f'k = (300 + A + B + C + D + E + F) % 10 = {s["k"]}.',
             f'α = 1 + (k % 4) = {s["alpha"]}; P_at = C{ s["alpha"] } = {fmt(s["target"])}.', '']
    for key, value in s.items():
        lines.append(f'- {key}: {fmt(value)}')
    lines += ['', 'd = (P_at − P_E) / ‖P_at − P_E‖; P(t) = P_E + t d.',
              f'd = {fmt(result["direction"])}.', '',
              '## 3. Interseções e escolha da superfície visível', '',
              'Esfera: w = P_E − C2; a = d·d, b = 2d·w, c = w·w − R2².',
              'Anel: u = D1/‖D1‖; w = P_E − C1; z0 = w·u; zd = d·u; '
              'w⊥ = w − z0u; d⊥ = d − zdu. '
              'a = d⊥·d⊥; b = 2w⊥·d⊥; c = w⊥·w⊥ − R1².',
              'Cone: usar C3 e u = D3/‖D3‖ na mesma decomposição, com q = (R3/H3)². '
              'a = d⊥·d⊥ − q zd²; b = 2[w⊥·d⊥ + q(H3 − z0)zd]; '
              'c = w⊥·w⊥ − q(H3 − z0)².',
              'Em ambos: resolver at² + bt + c = 0; Δ = b² − 4ac. '
              'Aceitar somente t > 0 e 0 ≤ z0 + t zd ≤ H. '
              'Se a = 0, resolver a equação linear.',
              'Pirâmide: para cada triângulo ABC, n = normal externa unitária de (B−A)×(C−A); '
              't = n·(A−P_E)/(n·d). Aceitar se t > 0 e as três coordenadas baricêntricas forem não negativas. '
              'As faces 1–4 são laterais; 5–6 triangulam a base.',
              'Chão: t = −P_Ey/dy, normal (0,1,0). Se dy = 0, o raio é paralelo.', '']
    for detail in result['trace']:
        lines += [f'### {detail["object"]}', '', '```json', json.dumps(detail,ensure_ascii=False,indent=2), '```', '']
    lines += ['Todas as interseções válidas em ordem crescente de t:', '',
              '| Superfície | t (m) |', '|---|---:|']
    for h in result['intersections']:
        lines.append(f'| {h["object"]} | {fmt(h["t"])} |')
    lines += ['', f'A primeira superfície é **{hit["object"]}**, com t = {fmt(hit["t"])}.',
              f'P = P_E + td = {fmt(hit["point"])}.', '',
              '## 4. Normal e vetores da iluminação', '',
              'Esfera: N = (P − C2)/R2. Anel: N = normalize(P − C1 − [(P−C1)·u]u). '
              'Cone: z = (P−C3)·u; radial = P−C3−zu; '
              'N = normalize(radial + (R3/H3)²(H3−z)u). '
              'Pirâmide: normal externa da face; chão: (0,1,0).',
              f'N = {fmt(hit["normal"])}.',
              'l = normalize(P_F − P); v = normalize(P_E − P) = −d; r = 2(N·l)N − l.',
              f'l = {fmt(result["l"])}.', f'v = {fmt(result["v"])}.',
              f'r = {fmt(result["r"])}.', f'N·l = {fmt(result["n_dot_l"])}.',
              f'r·v = {fmt(result["r_dot_v"])}.', '', '## 5. Visibilidade da luz', '',
              'Traçamos S(t) = (P + εl) + tl até a fonte, com ε = 10⁻⁶ m. '
              'Somente interseções entre a superfície e a fonte bloqueiam a luz.',
              f'Origem = {fmt(result["shadow_origin"])}; distância até a luz = {fmt(result["shadow_distance"])}.',
              f'Fonte visível: {"sim" if result["visible"] else "não"}.', '',
              'Interseções positivas desse raio (incluindo as que estão além da luz):', '',
              '```json', json.dumps(result['shadow_hits'],ensure_ascii=False,indent=2), '```', '',
              'Bloqueadores antes da fonte:', '```json', json.dumps(result['blockers'],ensure_ascii=False,indent=2), '```', '',
              'Os coeficientes e testes de todas as superfícies no raio de sombra estão no arquivo JSON.', '',
              '## 6. Energia RGB', '',
              'Com K_A = K_D = K_E = (0.5,0.25,0.5), I_A = (0.5,0.5,0.5), I_F = (1,1,1), e = 3:',
              'E_amb = K_A ⊙ I_A.',
              'E_dif = V (K_D ⊙ I_F) max(0,N·l).',
              'E_esp = V (K_E ⊙ I_F) max(0,r·v)³, somente se N·l > 0; caso contrário E_esp = 0.',
              'V = 1 quando a luz está visível; V = 0 quando bloqueada. ⊙ é o produto componente a componente.',
              f'Fator difuso efetivo = {fmt(result["diffuse_factor"])}.',
              f'Fator especular efetivo = {fmt(result["specular_factor"])}.',
              f'E_amb = {fmt(result["ambient"])}.', f'E_dif = {fmt(result["diffuse"])}.',
              f'E_esp = {fmt(result["specular"])}.',
              f'E_bruta = E_amb + E_dif + E_esp = {fmt(result["raw"])}.',
              f'**E_E = clamp(E_bruta, 0, 1) = {fmt(result["rgb"])}.**', '']
    return '\n'.join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--k', type=int, choices=range(10))
    group.add_argument('--matricula', help='seis dígitos; somente k será salvo')
    parser.add_argument('--output', type=Path, help='pasta para relatório Markdown e auditoria JSON')
    args = parser.parse_args()
    if args.matricula is not None:
        if len(args.matricula) != 6 or not all(c in '0123456789' for c in args.matricula):
            parser.error('matrícula deve ter exatamente seis dígitos ASCII')
        k = (300 + sum(map(int,args.matricula))) % 10
    else:
        k = args.k
    result = solve(k)
    if args.output:
        args.output.mkdir(parents=True,exist_ok=True)
        (args.output/f'ap1-k{k}.md').write_text(report(result),encoding='utf-8')
        (args.output/f'ap1-k{k}.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(f'k = {k}; alvo C{result["scene"]["alpha"]}; superfície: {result["hit"]["object"]}')
    print(f'E_E = {fmt(result["rgb"])}')
    if not args.output:
        print(report(result))


if __name__ == '__main__':
    main()
