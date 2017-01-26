#include "light.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "object.h"

vec3 Phong_Shader::Shade_Surface(const Ray& ray, const vec3& intersection_point,
		const vec3& same_side_normal, int recursion_depth) const
{
	vec3 color;
	for (std::vector<Light*>::const_iterator cit = world.lights.begin();
			cit != world.lights.end(); ++cit)
	{
		vec3 l = ((*cit)->position - intersection_point).normalized();
		bool SeeLight = true;
		if (world.enable_shadows)
		{
			Hit hit;
			Object* o_hit = 0;
			Ray shoot_to_light;
			shoot_to_light.direction = l;
			shoot_to_light.endpoint = intersection_point;
			o_hit = world.Closest_Intersection(shoot_to_light, hit);
			if (o_hit != 0)
			{
				double t_hit_light = ((*cit)->position - intersection_point).magnitude();
				if (hit.t < t_hit_light)
				{
					SeeLight = false;
				}
			}
		}
		if (SeeLight)
		{
			double four_pi = 4 * 3.14159265358979323846264338327950288;
			vec3 Ld =
					((*cit)->brightness
							/ (four_pi
									* (intersection_point - (*cit)->position).magnitude_squared()))
							* (*cit)->color;
			double lDOTn = dot(l, same_side_normal);
			vec3 Id;
			if (lDOTn > 0)
			{
				Id = color_diffuse * Ld * lDOTn;
			}

			vec3 Ls = Ld;
			vec3 r = -l + 2 * dot(l, same_side_normal) * same_side_normal;
			double cos_phi = dot(r, -ray.direction)
					/ (r.magnitude() * ray.direction.magnitude());
			vec3 Is;
			if (cos_phi > 0)
			{
				Is = color_specular * Ls * pow(cos_phi, specular_power);
			}
			color += Id + Is;
		}
	}
	vec3 Ia = color_ambient * world.ambient_color * world.ambient_intensity;
	color += Ia;
	return color;
}
