#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

#include <math.h>
#include <vector>
#include <iostream>
#include <limits>
#include "vec.h"
#include "light.h"

class Point_Light : public Light
{
public:
    Point_Light(const vec3& position,const vec3& color,double brightness)
        :Light(position,color,brightness)
    {}

    vec3 Emitted_Light(const Ray& ray) const
    {
        return color*brightness/(4*M_PI);
    }
};
#endif
