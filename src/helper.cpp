
#include "helper.h"

Vector3 mul(mat3 a, Vector3 b)
{
    Vector3 o;
    o.x = b.x*a.a1 + b.y*a.b1 + b.z*a.c1;
    o.y = b.x*a.a2 + b.y*a.b2 + b.z*a.c2;
    o.z = b.x*a.a3 + b.y*a.b3 + b.z*a.c3;
    return o;
}

mat3 mul(const mat3& a, const mat3& b)
{
    mat3 o;
    o.a1 = a.a1*b.a1 + a.b1*b.a2 + a.c1*b.a3;
    o.a2 = a.a2*b.a1 + a.b2*b.a2 + a.c2*b.a3;
    o.a3 = a.a3*b.a1 + a.b3*b.a2 + a.c3*b.a3;

    o.b1 = a.a1*b.b1 + a.b1*b.b2 + a.c1*b.b3;
    o.b2 = a.a2*b.b1 + a.b2*b.b2 + a.c2*b.b3;
    o.b3 = a.a3*b.b1 + a.b3*b.b2 + a.c3*b.b3;

    o.c1 = a.a1*b.c1 + a.b1*b.c2 + a.c1*b.c3;
    o.c2 = a.a2*b.c1 + a.b2*b.c2 + a.c2*b.c3;
    o.c3 = a.a3*b.c1 + a.b3*b.c2 + a.c3*b.c3;

    return o;
}




