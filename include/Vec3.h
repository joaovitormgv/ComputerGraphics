#ifndef VEC3_H
#define VEC3_H

#include <cmath>

class Vec3{
public:
    float x, y, z;

    Vec3(float x_=0, float y_=0, float z_=0);

    Vec3 operator + (const Vec3& v ) const;
    Vec3 operator - (const Vec3& v ) const;
    Vec3 operator * (float s ) const;
    Vec3 operator * (const Vec3& v) const;
    Vec3 operator - () const;
    float dot(const Vec3& v ) const; // Produto escalar
    float norma() const; // Magnitude
    Vec3 normalize() const;
    Vec3 cross(const Vec3& v) const; // Produto vetorial
};

#endif // VEC3_H