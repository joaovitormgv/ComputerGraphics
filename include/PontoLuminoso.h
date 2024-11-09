#ifndef PONTOLUMINOSO_H
#define PONTOLUMINOSO_H

#include "Vec3.h"

class PontoLuminoso
{
public:
    Vec3 position;
    Vec3 intensity;

    PontoLuminoso(Vec3 position_, Vec3 intensity_) : position(position_), intensity(intensity_) {}
};

#endif // PONTOLUMINOSO_H