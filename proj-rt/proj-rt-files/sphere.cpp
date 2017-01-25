#include "sphere.h"
#include "ray.h"


// Determine if the ray intersects with the sphere
bool Sphere::Intersection(const Ray& ray, std::vector<Hit>& hits) const
{
	vec3 c = this->center;
	double r = this->radius;
	vec3 u = ray.endpoint;
	vec3 w = ray.direction;
	vec3 v = u - c;


    // TODO
    return false;
}

vec3 Sphere::Normal(const vec3& point) const
{
    vec3 normal;
    // TODO: set the normal
    return normal;
}
