#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "vec.h"
#include "hit.h"
#include <vector>

// t has to be bigger than small_t to register an intersection with a ray.  You
// may need to tweak this value.
// http://stackoverflow.com/questions/17688360/ray-tracing-shadow-bug
static const double small_t = 1e-4;

class Ray;
class Shader;

class Object
{
public:
    Shader* material_shader;

    Object() :material_shader(0) {}
    virtual ~Object() {}

    // Check for an intersection in the range [0,inf).  Record all intersections
    // in order they are encountered, whether entering or leaving, in hits.  If
    // the ray begins inside the object, record that the ray enters the object
    // at time 0. In this way even entries indicate the ray entering, and odd
    // entries indicate that the ray is leaving.  Note that this differs from
    // the ray_exiting entry of the Hits structure in the case of Booleans,
    // which refers to the underlying primitive rather than the aggregate
    // object.
    virtual bool Intersection(const Ray& ray, std::vector<Hit>& hits) const=0;

    // Return the normal.  For objects with multiple parts (eg, cylinders), you
    // will need to use part to determine which piece was intersected.  This
    // routine should never be called for intersections recorded at time 0,
    // since these hits indicate that the ray started inside the object, not
    // that it actually intersects its surface.
    virtual vec3 Normal(const vec3& point) const=0;
};
#endif
