#include "sphere.h"
#include "ray.h"

// Determine if the ray intersects with the sphere
bool Sphere::Intersection(const Ray& ray, std::vector<Hit>& hits) const
{
	vec3 c = this->center;
	double r = this->radius;
	vec3 u = ray.endpoint;
	vec3 w = (ray.direction) / (ray.direction.magnitude());
	vec3 v = u - c;
	double temp = dot(w, v);
	double D = (temp * temp - (v.magnitude_squared() - r * r));
	if (D < 0)
	{
		return false;
	}
	else
	{
		double t0 = - dot(w, v) - sqrt(D);
		double t1 = - dot(w, v) + sqrt(D);
		if (t1 < 0)
		{
			return false;
		}
		Hit hit0, hit1;
		hit0.object = this;
		hit0.t = t0;
		hit0.ray_exiting = false;
		if (t0 < 0)
		{
			hit0.t = 0;
		}
		hit1.object = this;
		hit1.t = t1;
		hit1.ray_exiting = true;
		hits.push_back(hit0);
		hits.push_back(hit1);
		return true;
	}
}

vec3 Sphere::Normal(const vec3& point) const
{
	vec3 normal = (point - center).normalized();
	return normal;
}
