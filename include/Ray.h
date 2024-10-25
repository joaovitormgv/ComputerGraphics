#ifndef RAY_H
#define RAY_H

#include "Vec3.h"

class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    // Construtor que aceita origem e ponto final
    Ray(Vec3 origin_, Vec3 end_);
};

#endif // RAY_H