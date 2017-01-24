#ifndef __PLANE_H__
#define __PLANE_H__

#include "object.h"

class Plane : public Object
{
public:
    vec3 x1;
    vec3 normal;

    Plane(const vec3& point,const vec3& normal)
        :x1(point),normal(normal.normalized())
    {}

    virtual bool Intersection(const Ray& ray, std::vector<Hit>& hits) const override;
    virtual vec3 Normal(const vec3& point) const override;
};
#endif
