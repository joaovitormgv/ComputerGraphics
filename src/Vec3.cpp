#include "../include/Vec3.h"

Vec3::Vec3(float x_, float y_, float z_){
    x = x_;
    y = y_;
    z = z_;
}

Vec3 Vec3::operator + (const Vec3& v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator - (const Vec3& v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator * (float s) const {
    return Vec3(s * x, s * y, s * z);
}

float Vec3::dot(const Vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

float Vec3::norma() const {
    float n = this->dot(*this);
    return std::sqrt(n);
}
Vec3 Vec3::normalize() const {
    float n = this->norma();
    return Vec3(x / n, y / n, z / n);
}