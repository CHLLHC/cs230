#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::Shade_Surface(const Ray& ray,
		const vec3& intersection_point, const vec3& same_side_normal,
		int recursion_depth) const
{
	//Bugs somewhere
	vec3 color;
	vec3 C0 = shader->Shade_Surface(ray, intersection_point, same_side_normal,
			recursion_depth);
	vec3 C1;
	if (recursion_depth < world.recursion_depth_limit)
	{
		Ray reflect;
		reflect.direction = ray.direction
				+ 2 * dot(-ray.direction, same_side_normal) * same_side_normal;
		reflect.endpoint = intersection_point;
		C1 = world.Cast_Ray(reflect, recursion_depth + 1);
	}
	color = (1 - reflectivity) * C0 + reflectivity * C1;
	return color;
}
