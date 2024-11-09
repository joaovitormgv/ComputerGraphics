#include "../include/Ray.h"

Ray::Ray(Vec3 origin_, Vec3 end_) : origin(origin_) {
    Vec3 dir = end_ - origin;
    direction = dir.normalize();
}