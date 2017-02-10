#include "plane.h"
#include "ray.h"
#include <cfloat>

// Intersect with the half space defined by the plane.  The plane's normal
// points outside.  If the ray starts on the "inside" side of the plane, be sure
// to record a hit with t=0 as the first entry in hits.
bool Plane::Intersection(const Ray& ray, std::vector<Hit>& hits) const
{
	// TODO
	double DdotN = dot(ray.direction, normal);
	double Top = dot(x1 - ray.endpoint, normal);
	vec3 toPlane = x1 - ray.endpoint;
	double theDot = dot(toPlane, normal);
	if (theDot >= 0)
	{
		Hit hit1;
		hit1.object = this;
		hit1.ray_exiting = false;
		hit1.t = 0;
		hits.push_back(hit1);
		if (theDot != 0)
		{
			Hit hit2;
			hit2.object = this;
			hit2.ray_exiting = true;
			hit2.t = Top / DdotN;
			hits.push_back(hit2);
		}
		return true;
	}
	else
	{
		if (dot(ray.direction, normal) < 0)
		{
			Hit hit;
			hit.object = this;
			hit.ray_exiting = false;
			hit.t = Top / DdotN;
			hits.push_back(hit);
			return true;
		}
		return false;
	}
}

vec3 Plane::Normal(const vec3& point) const
{
	return normal;
}
