"""Casos geométricos com soluções conhecidas e resíduos para os dez cenários."""
import math
import unittest
from solver import scene, solve, intersections, unit, sub, add, mul, dot, norm, roots


class GeometryTests(unittest.TestCase):
    def test_k8_independent_plane_and_shadow_residual(self):
        from fractions import Fraction
        # Plano da face +x: 6.8(x-158)+y=340.
        # Substituição do raio NÃO normalizado: lambda=2637/3623.
        lam=Fraction(2637,3623)
        expected=tuple(float(Fraction(o)+lam*d) for o,d in zip((480,260,120),(-388,-260,88)))
        r=solve(8)
        for actual,wanted in zip(r['hit']['point'],expected):
            self.assertAlmostEqual(actual,wanted,places=9)
        self.assertEqual(r['hit']['object'],'pirâmide face 2')
        self.assertEqual(r['rgb'],(0.25,0.125,0.25))
        blocker=r['blockers'][0]
        self.assertEqual(blocker['object'],'anel cilíndrico')
        # O ponto bloqueador deve pertencer à lateral finita do anel.
        w=sub(blocker['point'],(92,0,208)); u=unit((8,7,4)); z=dot(w,u)
        self.assertAlmostEqual(norm(sub(w,mul(u,z))),18,places=8)
        self.assertTrue(0<z<165)
        self.assertLess(blocker['t'],r['shadow_distance'])

    def test_quadratic_and_linear(self):
        self.assertEqual(roots(0,2,-6), [3])
        self.assertEqual(roots(1,-2,1), [1])
        self.assertEqual(roots(1,0,1), [])
        self.assertEqual(roots(1,-3,2), [1,2])

    def test_sphere_from_inside_and_tangent(self):
        s = scene(0)
        c = s['centers'][1]
        hits, _ = intersections(s,c,(1,0,0))
        sphere = [h for h in hits if h['object']=='esfera']
        self.assertAlmostEqual(sphere[0]['t'],20)
        hits, _ = intersections(s,add(c,(-30,20,0)),(1,0,0))
        sphere = [h for h in hits if h['object']=='esfera']
        self.assertEqual(len(sphere),1)
        self.assertAlmostEqual(sphere[0]['t'],30)

    def test_open_cylinder_has_no_caps(self):
        s = scene(0)
        s.update(axis1=(0,1,0),height1=10,radius1=2)
        c = s['centers'][0]
        hits, _ = intersections(s,add(c,(0,-5,0)),(0,1,0))
        self.assertFalse(any(h['object']=='anel cilíndrico' for h in hits))
        hits, _ = intersections(s,add(c,(-5,5,0)),(1,0,0))
        ring = [h for h in hits if h['object']=='anel cilíndrico']
        self.assertEqual([h['t'] for h in ring],[3,7])
        hits, _ = intersections(s,add(c,(-5,11,0)),(1,0,0))
        self.assertFalse(any(h['object']=='anel cilíndrico' for h in hits))

    def test_cone_half_height_section(self):
        s = scene(0)
        s.update(axis3=(0,1,0),height3=10,radius3=4)
        c = s['centers'][2]
        hits, _ = intersections(s,add(c,(-5,5,0)),(1,0,0))
        cone = [h for h in hits if h['object']=='cone']
        self.assertEqual([h['t'] for h in cone],[3,7])
        self.assertAlmostEqual(cone[0]['normal'][0],-1/math.sqrt(1.16))
        self.assertAlmostEqual(cone[0]['normal'][1],0.4/math.sqrt(1.16))

    def test_pyramid_section_and_outward_normal(self):
        s=scene(0)
        c=s['centers'][3]
        hits,_=intersections(s,add(c,(0,10,30)),(0,0,-1))
        pyramid=[h for h in hits if h['object'].startswith('pirâmide')]
        self.assertEqual(len(pyramid),2)
        self.assertAlmostEqual(pyramid[0]['t'],25)
        self.assertAlmostEqual(pyramid[1]['t'],35)
        self.assertGreater(pyramid[0]['normal'][2],0)

    def test_all_k_surface_residuals_and_phong(self):
        for k in range(10):
            with self.subTest(k=k):
                r=solve(k)
                s=r['scene']
                self.assertAlmostEqual(norm(r['direction']),1)
                for hit in r['intersections']:
                    p,n=hit['point'],hit['normal']
                    self.assertAlmostEqual(norm(n),1)
                    self.assertGreater(hit['t'],0)
                    if hit['object']=='esfera':
                        self.assertAlmostEqual(norm(sub(p,s['centers'][1])),s['radius2'],places=7)
                    elif hit['object'] in ('anel cilíndrico','cone'):
                        i=1 if hit['object']=='anel cilíndrico' else 3
                        w=sub(p,s['centers'][i-1]); z=dot(w,s[f'axis{i}'])
                        radial=sub(w,mul(s[f'axis{i}'],z))
                        radius=s[f'radius{i}'] * (1 if i==1 else 1-z/s['height3'])
                        self.assertAlmostEqual(norm(radial),radius,places=7)
                        self.assertGreaterEqual(z,-1e-7)
                        self.assertLessEqual(z,s[f'height{i}']+1e-7)
                    elif hit['object'].startswith('pirâmide'):
                        # Verificação independente pela equação implícita da pirâmide.
                        x,y,z=sub(p,s['centers'][3]); a=s['side4']/2
                        if abs(y)>1e-7:
                            self.assertAlmostEqual(max(abs(x),abs(z))/a+y/s['height4'],1,places=7)
                        else:
                            self.assertLessEqual(max(abs(x),abs(z)),a+1e-7)
                    else:
                        self.assertAlmostEqual(p[1],0,places=7)
                self.assertTrue(all(0<=x<=1 for x in r['rgb']))
                self.assertAlmostEqual(r['raw'][0],r['raw'][2])
                self.assertAlmostEqual(r['raw'][0],2*r['raw'][1])
                if not r['visible']:
                    self.assertEqual(r['rgb'],(0.25,0.125,0.25))
                    self.assertTrue(all(0<h['t']<r['shadow_distance'] for h in r['blockers']))


if __name__=='__main__':
    unittest.main()
