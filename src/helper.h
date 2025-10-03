

#pragma once


struct Vector3
{
    double x, y, z;
};

struct mat3
{
    double a1, b1, c1;
    double a2, b2, c2;
    double a3, b3, c3;
};

Vector3 mul(mat3 a, Vector3 b);

inline Vector3& operator+=(Vector3 &a, Vector3 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

mat3 mul(const mat3& a, const mat3& b);





