#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "object.h"
class Sphere : public Object
{
    vec3 center;
    double radius;

public:
    Sphere(const vec3& center_input,double radius_input)
        :center(center_input),radius(radius_input)
    {}

    virtual bool Intersection(const Ray& ray, std::vector<Hit>& hits) const override;
    virtual vec3 Normal(const vec3& point) const override;
};
#endif
